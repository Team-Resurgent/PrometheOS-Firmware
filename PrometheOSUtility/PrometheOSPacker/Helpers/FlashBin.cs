public static class FlashBin
{
    private static uint GetFlashAddressMask(uint biosSize)
    {
        var maskBits = 0u;

        biosSize -= 1;
        while (biosSize > 0)
        {
            biosSize >>= 1;
            maskBits++;
        }

        var memCapacity = (uint)(1 << (int)maskBits);
        var addressMask = memCapacity - 1;
        return addressMask;
    }

    private static List<Block> GetUF2BlocksFromData(byte[] data, uint flashRomAddress)
    {
        var blocks = new List<Block>();

        var blockAddress = flashRomAddress;

        using var ms = new MemoryStream(data);
        while (true)
        {
            var currentBlock = new Block(blockAddress);
            var buffer = new byte[currentBlock.BlockSize];
            var bytesRead = ms.Read(buffer, 0, currentBlock.BlockSize);
            if (bytesRead == 0)
            {
                break;
            }

            currentBlock.SetBlockBytes(buffer);
            blocks.Add(currentBlock);
            blockAddress += (uint)currentBlock.BlockSize;
        }
        return blocks;
    }

    private static List<Block> GetUF2RomaddrMaskBlock(uint maskAddress, uint romaddrMask)
    {
        var blocks = new List<Block>();
        var currentBlock = new Block(maskAddress);
        currentBlock.SetBlockBytes(BitConverter.GetBytes(romaddrMask));

        // Copy 16 times a 256 block to fill a 4096 byte sector
        for (var i = 0; i < 16; i++)
        {
            blocks.Add(currentBlock);
        }

        return blocks;
    }

    private static byte[] CombineByteArrays(byte[] first, byte[] second)
    {
        byte[] combined = new byte[first.Length + second.Length];
        Buffer.BlockCopy(first, 0, combined, 0, first.Length);
        Buffer.BlockCopy(second, 0, combined, first.Length, second.Length);
        return combined;
    }

    public static byte[] ProcessUf2(byte[] data, uint baseAddress, uint maskAddress, uint familyId)
    {
        var uf2Blocks = new List<Block>();
        var byteStream = Array.Empty<byte>();

        uint romaddrMask = GetFlashAddressMask(256 * 1024);
        if (maskAddress != 0xffffffff)
        {
            uf2Blocks.AddRange(GetUF2RomaddrMaskBlock(maskAddress, romaddrMask));
        }

        uf2Blocks.AddRange(GetUF2BlocksFromData(data, baseAddress));

        int totalBlocks = uf2Blocks.Count;
        int currentBlock = 0;

        foreach (var block in uf2Blocks)
        {
            byteStream = CombineByteArrays(byteStream, block.Encode(currentBlock, totalBlocks, familyId));
            currentBlock++;
        }

        Console.WriteLine("Total blocks written: " + totalBlocks);

        return byteStream;
    }
}
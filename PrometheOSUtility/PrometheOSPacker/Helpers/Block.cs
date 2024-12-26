using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class Block
{
    private const int BLOCK_SIZE = 256;

    private uint mAddress;
    private byte[] mBlockBytes = new byte[BLOCK_SIZE];

    public int BlockSize { get; } = BLOCK_SIZE;

    public Block(uint address)
    {
        mAddress = address;
    }

    public void SetBlockBytes(byte[] bytes)
    {
        Array.Clear(mBlockBytes, 0, BLOCK_SIZE);
        Array.Copy(bytes, mBlockBytes, bytes.Length);
    }

    public byte[] Encode(int blockno, int numblocks, uint familyId)
    {
        const uint UF2_MAGIC_START0 = 0x0A324655;
        const uint UF2_MAGIC_START1 = 0x9E5D5157;
        const uint UF2_MAGIC_END = 0x0AB16F30;
        const uint flags = 0x2000; // withFamilyId flag

        using var memoryStream = new MemoryStream();
        using var binaryWriter = new BinaryWriter(memoryStream);

        binaryWriter.Write(UF2_MAGIC_START0);
        binaryWriter.Write(UF2_MAGIC_START1);
        binaryWriter.Write(flags);
        binaryWriter.Write(mAddress);
        binaryWriter.Write(BlockSize);
        binaryWriter.Write(blockno);
        binaryWriter.Write(numblocks);
        binaryWriter.Write(familyId);
        binaryWriter.Write(mBlockBytes);

        // Pad with zeros
        while (memoryStream.Position < (512 - sizeof(uint)))
        {
            binaryWriter.Write((byte)0);
        }

        binaryWriter.Write(UF2_MAGIC_END);
        return memoryStream.ToArray();
    }
}

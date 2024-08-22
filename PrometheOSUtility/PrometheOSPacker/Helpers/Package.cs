using FluentFTP;
using SixLabors.ImageSharp.PixelFormats;
using SixLabors.ImageSharp.Processing;
using System.Net;
using System.Net.Http.Headers;
using System.Text;

namespace PrometheOSPacker.Helpers
{
    //Bank1_256k      =  3  0x000000 - 0x03ffff  Sector Size 65536  Total  262144
    //Bank2_256k      =  4  0x040000 - 0x07ffff  Sector Size 65536  Total  262144
    //Bank3_256k      =  5  0x080000 - 0x0bffff  Sector Size 65536  Total  262144
    //Bank4_256k      =  6  0x0c0000 - 0x0fffff  Sector Size 65536  Total  262144
    //Bank1_512k      =  7  0x000000 - 0x07ffff  Sector Size 65536  Total  524288
    //Bank2_512k      =  8  0x080000 - 0x0fffff  Sector Size 65536  Total  524288
    //Bank1_1024k     =  9  0x000000 - 0x0fffff  Sector Size 65536  Total 1048576
    //Bank_PrometheOS =  2  0x100000 - 0x17ffff  Sector Size 65536  Total  524288
    //Bank_Bootloader =  1  0x180000 - 0x1bffff  Sector Size 65536  Total  262144
    //Bank_Recovery   = 10  0x1c0000 - 0x1fffff  See Below          Total  262144

    //0x1c0000 - 0x1effff  Sector Size 65536  Total 196608 (PrometheOS Continued)
    //0x1f0000 - 0x1f7fff  Sector Size 32768  Total 32768  (Installer Logo)
    //0x1f8000 - 0x1f9fff  Sector Size 8192   Total 8192   (Settings)
    //0x1fa000 - 0x1fbfff  Sector Size 8192   Total 8192   (Spare)
    //0x1fc000 - 0x1fffff  Sector Size 16384  Total 16384  (Spare)

    //Suggested byes for PrometheOS = 720896 (524288 bytes Bank_PrometheOS + First 196608 bytes of Bank_Recovery)

    internal class Package
    {
        public static byte[] CompressBank(string filePath, int size)
        {
            var fileData = File.ReadAllBytes(filePath);
            var compressedData = new byte[fileData.Length];
            var compressedSize = K4os.Compression.LZ4.LZ4Codec.Encode(fileData, compressedData, K4os.Compression.LZ4.LZ4Level.L12_MAX);
            if (compressedSize <= 0)
            {
                throw new Exception("Compress failed.");
            }
            byte[] promethosLengthBytes = BitConverter.GetBytes(fileData.Length);
            byte[] compressedLengthBytes = BitConverter.GetBytes(compressedSize);

            var bank = new byte[size];
            Array.Copy(promethosLengthBytes, 0, bank, 0, promethosLengthBytes.Length);
            Array.Copy(compressedLengthBytes, 0, bank, 4, compressedLengthBytes.Length);
            Array.Copy(compressedData, 0, bank, 8, compressedSize);

            var checksum = 0;
            for (int i = 0; i < bank.Length; i++)
            {
                checksum = checksum + bank[i] & 0xffff;
            }

            Console.WriteLine($"Compressed Size = {compressedSize + 8} of {size} - Checksum {checksum}");

            return bank;
        }

        private static string GetNewestBios(string folderPath)
        {
            var directoryInfo = new DirectoryInfo(folderPath);
            var files = directoryInfo.GetFiles("*.bin");
            var newestFile = files?.OrderByDescending(f => f.CreationTime).FirstOrDefault();
            if (newestFile == null)
            {
                throw new Exception("There does not seem to be any bios files in folder.");
            }
            return newestFile.FullName;
        }

        public static byte[] GetInstallerLogo()
        {
            var imageFileData = File.ReadAllBytes("nemesis.png");
            var image = SixLabors.ImageSharp.Image.Load<Argb32>(imageFileData);
            image.Mutate(i => i.Resize(178, 46));

            var result = new byte[32768];

            result[0] = (byte)'I';
            result[1] = (byte)'M';
            result[2] = 178;
            result[3] = 46;

            var offset = 4;
            for (int y = 0; y < 46; y++)
            {
                for (int x = 0; x < 178; x++)
                {
                    result[offset + 0] = image[x, y].R;
                    result[offset + 1] = image[x, y].G;
                    result[offset + 2] = image[x, y].B;
                    result[offset + 3] = image[x, y].A;
                    offset += 4;
                }
            }

            return result;
        }

        public static byte[] ExtractPrometheOS(string modchip)
        {
            var slnFolder = Utility.GetSlnFolder();
            var prometheosSourcePath = Path.GetFullPath($"..\\Input\\prometheos-{modchip}.bin", slnFolder);
            if (File.Exists(prometheosSourcePath) == false)
            {
                Console.WriteLine($"Error: Looks like you didnt suppliy prometheos-{modchip}.bin in input folder.");
                throw new Exception("Failed: ExtractPrometheOS");
            }

            var prometheosSource = File.ReadAllBytes(prometheosSourcePath);
            var firmware = new byte[256 * 1024];

            if (modchip.Equals("Xenium", StringComparison.CurrentCultureIgnoreCase))
            {
                Array.Copy(prometheosSource, 0x180000, firmware, 0, 256 * 1024);
                return firmware;
            }

            if (modchip.Equals("Xecuter", StringComparison.CurrentCultureIgnoreCase))
            {
                Array.Copy(prometheosSource, 0x000000, firmware, 0, 256 * 1024);
                return firmware;
            }

            if (modchip.Equals("Xchanger", StringComparison.CurrentCultureIgnoreCase))
            {
                Array.Copy(prometheosSource, 0x0C0000, firmware, 0, 256 * 1024);
                return firmware;
            }

            if (modchip.Equals("Aladdin1mb", StringComparison.CurrentCultureIgnoreCase))
            {
                Array.Copy(prometheosSource, 0x0C0000, firmware, 0, 256 * 1024);
                return firmware;
            }

            if (modchip.Equals("Aladdin2mb", StringComparison.CurrentCultureIgnoreCase))
            {
                Array.Copy(prometheosSource, 0x100000, firmware, 0, 256 * 1024);
                return firmware;
            }

            if (modchip.StartsWith("Modxo", StringComparison.CurrentCultureIgnoreCase))
            {
                Array.Copy(prometheosSource, 0x040000, firmware, 0, 256 * 1024);
                return firmware;
            }

            throw new Exception($"Failed: Unrecognized modchip {modchip}.");
        }

        public static void PackageTools()
        {
            var slnFolder = Utility.GetSlnFolder();
            var promethosToolsXbePath = Path.GetFullPath($"..\\PrometheOSXbe\\PrometheOSXbe\\Release-Tools\\PrometheOSTools.xbe", slnFolder);
            var tools = File.ReadAllBytes(promethosToolsXbePath);

            var buildPath = Path.Combine(slnFolder, "..\\Build");
            Directory.CreateDirectory(buildPath);
            File.WriteAllBytes(Path.Combine(buildPath, $"prometheos-tools.xbe"), tools);
        }

        public static void PackageXenium(string modchip)
        {
            var slnFolder = Utility.GetSlnFolder();
            var prometheos = ExtractPrometheOS(modchip);

            var promethosXbePath = Path.GetFullPath($"..\\PrometheOSXbe\\PrometheOSXbe\\Release-{modchip}\\PrometheOS.xbe", slnFolder);
            if (File.Exists(promethosXbePath) == false)
            {
                Console.WriteLine("Error: Looks like you didnt build PrometheOSXbe.");
                return;
            }

            Console.WriteLine("Packing PrometheOS Xbe...");
            var promethosxbeBank = CompressBank(promethosXbePath, (512 + 96) * 1024);

            var firmware = new byte[2048 * 1024];
            Array.Copy(promethosxbeBank, 0, firmware, 0x100000, 512 * 1024);
            Array.Copy(promethosxbeBank, 512 * 1024, firmware, 0x1e0000, 96 * 1024);
            Array.Copy(prometheos, 0, firmware, 0x180000, prometheos.Length);

            //var logo = GetInstallerLogo();
            //Array.Copy(logo, 0, firmware, 0x1F0000 , logo.Length);

            var buildPath = Path.Combine(slnFolder, "..\\Build");
            Directory.CreateDirectory(buildPath);
            File.WriteAllBytes(Path.Combine(buildPath, $"prometheos-{modchip.ToLower()}.bin"), firmware);
        }

        public static void PackageXecuter(string modchip)
        {
            var slnFolder = Utility.GetSlnFolder();
            var prometheos = ExtractPrometheOS(modchip);

            var promethosXbePath = Path.GetFullPath($"..\\PrometheOSXbe\\PrometheOSXbe\\Release-{modchip}\\PrometheOS.xbe", slnFolder);
            if (File.Exists(promethosXbePath) == false)
            {
                Console.WriteLine("Error: Looks like you didnt build PrometheOSXbe.");
                return;
            }

            Console.WriteLine("Packing PrometheOS Xbe...");
            var promethosxbeBank = CompressBank(promethosXbePath, 655360);

            var firmware = new byte[2048 * 1024];
            Array.Copy(promethosxbeBank, 0, firmware, 0x040000, promethosxbeBank.Length);
            Array.Copy(prometheos, 0, firmware, 0x000000, prometheos.Length);

            //var logo = GetInstallerLogo();
            //Array.Copy(logo, 0, firmware, 0x0e0000 , logo.Length);

            var buildPath = Path.Combine(slnFolder, "..\\Build");
            Directory.CreateDirectory(buildPath);
            File.WriteAllBytes(Path.Combine(buildPath, $"prometheos-{modchip.ToLower()}.bin"), firmware);
        }

        public static void PackageXchanger(string modchip)
        {
            var slnFolder = Utility.GetSlnFolder();
            var prometheos = ExtractPrometheOS(modchip);

            var promethosXbePath = Path.GetFullPath($"..\\PrometheOSXbe\\PrometheOSXbe\\Release-{modchip}\\PrometheOS.xbe", slnFolder);
            if (File.Exists(promethosXbePath) == false)
            {
                Console.WriteLine("Error: Looks like you didnt build PrometheOSXbe.");
                return;
            }

            Console.WriteLine("Packing PrometheOS Xbe...");
            var promethosxbeBank = CompressBank(promethosXbePath, 508 * 1024);

            var firmware = new byte[1024 * 1024];
            Array.Copy(promethosxbeBank, 0, firmware, 0x000000, 508 * 1024);
            Array.Copy(prometheos, 0, firmware, 0x0C0000, prometheos.Length);


            var buildPath = Path.Combine(slnFolder, "..\\Build");
            Directory.CreateDirectory(buildPath);
            File.WriteAllBytes(Path.Combine(buildPath, $"prometheos-{modchip.ToLower()}.bin"), firmware);
        }

        private static void PackageModxoVariant(string variant, string modchip)
        {
            var slnFolder = Utility.GetSlnFolder();
            var prometheos = ExtractPrometheOS($"{modchip}-{variant}");

            var promethosXbePath = Path.GetFullPath($"..\\PrometheOSXbe\\PrometheOSXbe\\Release-{modchip}\\PrometheOS.xbe", slnFolder);
            if (File.Exists(promethosXbePath) == false)
            {
                Console.WriteLine("Error: Looks like you didnt build PrometheOSXbe.");
                return;
            }

            Console.WriteLine("Packing PrometheOS Xbe...");
            var promethosxbeBank = CompressBank(promethosXbePath, 0x080000 - 0x1000);

            var firmware = new byte[1 * 1024 * 1024];
            Array.Copy(promethosxbeBank, 0, firmware, 0x080000, promethosxbeBank.Length);
            Array.Copy(prometheos, 0, firmware, 0x040000, prometheos.Length);

            var modxo = File.ReadAllBytes($"modxo-{variant}.bin");
            Array.Copy(modxo, 0, firmware, 0x000000, modxo.Length);

            var buildPath = Path.Combine(slnFolder, "..\\Build");
            Directory.CreateDirectory(buildPath);
            File.WriteAllBytes(Path.Combine(buildPath, $"prometheos-{modchip.ToLower()}-{variant.ToLower()}.bin"), firmware);

            var uf2 = FlashBin.ProcessUf2(firmware, 0x10000000, 0xffffffff);
            File.WriteAllBytes(Path.Combine(buildPath, $"prometheos-{modchip.ToLower()}-{variant.ToLower()}.uf2"), uf2);
        }

        public static void PackageModxo(string modchip)
        {
            PackageModxoVariant("official-pico", modchip);
            PackageModxoVariant("rp2040-zero", modchip);
            PackageModxoVariant("yd-rp2040", modchip);

            if (File.Exists("modxo-custom.bin"))
            {
                PackageModxoVariant("custom", modchip);
            }
        }

        public static void PackageAladdin1mb(string modchip)
        {
            var slnFolder = Utility.GetSlnFolder();
            var prometheos = ExtractPrometheOS(modchip);

            var promethosXbePath = Path.GetFullPath($"..\\PrometheOSXbe\\PrometheOSXbe\\Release-{modchip}\\PrometheOS.xbe", slnFolder);
            if (File.Exists(promethosXbePath) == false)
            {
                Console.WriteLine("Error: Looks like you didnt build PrometheOSXbe.");
                return;
            }

            Console.WriteLine("Packing PrometheOS Xbe...");
            var promethosxbeBank = CompressBank(promethosXbePath, 508 * 1024);

            var firmware = new byte[1024 * 1024];
            Array.Copy(promethosxbeBank, 0, firmware, 0x000000, 508 * 1024);
            Array.Copy(prometheos, 0, firmware, 0x0C0000, prometheos.Length);

            var buildPath = Path.Combine(slnFolder, "..\\Build");
            Directory.CreateDirectory(buildPath);
            File.WriteAllBytes(Path.Combine(buildPath, $"prometheos-{modchip.ToLower()}.bin"), firmware);
        }

        public static void PackageAladdin2mb(string modchip)
        {
            var slnFolder = Utility.GetSlnFolder();
            var prometheos = ExtractPrometheOS(modchip);

            var promethosXbePath = Path.GetFullPath($"..\\PrometheOSXbe\\PrometheOSXbe\\Release-{modchip}\\PrometheOS.xbe", slnFolder);
            if (File.Exists(promethosXbePath) == false)
            {
                Console.WriteLine("Error: Looks like you didnt build PrometheOSXbe.");
                return;
            }

            Console.WriteLine("Packing PrometheOS Xbe...");
            var promethosxbeBank = CompressBank(promethosXbePath, (768 - 36) * 1024);

            var firmware = new byte[2 * 1024 * 1024];
            Array.Copy(promethosxbeBank, 0, firmware, 0x140000, (768 - 36) * 1024);
            Array.Copy(prometheos, 0, firmware, 0x100000, prometheos.Length);

            var buildPath = Path.Combine(slnFolder, "..\\Build");
            Directory.CreateDirectory(buildPath);
            File.WriteAllBytes(Path.Combine(buildPath, $"prometheos-{modchip.ToLower()}.bin"), firmware);
        }

        public static void FtpXeniumOS()
        {
            var slnFolder = Utility.GetSlnFolder();
            var buildPath = Path.Combine(slnFolder, "Build");

            try
            {
                using (FtpClient ftpClient = new FtpClient("ftp://xenium.local", "admin", "Blu3P3t3r!"))
                {
                    ftpClient.Connect();
                    ftpClient.UploadFile(Path.Combine(buildPath, "xeniumos.bin"), "/home/admin/xenium-programmer/xenium-bin/xeniumos.bin");
                    Console.WriteLine($"File uploaded successfully");
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error uploading file: {ex.Message}");
            }
        }
    }
}

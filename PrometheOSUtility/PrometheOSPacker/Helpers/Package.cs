using FluentFTP;
using SixLabors.ImageSharp.PixelFormats;
using SixLabors.ImageSharp.Processing;

namespace PrometheOSPacker.Helpers
{
    internal class Package
    {
        public static byte[] CompressBank(string filePath, int size)
        {
            var fileData = File.ReadAllBytes(filePath);

            if (LZXToolkit.LZXPacker.Compress(fileData, out var compressedData) == false)
            {
                throw new Exception("Compress failed.");
            }

            byte[] promethosLengthBytes = BitConverter.GetBytes(fileData.Length);
            byte[] compressedLengthBytes = BitConverter.GetBytes(compressedData.Length);

            var bank = new byte[size];
            Array.Copy(promethosLengthBytes, 0, bank, 0, promethosLengthBytes.Length);
            Array.Copy(compressedLengthBytes, 0, bank, 4, compressedLengthBytes.Length);
            Array.Copy(compressedData, 0, bank, 8, compressedData.Length);

            var checksum = 0;
            for (int i = 0; i < bank.Length; i++)
            {
                checksum = checksum + bank[i] & 0xffff;
            }

            Console.WriteLine($"Compressed Size = {compressedData.Length + 8} of {size} - Checksum {checksum}");

            return bank;
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

            if (modchip.Equals("Smartxx", StringComparison.CurrentCultureIgnoreCase))
            {
                Array.Copy(prometheosSource, 0x100000, firmware, 0, 256 * 1024);
                return firmware;
            }

            if (modchip.Equals("SmartxxOpx", StringComparison.CurrentCultureIgnoreCase))
            {
                Array.Copy(prometheosSource, 0x100000, firmware, 0, 256 * 1024);
                return firmware;
            }

            throw new Exception($"Failed: Unrecognized modchip {modchip}.");
        }

        public static void PackageTools()
        {
            var slnFolder = Utility.GetSlnFolder();
            var promethosToolsXbePath = Path.GetFullPath($"..\\PrometheOSXbe\\PrometheOSXbe\\Release-Tools\\PrometheOS-Tools.xbe", slnFolder);
            var tools = File.ReadAllBytes(promethosToolsXbePath);

            var buildPath = Path.Combine(slnFolder, "Build");
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
            Array.Fill<byte>(firmware, 0xff, 0, firmware.Length);
            Array.Copy(promethosxbeBank, 0, firmware, 0x100000, 512 * 1024);
            Array.Copy(promethosxbeBank, 512 * 1024, firmware, 0x1e0000, 96 * 1024);
            Array.Copy(prometheos, 0, firmware, 0x180000, prometheos.Length);

            var buildPath = Path.Combine(slnFolder, "Build");
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
            Array.Fill<byte>(firmware, 0xff, 0, firmware.Length);
            Array.Copy(promethosxbeBank, 0, firmware, 0x040000, promethosxbeBank.Length);
            Array.Copy(prometheos, 0, firmware, 0x000000, prometheos.Length);

            var buildPath = Path.Combine(slnFolder, "Build");
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
            Array.Fill<byte>(firmware, 0xff, 0, firmware.Length);
            Array.Copy(promethosxbeBank, 0, firmware, 0x000000, 508 * 1024);
            Array.Copy(prometheos, 0, firmware, 0x0C0000, prometheos.Length);


            var buildPath = Path.Combine(slnFolder, "Build");
            Directory.CreateDirectory(buildPath);
            File.WriteAllBytes(Path.Combine(buildPath, $"prometheos-{modchip.ToLower()}.bin"), firmware);
        }

        public static void PackageSmartxxOpx(string modchip)
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
            var promethosxbeBank = CompressBank(promethosXbePath, (768 - 64) * 1024);

            var firmware = new byte[2 * 1024 * 1024];
            Array.Fill<byte>(firmware, 0xff, 0, firmware.Length);
            Array.Copy(promethosxbeBank, 0, firmware, 0x110000, (768 - 64) * 1024);
            Array.Copy(prometheos, 0, firmware, 0x1c0000, prometheos.Length);

            var buildPath = Path.Combine(slnFolder, "Build");
            Directory.CreateDirectory(buildPath);
            File.WriteAllBytes(Path.Combine(buildPath, $"prometheos-{modchip.ToLower()}.bin"), firmware);
        }

        public static void PackageSmartxx(string modchip)
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
            var promethosxbeBank = CompressBank(promethosXbePath, (1024 - 64) * 1024);

            var firmware = new byte[4 * 1024 * 1024];
            Array.Fill<byte>(firmware, 0xff, 0, firmware.Length);
            Array.Copy(promethosxbeBank, 0, firmware, 0x200000, (1024 - 64) * 1024);
            Array.Copy(prometheos, 0, firmware, 0x1c0000, prometheos.Length);

            var buildPath = Path.Combine(slnFolder, "Build");
            Directory.CreateDirectory(buildPath);
            File.WriteAllBytes(Path.Combine(buildPath, $"prometheos-{modchip.ToLower()}.bin"), firmware);
        }

        private static void PackageModxoVariant(byte[] prometheos, string variant, string modchip, uint familyId)
        {
            var slnFolder = Utility.GetSlnFolder();

            var promethosXbePath = Path.GetFullPath($"..\\PrometheOSXbe\\PrometheOSXbe\\Release-{modchip}\\PrometheOS.xbe", slnFolder);
            if (File.Exists(promethosXbePath) == false)
            {
                Console.WriteLine("Error: Looks like you didnt build PrometheOSXbe.");
                return;
            }

            Console.WriteLine("Packing PrometheOS Xbe...");
            var promethosxbeBank = CompressBank(promethosXbePath, 0x080000 - 0x1000);

            var firmware = new byte[1 * 1024 * 1024];
            Array.Fill<byte>(firmware, 0xff, 0, firmware.Length);
            Array.Copy(promethosxbeBank, 0, firmware, 0x080000, promethosxbeBank.Length);
            Array.Copy(prometheos, 0, firmware, 0x040000, prometheos.Length);

            var modxo = File.ReadAllBytes($"modxo-{variant}.bin");
            Array.Copy(modxo, 0, firmware, 0x000000, modxo.Length);

            var buildPath = Path.Combine(slnFolder, "Build");
            Directory.CreateDirectory(buildPath);
            File.WriteAllBytes(Path.Combine(buildPath, $"prometheos-{modchip.ToLower()}-{variant.ToLower()}.bin"), firmware);

            var uf2 = FlashBin.ProcessUf2(firmware, 0x10000000, 0xffffffff, familyId);
            File.WriteAllBytes(Path.Combine(buildPath, $"prometheos-{modchip.ToLower()}-{variant.ToLower()}.uf2"), uf2);
        }

        public static void PackageModxo(string modchip)
        {
            var slnFolder = Utility.GetSlnFolder();
            var prometheos = ExtractPrometheOS($"{modchip}-official-pico");
            uint defaultFamilyId = 0xE48BFF56;

            PackageModxoVariant(prometheos, "official-pico", modchip, defaultFamilyId);
            PackageModxoVariant(prometheos, "official-pico2", modchip, 0xE48BFF59);
            PackageModxoVariant(prometheos, "rp2040-zero-tiny", modchip, defaultFamilyId);
            PackageModxoVariant(prometheos, "yd-rp2040", modchip, defaultFamilyId);
            PackageModxoVariant(prometheos, "xiao-rp2040", modchip, defaultFamilyId);

            if (File.Exists("modxo-custom.bin"))
            {
                PackageModxoVariant(prometheos, "custom", modchip, defaultFamilyId);
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
            Array.Fill<byte>(firmware, 0xff, 0, firmware.Length);
            Array.Copy(promethosxbeBank, 0, firmware, 0x000000, 508 * 1024);
            Array.Copy(prometheos, 0, firmware, 0x0C0000, prometheos.Length);

            var buildPath = Path.Combine(slnFolder, "Build");
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
            Array.Fill<byte>(firmware, 0xff, 0, firmware.Length);
            Array.Copy(promethosxbeBank, 0, firmware, 0x140000, (768 - 36) * 1024);
            Array.Copy(prometheos, 0, firmware, 0x100000, prometheos.Length);

            var buildPath = Path.Combine(slnFolder, "Build");
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

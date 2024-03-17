using FluentFTP;

namespace PrometheOSPacker.Helpers
{
    internal class Package
    {
        private static byte[] CompressBank(string filePath, int size)
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

            Console.WriteLine($"Compressed Size = {compressedSize} of {size}\n");

            return bank;
        }

        public static bool PackageXenium(string modchip)
        {
            var slnFolder = Utility.GetSlnFolder();
            var promethosXbePath = Path.GetFullPath($"..\\PrometheOSXbe\\PrometheOSXbe\\Release-{modchip}\\PrometheOSXbe.xbe", slnFolder);
            if (File.Exists(promethosXbePath) == false)
            {
                Console.WriteLine("Error: Looks like you didnt build PrometheOSXbe.");
                return false;
            }

            var prometheosSourcePath = Path.GetFullPath($"..\\Input\\prometheos-{modchip}.bin", slnFolder);
            if (File.Exists(prometheosSourcePath) == false)
            {
                Console.WriteLine($"Error: Looks like you didnt suppliy prometheos-{modchip}.bin in input folder.");
                return false;
            }

            Console.WriteLine("Packing PrometheOS Xbe...");
            var promethosxbeBank = CompressBank(promethosXbePath, (512 + 96) * 1024);

            var firmware = new byte[2048 * 1024];

            var prometheosSource = File.ReadAllBytes(prometheosSourcePath);
            if (prometheosSource.Length != firmware.Length)
            {
                Console.WriteLine($"Error: Looks like prometheos-{modchip}.bin is invalid length.");
                return false;
            }

            Array.Copy(promethosxbeBank, 0x000000, firmware, 0x100000, 512 * 1024);
            Array.Copy(promethosxbeBank, 512 * 1024, firmware, 0x1e0000, 96 * 1024);
            Array.Copy(prometheosSource, 0x180000, firmware, 0x180000, 256 * 1024);

            var buildPath = Path.Combine(slnFolder, "..\\Build");
            Directory.CreateDirectory(buildPath);

            File.WriteAllBytes(Path.Combine(buildPath, $"prometheos-{modchip}.bin"), firmware);
            return true;
        }

        public static bool PackageXecuter(string modchip)
        {
            var slnFolder = Utility.GetSlnFolder();
            var promethosXbePath = Path.GetFullPath($"..\\PrometheOSXbe\\PrometheOSXbe\\Release-{modchip}\\PrometheOSXbe.xbe", slnFolder);
            if (File.Exists(promethosXbePath) == false)
            {
                Console.WriteLine("Error: Looks like you didnt build PrometheOSXbe.");
                return false;
            }

            var prometheosSourcePath = Path.GetFullPath($"..\\Input\\prometheos-{modchip}.bin", slnFolder);
            if (File.Exists(prometheosSourcePath) == false)
            {
                Console.WriteLine($"Error: Looks like you didnt suppliy prometheos-{modchip}.bin in input folder.");
                return false;
            }

            Console.WriteLine("Packing PrometheOS Xbe...");
            var promethosxbeBank = CompressBank(promethosXbePath, 655360);

            var firmware = new byte[2048 * 1024];

            var prometheosSource = File.ReadAllBytes(prometheosSourcePath);
            if (prometheosSource.Length != firmware.Length)
            {
                Console.WriteLine($"Error: Looks like prometheos-{modchip}.bin is invalid length.");
                return false;
            }

            Array.Copy(promethosxbeBank, 0x000000, firmware, 0x040000, promethosxbeBank.Length);
            Array.Copy(prometheosSource, 0x000000, firmware, 0x000000, 256 * 1024);

            var buildPath = Path.Combine(slnFolder, "..\\Build");
            Directory.CreateDirectory(buildPath);

            File.WriteAllBytes(Path.Combine(buildPath, $"prometheos-{modchip}.bin"), firmware);
            return true;
        }

        public static bool PackageXchanger(string modchip)
        {
            var slnFolder = Utility.GetSlnFolder();
            var promethosXbePath = Path.GetFullPath($"..\\PrometheOSXbe\\PrometheOSXbe\\Release-{modchip}\\PrometheOSXbe.xbe", slnFolder);
            if (File.Exists(promethosXbePath) == false)
            {
                Console.WriteLine("Error: Looks like you didnt build PrometheOSXbe.");
                return false;
            }

            var prometheosSourcePath = Path.GetFullPath($"..\\Input\\prometheos-{modchip}.bin", slnFolder);
            if (File.Exists(prometheosSourcePath) == false)
            {
                Console.WriteLine($"Error: Looks like you didnt suppliy prometheos-{modchip}.bin in input folder.");
                return false;
            }

            Console.WriteLine("Packing PrometheOS Xbe...");
            var promethosxbeBank = CompressBank(promethosXbePath, 508 * 1024);

            var firmware = new byte[1024 * 1024];

            var prometheosSource = File.ReadAllBytes(prometheosSourcePath);
            if (prometheosSource.Length != firmware.Length)
            {
                Console.WriteLine($"Error: Looks like prometheos-{modchip}.bin is invalid length.");
                return false;
            }

            Array.Copy(promethosxbeBank, 0x000000, firmware, 0x000000, 508 * 1024);
            Array.Copy(prometheosSource, 0x0C0000, firmware, 0x0C0000, 256 * 1024);

            var buildPath = Path.Combine(slnFolder, "..\\Build");
            Directory.CreateDirectory(buildPath);

            File.WriteAllBytes(Path.Combine(buildPath, $"prometheos-{modchip}.bin"), firmware);
            return true;
        }

        public static bool PackageAladdin1mb(string modchip)
        {
            var slnFolder = Utility.GetSlnFolder();
            var promethosXbePath = Path.GetFullPath($"..\\PrometheOSXbe\\PrometheOSXbe\\Release-{modchip}\\PrometheOSXbe.xbe", slnFolder);
            if (File.Exists(promethosXbePath) == false)
            {
                Console.WriteLine("Error: Looks like you didnt build PrometheOSXbe.");
                return false;
            }

            var prometheosSourcePath = Path.GetFullPath($"..\\Input\\prometheos-{modchip}.bin", slnFolder);
            if (File.Exists(prometheosSourcePath) == false)
            {
                Console.WriteLine($"Error: Looks like you didnt suppliy prometheos-{modchip}.bin in input folder.");
                return false;
            }

            Console.WriteLine("Packing PrometheOS Xbe...");
            var promethosxbeBank = CompressBank(promethosXbePath, 508 * 1024);

            var firmware = new byte[1024 * 1024];

            var prometheosSource = File.ReadAllBytes(prometheosSourcePath);
            if (prometheosSource.Length != firmware.Length)
            {
                Console.WriteLine($"Error: Looks like prometheos-{modchip}.bin is invalid length.");
                return false;
            }

            Array.Copy(promethosxbeBank, 0x000000, firmware, 0x000000, 508 * 1024);
            Array.Copy(prometheosSource, 0x0C0000, firmware, 0x0C0000, 256 * 1024);

            var buildPath = Path.Combine(slnFolder, "..\\Build");
            Directory.CreateDirectory(buildPath);

            File.WriteAllBytes(Path.Combine(buildPath, $"prometheos-{modchip}.bin"), firmware);
            return true;
        }

        public static void FtpPrometheOS(string modchip, string ftpAddress, string user, string pass, string destFilePath)
        {
            var slnFolder = Utility.GetSlnFolder();
            var buildPath = Path.Combine(slnFolder, "..\\Build");

            try
            {
                using FtpClient ftpClient = new(ftpAddress, user, pass);
                ftpClient.Connect();
                ftpClient.UploadFile(Path.Combine(buildPath, $"prometheos-{modchip}.bin"), destFilePath);
                Console.WriteLine($"File uploaded successfully");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error uploading file: {ex.Message}");
            }
        }
    }
}

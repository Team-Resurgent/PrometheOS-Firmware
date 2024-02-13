using FluentFTP;
using SixLabors.ImageSharp.PixelFormats;
using SixLabors.ImageSharp.Processing;

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

            Console.WriteLine($"Compressed Size = {compressedSize} of {(512 + 192) * 1024}\n");

            return bank;
        }

        private static byte[] GetInstallerLogo(string? name)
        {
            var result = new byte[32768];
            if (string.IsNullOrEmpty(name))
            {
                return result;
            }

            var filename = $"{name}.png";
            if (File.Exists(filename))
            {
                var imageFileData = File.ReadAllBytes(filename);
                var image = SixLabors.ImageSharp.Image.Load<Argb32>(imageFileData);
                image.Mutate(i => i.Resize(178, 46));

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
            }
            return result;
        }

        public static bool Process(string installerName)
        {
            var slnFolder = Utility.GetSlnFolder();
            var promethosXbePath = Path.GetFullPath("..\\PrometheOSXbe\\PrometheOSXbe\\Release\\PrometheOSXbe.xbe", slnFolder);
            if (File.Exists(promethosXbePath) == false)
            {
                Console.WriteLine("Error: Looks like you didnt build PrometheOSXbe.");
                return false;
            }

            var prometheosSourcePath = Path.GetFullPath("..\\Input\\prometheos.bin", slnFolder);
            if (File.Exists(prometheosSourcePath) == false)
            {
                Console.WriteLine("Error: Looks like you didnt suppliy prometheos.bin in input folder.");
                return false;
            }

            var promethosxbeBank = CompressBank(promethosXbePath, (512 + 192) * 1024);

            var firmware = new byte[2048 * 1024];
            
            var prometheosSource = File.ReadAllBytes(prometheosSourcePath);
            if (prometheosSource.Length != firmware.Length)
            {
                Console.WriteLine("Error: Looks like prometheos.bin is invalid length.");
                return false;
            }

            Array.Copy(promethosxbeBank, 0, firmware, 0x100000, 512 * 1024);
            Array.Copy(promethosxbeBank, 512 * 1024, firmware, 0x1c0000, 192 * 1024);

            Array.Copy(prometheosSource, 0x180000, firmware, 0x180000, 256 * 1024);

            var logo = GetInstallerLogo(installerName);
            Array.Copy(logo, 0, firmware, 0x1F0000 , logo.Length);

            var buildPath = Path.Combine(slnFolder, "..\\Build");
            Directory.CreateDirectory(buildPath);

            File.WriteAllBytes(Path.Combine(buildPath, "prometheos.bin"), firmware);

            return true;
        }

        public static void FtpPrometheOS(string ftpAddress, string user, string pass, string destFilePath)
        {
            var slnFolder = Utility.GetSlnFolder();
            var buildPath = Path.Combine(slnFolder, "..\\Build");

            try
            {
                using FtpClient ftpClient = new(ftpAddress, user, pass);
                ftpClient.Connect();
                ftpClient.UploadFile(Path.Combine(buildPath, "prometheos.bin"), destFilePath);
                Console.WriteLine($"File uploaded successfully");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error uploading file: {ex.Message}");
            }
        }
    }
}

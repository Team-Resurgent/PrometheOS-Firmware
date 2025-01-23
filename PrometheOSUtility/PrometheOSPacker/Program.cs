using PrometheOSPacker.Helpers;
using System.Diagnostics;

namespace PrometheOSPacker
{
    internal class Program
    {
        static async Task<bool> DownloadFileAsync(string url, string filename)
        {
            using (HttpClient client = new HttpClient())
            {
                try
                {
                    byte[] fileBytes = await client.GetByteArrayAsync(url);
                    await File.WriteAllBytesAsync(filename, fileBytes);
                    return true;
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"An error occurre downloading file: {ex.Message}");
                    return false;
                }
            }
        }

        static void Main()
        {
            var version = "V1.0.8";
            //version = "latest";

            var baseUrl = $"https://github.com/Team-Resurgent/Modxo/releases/download/{version}";

            Console.WriteLine("Downloading 'modxo-official-pico.bin'.");
            if (DownloadFileAsync($"{baseUrl}/modxo_official_pico.bin", "modxo-official-pico.bin").GetAwaiter().GetResult() == false)
            {
                return;
            }
            Console.WriteLine("Downloading 'modxo-official-pico2.bin'.");
            if (DownloadFileAsync($"{baseUrl}/modxo_official_pico2.bin", "modxo-official-pico2.bin").GetAwaiter().GetResult() == false)
            {
                return;
            }
            Console.WriteLine("Downloading 'modxo-yd-rp2040.bin'.");
            if (DownloadFileAsync($"{baseUrl}/modxo_yd_rp2040.bin", "modxo-yd-rp2040.bin").GetAwaiter().GetResult() == false)
            {
                return;
            }
            Console.WriteLine("Downloading 'modxo-rp2040-zero-tiny.bin'.");
            if (DownloadFileAsync($"{baseUrl}/modxo_rp2040_zero_tiny.bin", "modxo-rp2040-zero-tiny.bin").GetAwaiter().GetResult() == false)
            {
                return;
            }
            Console.WriteLine("Downloading 'modxo-xiao-rp2040.bin'.");
            if (DownloadFileAsync($"{baseUrl}/modxo_xiao_rp2040.bin", "modxo-xiao-rp2040.bin").GetAwaiter().GetResult() == false)
            {
                return;
            }
            Console.WriteLine("Downloading 'modxo-ultra.bin'.");
            if (DownloadFileAsync($"{baseUrl}/modxo_ultra.bin", "modxo-ultra.bin").GetAwaiter().GetResult() == false)
            {
                return;
            }
            Console.WriteLine();

            var prometheosWebTestIp = "192.168.1.66"; // If you change ip in PrometheOSWeb update here

            Console.WriteLine("1) Updating embeded web files in XBE...");
            Minify.Process(prometheosWebTestIp);
            Console.WriteLine();

            Console.WriteLine("2) Please now build as Release PrometheOSTools and PrometheOSXbe for every modchip...");
            Console.WriteLine();
            Console.WriteLine("Press Enter when done.");
            Console.ReadLine();

            Console.WriteLine("3) Packaging PrometheOSTools and PrometheOS firmware for each modchip...");

            Package.PackageTools();

            var modchips = new string[]
            {
                "Aladdin1mb",
                "Aladdin2mb",
                "Xenium",
                "Xecuter",
                "Xchanger",
                "Aladdin1mb",
                "Aladdin2mb",
                "Modxo",
            };

            foreach (var modchip in modchips)
            {
                if (modchip.Equals("Xenium", StringComparison.CurrentCultureIgnoreCase))
                {
                    Package.PackageXenium(modchip);
                }

                if (modchip.Equals("Xecuter", StringComparison.CurrentCultureIgnoreCase))
                {
                    Package.PackageXecuter(modchip);
                }

                if (modchip.Equals("Xchanger", StringComparison.CurrentCultureIgnoreCase))
                {
                    Package.PackageXchanger(modchip);
                }

                if (modchip.Equals("Aladdin1mb", StringComparison.CurrentCultureIgnoreCase))
                {
                    Package.PackageAladdin1mb(modchip);
                }

                if (modchip.Equals("Aladdin2mb", StringComparison.CurrentCultureIgnoreCase))
                {
                    Package.PackageAladdin2mb(modchip);
                }

                if (modchip.Equals("Modxo", StringComparison.CurrentCultureIgnoreCase))
                {
                    Package.PackageModxo(modchip);
                }

                // Edit and enable below lines if you wish to ftp to xbox / xenium programmer
                //Console.WriteLine("4) FTP PrometheOS firmware...");
                //Package.FtpPrometheOS("127.0.0.1", "xbox", "xbox", $"/c/prometheos-{modchip}.bin");
            }

            Console.WriteLine("Done\n");
            Console.WriteLine();
            Console.WriteLine("Press Enter to finish.");
            Console.ReadLine();

        }
    }
}
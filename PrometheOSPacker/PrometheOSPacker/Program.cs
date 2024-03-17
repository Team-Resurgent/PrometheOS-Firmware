using PrometheOSPacker.Helpers;

namespace PrometheOSPacker
{
    internal class Program
    {
        static void Main()
        {
            var prometheosWebTestIp = "192.168.1.151"; // If you change ip in PrometheOSWeb update here
 
            Console.WriteLine("1) Updating embeded web files in XBE...");
            Minify.Process(prometheosWebTestIp);
            Console.WriteLine();

            Console.WriteLine("2) Please now build as Release PrometheOSXbe for every modchip...");
            Console.WriteLine();
            Console.WriteLine("Press Enter when done.");
            Console.ReadLine();

            Console.WriteLine("3) Packaging PrometheOS firmware for each modchip...");

            var modchips = new string[]
            {
                "Xenium",
                "Aladdin1mb",
                "Xecuter",
                "Xchanger"
            };

            foreach (var modchip in modchips)
            {
                Console.WriteLine($"Packaging PrometheOS for '{modchip}'...");

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
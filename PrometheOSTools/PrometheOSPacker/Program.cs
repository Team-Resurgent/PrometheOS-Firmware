using PrometheOSPacker.Helpers;

namespace PrometheOSPacker
{
    internal class Program
    {
        static void Main()
        {
            var prometheosWebTestIp = "192.168.1.151"; // If you change ip in PrometheOSWeb update here
            var installerName = ""; // Current choice of ace, andr0, booter, nemesis, this will be embedded in the about screen


            Console.WriteLine("1) Updating embeded web files in XBE...");
            Minify.Process(prometheosWebTestIp);
            Console.WriteLine();

            Console.WriteLine("2) Please now build as Release PrometheOSXbe...");
            Console.WriteLine();
            Console.WriteLine("Press Enter when done.");
            Console.ReadLine();

            Console.WriteLine("3) Packaging PrometheOS firmware...");
            bool success = Package.Process(installerName);
            Console.WriteLine(success ? "Done\n" : "Failed\n");

            // Edit and enable below lines if you wish to ftp to xbox / xenium programmer
            //Console.WriteLine("4) FTP PrometheOS firmware...");
            //Package.FtpPrometheOS("127.0.0.1", "xbox", "xbox", "/c/prometheos.bin");

            Console.WriteLine();
            Console.WriteLine("Press Enter to finish.");
            Console.ReadLine();
        }
    }
}
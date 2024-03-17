using BundlerMinifier;
using System.Text;

namespace PrometheOSPacker.Helpers
{
    internal static class Minify
    {
        private static void BundleFile(string sourceFolder, string destFolder, string filename)
        {
            var sourceFile = Path.Combine(sourceFolder, filename);
            var destFile = Path.Combine(destFolder, filename);

            if (Path.GetExtension(filename).Equals(".ico", StringComparison.OrdinalIgnoreCase))
            {
                File.Copy(sourceFile, destFile, true);
                return;
            }

            var bundleJson = Path.Combine(Utility.GetAppFolder(), "bundle.json");
            if (File.Exists(bundleJson))
            {
                File.Delete(bundleJson);
            }

            var bundle = new Bundle
            {
                IncludeInProject = false,
                OutputFileName = destFile
            };
            bundle.InputFiles.Add(sourceFile);

            BundleHandler.AddBundle(bundleJson, bundle);

            var bundleProcessor = new BundleFileProcessor();
            var bundles = BundleHandler.GetBundles(bundleJson);
            bundleProcessor.Process(bundleJson, bundles);

            var minFile = Path.Combine(destFolder, $"{Path.GetFileNameWithoutExtension(filename)}.min{Path.GetExtension(filename)}");
            if (File.Exists(minFile) == true && File.Exists(destFile))
            {
                File.Move(minFile, destFile, true);
            }

            File.Delete(bundleJson);
        }

        private static void IncludeFile(string ipToReplace, string sourceFolder, string destFolder, string filename)
        {
            var includeName = filename.Replace(".", "_");
            var headerName = $"{includeName}.h";

            var sourceFile = Path.Combine(sourceFolder, filename);
            var destFile = Path.Combine(destFolder, headerName);

            byte[] sourceData;
            if (Path.GetExtension(sourceFile).Equals(".ico", StringComparison.OrdinalIgnoreCase))
            {
                sourceData = File.ReadAllBytes(sourceFile);
            }
            else
            {
                var sourceText = File.ReadAllText(sourceFile);
                sourceText = sourceText.Replace($"http://{ipToReplace}", "");
                sourceData = Encoding.UTF8.GetBytes(sourceText);
            }

            var resultBuilder = new StringBuilder();
            resultBuilder.AppendLine("#pragma once");
            resultBuilder.AppendLine("");
            resultBuilder.AppendLine($"const uint8_t {includeName}[] = {{");

            for (var i = 0; i < sourceData.Length; i++)
            {
                if (i % 16 == 0)
                {
                    resultBuilder.Append("  ");
                }
                resultBuilder.Append("0x");
                resultBuilder.Append(sourceData[i].ToString("x2"));
                if (i < sourceData.Length - 1)
                {
                    resultBuilder.Append(',');
                    if (i % 16 != 15)
                    {
                        resultBuilder.Append(' ');
                    }
                }
                if (i % 16 == 15)
                {
                    resultBuilder.AppendLine();
                }
            }
            resultBuilder.AppendLine();
            resultBuilder.AppendLine("};");

            var result = resultBuilder.ToString();
            File.WriteAllText(destFile, result);
        }

        public static void Process(string ipToReplace)
        {
            var slnFolder = Utility.GetSlnFolder();

            var webFolder = Path.Combine(slnFolder, "PrometheOSWeb", "wwwroot");
            var bundleFolder = Path.Combine(slnFolder, "PrometheOSPacker", "bundle");
            Directory.CreateDirectory(bundleFolder);

            var webFiles = Directory.GetFiles(webFolder);
            foreach (var webFile in webFiles)
            {
                BundleFile(webFolder, bundleFolder, Path.GetFileName(webFile));
            }

            var includeFolder = Path.GetFullPath("..\\PrometheOSXbe\\PrometheOSXbe\\Assets\\Web", slnFolder);

            var bundleFiles = Directory.GetFiles(bundleFolder);
            foreach (var bundleFile in bundleFiles)
            {
                IncludeFile(ipToReplace, bundleFolder, includeFolder, Path.GetFileName(bundleFile));
            }
        }
    }
}

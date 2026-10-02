namespace PrometheOSPacker.Helpers
{
    internal static class Utility
    {
        public static string GetAppFolder()
        {
            return AppDomain.CurrentDomain.BaseDirectory;
        }

        public static string GetSlnFolder()
        {
            var directory = GetAppFolder();
            var files = Directory.GetFiles(directory, "*.sln");
            while (files.Length == 0)
            {
                var parentDirectoryInfo = Directory.GetParent(directory);
                if (parentDirectoryInfo == null)
                {
                    throw new Exception("Unable to get parent directory.");
                }
                directory = parentDirectoryInfo.FullName;
                files = Directory.GetFiles(directory, "*.sln");
            }
            return directory;
        }

        public static string GetRepoRoot()
        {
            var directory = GetSlnFolder();
            while (true)
            {
                if (Directory.Exists(Path.Combine(directory, ".git")))
                {
                    return directory;
                }

                var parentDirectoryInfo = Directory.GetParent(directory);
                if (parentDirectoryInfo == null)
                {
                    throw new Exception("Unable to find repository root.");
                }

                directory = parentDirectoryInfo.FullName;
            }
        }
    }
}

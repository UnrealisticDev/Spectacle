#include <filesystem>

struct FPaths
{
    namespace fs = std::filesystem;
    using path = fs::path;

    /** Creates the temporary directory if it does not exist. */
    static void CreateTempDirectory()
    {
        if (!fs::exists(TempDirectory()))
        {
            fs::create_directory(TempDirectory());
        }
    }

    /** Returns the path to the temporary directory. */
    static path TempDirectory()
    {
        static const path Dir = fs::temp_directory_path().append("Spectacle\\");
        return Dir;
    }

    /** Returns the path to the source directory. */
    static path SourceDirectory()
    {
        static const path Dir = TempDirectory().append("Source\\");
        return Dir;
    }

    /** Returns the path to the results directory. */
    static path ResultsDirectory()
    {
        static const path Dir = TempDirectory().append("Results\\");
        return Dir;
    }
};
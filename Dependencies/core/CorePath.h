#include <filesystem>

struct FPaths
{
    /** Returns the path to the temporary directory. */
    static std::filesystem::path TempDirectory()
    {
        static const std::filesystem::path Dir = std::filesystem::temp_directory_path().append("Spectacle\\");
        return Dir;
    }

    /** Creates the temporary directory if it does not exist. */
    static void CreateTempDirectory()
    {
        if ( !std::filesystem::exists(TempDirectory()) )
        {
            std::filesystem::create_directory(TempDirectory());
        }
    }
};
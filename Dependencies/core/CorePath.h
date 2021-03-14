#include <filesystem>

struct FPath
{
    /** Returns the path to the temporary directory. */
    static std::filesystem::path TempDirectory()
    {
        std::filesystem::path Dir = std::filesystem::temp_directory_path();
        Dir += +"Spectacle\\";
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
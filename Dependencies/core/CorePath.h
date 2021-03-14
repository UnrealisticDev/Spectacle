#pragma once

#include <filesystem>

struct FPaths
{
    /** Creates the temporary directory if it does not exist. */
    static void CreateTempDirectory()
    {
        if (!std::filesystem::exists(FPaths::TempDirectory()))
        {
            std::filesystem::create_directory(TempDirectory());
        }
    }

    /** Returns the path to the temporary directory. */
    static std::filesystem::path TempDirectory()
    {
        static const std::filesystem::path Dir = std::filesystem::temp_directory_path().append("Spectacle\\");
        return Dir;
    }

    /** Returns the path to the source directory. */
    static std::filesystem::path SourceDirectory()
    {
        static const std::filesystem::path Dir = TempDirectory().append("Source\\");
        return Dir;
    }

    /** Returns the path to the results directory. */
    static std::filesystem::path ResultsDirectory()
    {
        static const std::filesystem::path Dir = TempDirectory().append("Results\\");
        return Dir;
    }
};
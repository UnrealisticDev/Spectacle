#include "SpecifierCollector.h"
#include <filesystem>
#include <iostream>
#include "CoreTypes.h"
#include "json.hpp"
#include <fstream>

bool FSpecifierCollector::GatherSpecifiers(const char* Directory, FSpecifierStats& OutStats)
{
	namespace fs = std::filesystem;

	if ( !fs::exists(Directory) || !fs::is_directory(Directory) )
	{
		std::cerr << "Source directory" << Directory << "does not exist.";
		return false;
	}

	OutStats.clear();
	FString ResultPath = "Results.json";
	for (const fs::directory_entry& SourcePath : fs::recursive_directory_iterator(Directory))
	{
		if ( !SourcePath.is_directory() && SourcePath.path().extension() == ".h" )
		{
			std::cout << SourcePath.path() << std::endl;
			
			// Run parse subprocess - results in output at Results.json
			FString ParseCommand = "F:/Projects/Unrealistic/Spectacle/x64/Debug/Parser.exe";
			ParseCommand += " " + SourcePath.path().string();
			ParseCommand += " " + ResultPath;
			std::system(ParseCommand.c_str());

			// Load parse results
			using json = nlohmann::json;

			FString FileContent;
			std::ifstream File(ResultPath);

			try {
				if (File.is_open())
				{
					FString Line = "";
					while (!File.eof())
					{
						std::getline(File, Line);
						FileContent += Line + "\n\r"; // Normalize line endings
					}
				}
			}
			catch (std::exception& e)
			{
				std::cout << "Failed to load result file: " << e.what() << "\n";
			}

			try {
				json Results = json::parse(FileContent);

				assert(Results.is_object());
				for (const json& Result : Results["data"])
				{
					assert(Result.is_object());
					std::cout << Result << "\n";
				}
			}
			catch(std::exception& e)
			{
				std::cout << "Failed to parse result file: " << e.what() << "\n";
			}

			// Append to OutCollection
		}
	}

	return true;
}

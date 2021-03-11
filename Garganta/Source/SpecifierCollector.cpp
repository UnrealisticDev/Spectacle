#include "SpecifierCollector.h"
#include <filesystem>
#include <iostream>
#include "CoreTypes.h"
#include "json.hpp"
#include <fstream>

void FSpecifierCollector::ParseSpecifiers(const char* Directory)
{
	namespace fs = std::filesystem;

	if ( !fs::exists(Directory) || !fs::is_directory(Directory) )
	{
		std::cerr << "Source directory" << Directory << "does not exist.";
		return;
	}

	Stats.clear();
	FString ResultPath = "Results.json";
	for (const fs::directory_entry& SourcePath : fs::recursive_directory_iterator(Directory))
	{
		if ( !SourcePath.is_directory() && SourcePath.path().extension() == ".h" )
		{
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

					// Append to collection
					FUnrealSpecifier Specifier
					(
						Result["type"].get<FString>(),
						Result["meta"].get<bool>(),
						Result["key"].get<FString>()
					);
					
					Stats[Specifier][SourcePath.path().string()] = Result["count"];
				}
			}
			catch(std::exception& e)
			{
				std::cout << "Failed to parse result file: " << e.what() << "\n";
			}
		}
	}
}

void FSpecifierCollector::Upload()
{
	// no op
}

void FSpecifierCollector::Dump()
{
	for (const std::pair<FUnrealSpecifier, std::unordered_map<FString, int32>>& Stat : Stats)
	{
		std::cout 
			<< "(" + Stat.first.Type + ")" 
			<< " " << Stat.first.Key 
			<< " " << (Stat.first.bMetadata ? "meta" : "") 
			<< std::endl;

		for (const std::pair<const FString, int32>& File : Stat.second)
		{
			std::cout 
				<< "\t" 
				<< File.first 
				<< " --> " 
				<< File.second 
				<< std::endl;
		}

		std::cout << std::endl;
	}
}

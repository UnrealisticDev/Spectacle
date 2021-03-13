#include "SpecifierCollector.h"
#include <iostream>
#include <fstream>
#include "CoreTypes.h"

void FSpecifierCollector::ParseSpecifiers(const char* SourceDirectory, const char* ResultsDirectory)
{
	namespace fs = std::filesystem;

	if ( !fs::exists(SourceDirectory) || !fs::is_directory(SourceDirectory) )
	{
		std::cerr << "Source directory" << SourceDirectory << "does not exist.";
		return;
	}

	if ( !fs::exists(ResultsDirectory) || !fs::is_directory(ResultsDirectory) )
	{
		fs::create_directory(ResultsDirectory);
	}

	const FString ParseOutputPath = "Output.json";
	for (const fs::directory_entry& SourcePath : fs::recursive_directory_iterator(SourceDirectory))
	{
		if ( !SourcePath.is_directory() && SourcePath.path().extension() == ".h" )
		{
			const fs::path RelativeSourcePath = SourcePath.path().lexically_relative(SourceDirectory);
			FString ParseCommand = "F:/Projects/Unrealistic/Spectacle/x64/Debug/Parser.exe";
			{
				ParseCommand += " " + SourcePath.path().string();
				ParseCommand += " " + ParseOutputPath;
			}

			std::cout << "Parsing: " << RelativeSourcePath.string() << std::endl;

			try 
			{
				std::system(ParseCommand.c_str());
			}

			catch (...) 
			{
				std::cout << "\tSomething went wrong in parsing source..." << std::endl;
				continue;
			}

			FJson Results;
			try
			{
				Results = FJson::parse(std::ifstream(ParseOutputPath));
			}
			
			catch (...)
			{
				std::cout << "\tSomething went wrong in parsing output..." << std::endl;
			}

			SaveResults(Results, RelativeSourcePath);
		}
	}
}

void FSpecifierCollector::Upload()
{
	//const FString ParsedDatasetPath = "Dataset.json";
	//std::ofstream ParsedDataset(ParsedDatasetPath);
	//if ( !ParsedDataset.is_open())
	//{
	//	std::cerr << "Failed to open/create Dataset.json";
	//}

	//using json = nlohmann::json;
	//json jDataset;
	//{
	//	jDataset["items"] = {};
	//	for (const std::pair<FUnrealSpecifier, std::unordered_map<FString, int32>>& Stat : Stats)
	//	{
	//		json jStat;
	//		jStat["type"] = Stat.first.Type;
	//		jStat["meta"] = Stat.first.bMetadata;
	//		jStat["key"] = Stat.first.Key;

	//		jStat["appearances"] = {};
	//		for (const std::pair<const FString, int32>& Appearance : Stat.second)
	//		{
	//			json jAppearance;
	//			jAppearance["file"] = Appearance.first;
	//			jAppearance["count"] = Appearance.second;

	//			jStat["appearances"] += jAppearance;
	//		}

	//		jDataset["items"] += jStat;
	//	}
	//}

	//ParsedDataset << jDataset;
	//ParsedDataset.close();

	//FString UploadCommand = "node";
	//{
	//	UploadCommand += " F:/Projects/Unrealistic/Spectacle/Broadcaster/app.js";
	//	UploadCommand += " ";
	//	UploadCommand += ParsedDatasetPath;
	//}
	//std::system(UploadCommand.c_str());
}

void FSpecifierCollector::Cleanup(const char* ResultsDirectory)
{
	namespace fs = std::filesystem;

	if ( fs::exists(ResultsDirectory) && fs::is_directory(ResultsDirectory) )
	{
		fs::remove_all(ResultsDirectory);
	}
}

void FSpecifierCollector::SaveResults(const FJson& Results, const std::filesystem::path& RelativeSourcePath)
{
	try
	{
		namespace fs = std::filesystem;
		for (const auto& Item : Results["items"])
		{
			assert(Item.contains("type") && Item.contains("key") && Item.contains("meta"));

			fs::path SavePath = "Results";
			SavePath /= Item["type"].get<FString>() + "_" + Item["key"].get<FString>();
			SavePath += ".json";

			//std::cout << SavePath.string() << std::endl;

			if (fs::exists(SavePath))
			{
				FJson Existing = FJson::parse(std::ifstream(SavePath));
				std::ofstream SaveFile(SavePath);
				Existing["occ"].push_back({
					{ "file", RelativeSourcePath.string() },
					{ "count", Item["count"] }
					});
				SaveFile << Existing.dump();
			}

			else
			{
				std::ofstream SaveFile(SavePath);
				FJson Fresh;
				{
					Fresh["type"] = Item["type"];
					Fresh["key"] = Item["key"];
					Fresh["meta"] = Item["meta"];
					Fresh["occ"] = FJson::array
					({
						{
							{"file", RelativeSourcePath.string()},
							{"count", Item["count"] }
						}
						});
				};
				SaveFile << Fresh.dump();
			}
		}
	}

	catch (...)
	{
		std::cout << "Something went wrong while saving results." << std::endl;
	}
}

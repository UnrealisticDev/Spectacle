#include "SpecifierCollector.h"
#include <iostream>
#include <fstream>
#include <algorithm>
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
	FString UploadCommand = "node";
	{
		UploadCommand += " F:/Projects/Unrealistic/Spectacle/Broadcaster/app.js";
		UploadCommand += " ";
		UploadCommand += "Results";
	}
	std::system(UploadCommand.c_str());
}

void FSpecifierCollector::Cleanup(const char* ResultsDirectory)
{
	namespace fs = std::filesystem;

	if ( fs::exists(ResultsDirectory) && fs::is_directory(ResultsDirectory) )
	{
		fs::remove_all(ResultsDirectory);
	}
}

void FSpecifierCollector::ValidateResult(const FJson& Result)
{
	assert(Result["type"].is_string());
	assert(Result["key"].is_string());
	assert(Result["meta"].is_boolean());
	assert(Result["occ"].is_array() && Result["occ"].size() > 0);
}

FString FSpecifierCollector::GetResultFilename(const FJson& Result) const
{
	return Result["type"].get<FString>() + "_" + Result["key"].get<FString>() + ".json";
}

void FSpecifierCollector::TrimResults(const char* ResultsDirectory, int Size)
{
	namespace fs = std::filesystem;

	if ( !fs::exists(ResultsDirectory) || !fs::is_directory(ResultsDirectory) )
	{
		throw std::runtime_error("Results directory is invalid.");
	}

	for ( const fs::directory_entry& ResultPath : fs::recursive_directory_iterator(ResultsDirectory) )
	{
		if ( ResultPath.is_directory() )
		{
			continue;
		}

		FJson Result = FJson::parse(std::ifstream(ResultPath));
		ValidateResult(Result);
		TrimResult(Result, Size);
		SaveResult(Result);
	}
}

void FSpecifierCollector::TrimResult(FJson& Result, int Size)
{
	FJson& Occurences = Result["occ"];
	if (Occurences.size() <= Size)
	{
		return; // Nothing to trim
	}

	auto CountComparator = [](const FJson& A, const FJson& B) -> bool
	{
		return A["count"].get<int32>() > B["count"].get<int32>();
	};

	std::sort(Occurences.begin(), Occurences.end(), CountComparator);
	auto TempVec = Occurences.get<TArray<FJson>>();
	TempVec.resize(Size);
	Occurences = TempVec;
}

void FSpecifierCollector::SaveResults(const FJson& Results, const std::filesystem::path& RelativeSourcePath)
{
	try
	{
		namespace fs = std::filesystem;
		for (const FJson& Result : Results["items"])
		{
			assert(Result.contains("type") && Result.contains("key") && Result.contains("meta"));

			fs::path SavePath = "Results";
			SavePath /= GetResultFilename(Result);

			if (fs::exists(SavePath))
			{
				FJson Existing = FJson::parse(std::ifstream(SavePath));
				std::ofstream SaveFile(SavePath);
				Existing["occ"].push_back({
					{ "file", RelativeSourcePath.string() },
					{ "count", Result["count"] }
					});
				SaveFile << Existing.dump();
			}

			else
			{
				std::ofstream SaveFile(SavePath);
				FJson Fresh;
				{
					Fresh["type"] = Result["type"];
					Fresh["key"] = Result["key"];
					Fresh["meta"] = Result["meta"];
					Fresh["occ"] = FJson::array
					({
						{
							{"file", RelativeSourcePath.string()},
							{"count", Result["count"] }
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

void FSpecifierCollector::SaveResult(const FJson& Result)
{
	std::ofstream ResultFile( std::filesystem::path("Results") / GetResultFilename(Result) );
	ResultFile << Result.dump();
}

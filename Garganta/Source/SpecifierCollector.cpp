#include "SpecifierCollector.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include "CoreTypes.h"
#include "CorePath.h"

void FSpecifierCollector::ParseSpecifiers()
{
	namespace fs = std::filesystem;
	fs::path SourceDirectory = FPaths::SourceDirectory();
	fs::path ResultsDirectory = FPaths::ResultsDirectory();
	const FString ParsedOutputFilename = "Parsed.json";
	const fs::path ParsedOutputPath = FPaths::TempDirectory().append(ParsedOutputFilename);

	if ( !fs::exists(SourceDirectory) || !fs::is_directory(SourceDirectory) )
	{
		std::cerr << "Source directory" << SourceDirectory << "does not exist.";
		return;
	}

	if ( !fs::exists(ResultsDirectory) || !fs::is_directory(ResultsDirectory) )
	{
		fs::create_directory(ResultsDirectory);
	}

	for (const fs::directory_entry& SourcePath : fs::recursive_directory_iterator(SourceDirectory))
	{
		if ( !SourcePath.is_directory() && SourcePath.path().extension() == ".h" )
		{
			const fs::path RelativeSourcePath = SourcePath.path().lexically_relative(SourceDirectory);
			std::cout << "Parsing: " << RelativeSourcePath.string() << std::endl;

			try
			{
				FString ParseCommand = "Parser";
				ParseCommand
					.append(" ")
					.append(SourcePath.path().string())
					.append(" ")
					.append(ParsedOutputFilename);

				std::system(ParseCommand.c_str());
			}

			catch (...) 
			{
				std::cout << "\tSomething went wrong in parsing source..." << std::endl;
				continue;
			}

			FJson ParsedSpecifiers;
			try
			{
				ParsedSpecifiers = FJson::parse(std::ifstream(ParsedOutputPath));
			}
			
			catch (...)
			{
				std::cout << "\tSomething went wrong in parsing output..." << std::endl;
				continue;
			}

			FJson Result;
			for (const FJson& ParsedSpecifier : ParsedSpecifiers["items"])
			{
				ValidateParsedSpecifier(ParsedSpecifier);
				ConvertParsedSpecifierToResult(ParsedSpecifier, RelativeSourcePath, Result);
				ValidateResult(Result);
				SaveResult(Result);
			}
		}
	}
}

void FSpecifierCollector::ValidateParsedSpecifier(const FJson& ParsedSpecifier)
{
	assert(ParsedSpecifier["type"].is_string());
	assert(ParsedSpecifier["key"].is_string());
	assert(ParsedSpecifier["meta"].is_boolean());
	assert(ParsedSpecifier["count"].is_number() && ParsedSpecifier["count"].get<int32>() > 0);
}

void FSpecifierCollector::ConvertParsedSpecifierToResult(const FJson& ParsedSpecifier, const std::filesystem::path& RelativeSourcePath, FJson& Result)
{
	try
	{
		Result.clear();

		namespace fs = std::filesystem;
		fs::path ResultFilePath = FPaths::ResultsDirectory().append(GetResultFilename(ParsedSpecifier["type"], ParsedSpecifier["key"]));
		if ( fs::exists(ResultFilePath) )
		{
			Result = FJson::parse(std::ifstream(ResultFilePath));
			Result["occ"].push_back
			({
				{ "file", RelativeSourcePath.generic_string() },
				{ "count", ParsedSpecifier["count"] }
			});
		}

		else
		{
			Result["type"] = ParsedSpecifier["type"];
			Result["key"] = ParsedSpecifier["key"];
			Result["meta"] = ParsedSpecifier["meta"];
			Result["occ"] = FJson::array
			({
				{
					{"file", RelativeSourcePath.generic_string()},
					{"count", ParsedSpecifier["count"] }
				}
			});
		}
	}

	catch (...)
	{
		std::cout << "Something went wrong while converting parsed specifiers to results." << std::endl;
	}
}

void FSpecifierCollector::ValidateResult(const FJson& Result)
{
	assert(Result["type"].is_string());
	assert(Result["key"].is_string());
	assert(Result["meta"].is_boolean());
	assert(Result["occ"].is_array() && Result["occ"].size() > 0);
}

FString FSpecifierCollector::GetResultFilename(const FJson& Type, const FJson& Key) const
{
	return Type.get<FString>() + "_" + Key.get<FString>() + ".json";
}

FString FSpecifierCollector::GetResultFilename(const FJson& Result) const
{
	return GetResultFilename(Result["type"], Result["key"]);
}

void FSpecifierCollector::SaveResult(const FJson& Result)
{
	std::ofstream ResultFile( FPaths::ResultsDirectory().append(GetResultFilename(Result)) );
	ResultFile << Result.dump();
}

void FSpecifierCollector::TrimResults(int Size)
{
	namespace fs = std::filesystem;
	fs::path ResultsDirectory = FPaths::ResultsDirectory();

	if ( !fs::exists(ResultsDirectory) || !fs::is_directory(ResultsDirectory) )
	{
		throw std::runtime_error("Results directory is invalid. Nothing to trim.");
	}

	for ( const fs::directory_entry& ResultPath : fs::recursive_directory_iterator(ResultsDirectory) )
	{
		if ( ResultPath.is_directory() )
		{
			continue;
		}

		FJson Result = FJson::parse(std::ifstream(ResultPath));
		TrimResult(Result, Size);
		SaveResult(Result);
	}
}

void FSpecifierCollector::TrimResult(FJson& Result, int Size)
{
	FJson& Occurences = Result["occ"];
	if (Occurences.size() <= Size) // Nothing to trim
	{
		return;
	}

	auto CountComparator = [](const FJson& A, const FJson& B) -> bool
	{
		return A["count"].get<int32>() > B["count"].get<int32>();
	};

	std::sort(Occurences.begin(), Occurences.end(), CountComparator);
	TArray<FJson> Temp = Occurences.get<TArray<FJson>>();
	Temp.resize(Size);
	Occurences = Temp;
}

void FSpecifierCollector::Upload()
{
	FString UploadCommand = "node";
	UploadCommand
		.append(" ")
		.append("F:/Projects/Unrealistic/Spectacle/Broadcaster/app.js")
		.append(" ")
		.append(FPaths::ResultsDirectory().string());

	std::system(UploadCommand.c_str());
}

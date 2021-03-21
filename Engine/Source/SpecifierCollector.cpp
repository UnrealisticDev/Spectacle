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
		throw std::invalid_argument("Source directory" + SourceDirectory.string() + "does not exist.");
	}

	if ( !fs::exists(ResultsDirectory) || !fs::is_directory(ResultsDirectory) )
	{
		fs::create_directory(ResultsDirectory);
	}

	std::cout << "Parsing..." << std::endl;

	for (const fs::directory_entry& SourcePath : fs::recursive_directory_iterator(SourceDirectory))
	{
		if ( !SourcePath.is_directory() && SourcePath.path().extension() == ".h" )
		{
			const fs::path RelativeSourcePath = SourcePath.path().lexically_relative(SourceDirectory);

			FString ParseCommand = "Parse";
			ParseCommand
				.append(" ")
				.append(SourcePath.path().string())
				.append(" ")
				.append(ParsedOutputFilename);

			bool bParseFail = std::system(ParseCommand.c_str());
			if ( bParseFail )
			{
				std::cerr << "Failed to parse source file: " << RelativeSourcePath.string() << std::endl;
				continue;
			}
			
			FJson ParsedSpecifiers;
			try
			{
				ParsedSpecifiers = FJson::parse(std::ifstream(ParsedOutputPath));
			}
			
			catch (FJsonParseError e)
			{
				std::cerr << "Failed to deserialize results for source file: "
					<< RelativeSourcePath.string()
					<< " : " << e.what()
					<< std::endl;

				continue;
			}

			FJson Result;
			for (const FJson& ParsedSpecifier : ParsedSpecifiers["specifiers"])
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
	Result.clear();

	namespace fs = std::filesystem;
	fs::path ResultFilePath = FPaths::ResultsDirectory().append(GetResultFilename(ParsedSpecifier["type"], ParsedSpecifier["key"]));

	try
	{
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

	catch (FJsonParseError e)
	{
		std::cerr << "Parsing error when reading result file: " << ResultFilePath << std::endl;
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

		FJson Result;
		try
		{
			Result = FJson::parse(std::ifstream(ResultPath));
		}
		catch (FJsonParseError e)
		{
			std::cerr << "Parse error when reading file for trimming: " 
				<< ResultPath.path() 
				<< " : " << e.what() 
				<< std::endl;

			continue;
		}

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

void FSpecifierCollector::Upload(FString Branch)
{
	if ( Branch.rfind("4.", 0) == 0 )
	{
		int32 DelimPos = (int32)Branch.find('.');
		Branch = Branch.substr
		(
			DelimPos + 1,
			Branch.length() - DelimPos
		);
	}

	FString UploadCommand = "upload";
	UploadCommand
		.append(" ")
		.append(FPaths::ResultsDirectory().string())
		.append(" ")
		.append(Branch);

	bool bUploadFail = std::system(UploadCommand.c_str());
	if ( bUploadFail )
	{
		std::cerr << "Upload failed.";
	}
}

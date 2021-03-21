#include <fstream>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <chrono>
#include <thread>

#include <Windows.h>

#include "CoreTypes.h"
#include "json.hpp"

const uint8 MIN_VERSION = 0; // Because there is no such thing as UE 4.-1
const uint8 MAX_VERSION = 99; // Because we'll all be dead by the time UE 4.100 comes out
const FString HistoryFilePath = "history.json";

using FJson = nlohmann::json;
using FJsonParseError = nlohmann::detail::parse_error;

TArray<FJson> LoadParsedVersions()
{
	TArray<FJson> ParsedVersions;

	std::ifstream HistoryFile(HistoryFilePath);
	if (HistoryFile.is_open())
	{
		FJson History = FJson::parse(HistoryFile);
		if (!History.is_null() && History.contains("parsedVersions") && History["parsedVersions"].is_array())
		{
			ParsedVersions = History["parsedVersions"].get<TArray<FJson>>();
		}
	}

	return ParsedVersions;
}

bool VersionAlreadyParsed(uint8 Version, TArray<FJson> ParsedVersions)
{
	auto Contains = [Version](const FJson& ParsedVersion) -> bool
	{
		return Version == ParsedVersion.get<uint8>();
	};

	bool bParsed = std::find_if
	(
		ParsedVersions.begin(),
		ParsedVersions.end(),
		Contains
	) != ParsedVersions.end();

	return bParsed;
}

bool VersionExists(uint8 Version)
{
	FString sVersion = "4." + std::to_string(Version);

	FString ValidateCommand = "svn ls > nul";
	ValidateCommand
		.append(" ")
		.append("https://github.com/EpicGames/UnrealEngine")
		.append("/branches/")
		.append(sVersion);

	return std::system(ValidateCommand.c_str()) == 0;
}

void ParseVersion(uint8 Version)
{
	FString sVersion = "4." + std::to_string(Version);

	FString ParseCommand = "Engine";
	ParseCommand
		.append(" ")
		.append(sVersion);

	std::system(ParseCommand.c_str());
}

void SaveParsedVersions(const TArray<FJson>& ParsedVersions)
{
	if ( !ParsedVersions.empty() )
	{
		std::ofstream HistoryFile(HistoryFilePath);
		FJson History;
		History["parsedVersions"] = ParsedVersions;
		HistoryFile << History.dump();
	}
}

/** 
 * This is a heartbeat process 
 * designed to iterate over each version of Unreal Engine 4
 * and put it through the parsing engine. Once all existing
 * versions are parsed, the process goes into monitoring
 * mode and periodically checks for new versions.
 * If a new version is available, it will parse it.
 */
int main(int ArgumentCount, char* Arguments[])
{
	if (ArgumentCount > 1 && Arguments[1] == "hidden")
	{
		std::ofstream out("Log.txt");
		std::streambuf* coutbuf = std::cout.rdbuf();
		std::cout.rdbuf(out.rdbuf());

		::ShowWindow(::GetConsoleWindow(), SW_HIDE); // Hide console and redirect output to file
	}

	TArray<FJson> ParsedVersions;
	try
	{
		ParsedVersions = LoadParsedVersions();
	}
	catch (std::ifstream::failure e)
	{
		std::cerr << "Encountered error in opening history file: " << e.what() << std::endl;
		return -1;
	}
	catch (FJsonParseError e)
	{
		std::cerr << "Encountered error in parsing history file: " << e.what() << std::endl;
		return -1;
	}
	std::cout << "Loaded parsed versions." << std::endl;

	uint8 LatestUnparsedVersion = 0;

	for (uint8 Version = MIN_VERSION; Version < MAX_VERSION; ++Version)
	{
		if ( VersionAlreadyParsed(Version, ParsedVersions) )
		{
			std::cout << "Skipping as already parsed: " << std::to_string(Version) << std::endl;
			continue;
		}

		if ( !VersionExists(Version) )
		{
			std::cout << "Could not find: " << std::to_string(Version) << std::endl;
			LatestUnparsedVersion = Version;
			break;
		}

		std::cout << "Parsing: " << std::to_string(Version) << std::endl;
		ParseVersion(Version);

		ParsedVersions.push_back(Version);
		SaveParsedVersions(ParsedVersions);
	}

	std::cout << "Shifting to monitoring mode." << std::endl;
	
	using FDuration = std::chrono::duration;
	FDuration ScanInterval = std::chrono::hours(24);
	while ( LatestUnparsedVersion < MAX_VERSION )
	{
		if ( !VersionExists(LatestUnparsedVersion) )
		{
			std::cout << "Could not find: " << std::to_string(LatestUnparsedVersion) << std::endl;
			std::cout << "Sleeping..." << std::endl;
			std::this_thread::sleep_for(ScanInterval);
			continue;
		}

		std::cout << "Parsing: " << std::to_string(LatestUnparsedVersion) << std::endl;
		ParseVersion(LatestUnparsedVersion);

		ParsedVersions.push_back(LatestUnparsedVersion);
		SaveParsedVersions(ParsedVersions);

		++LatestUnparsedVersion;
	}	
}

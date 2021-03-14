#include <filesystem>
#include "Repository.h"
#include "SpecifierCollector.h"
#include "CoreTypes.h"
#include "CorePath.h"

int main(int ArgumentCount, char* Arguments[])
{
	namespace fs = std::filesystem;

	FString RepoURL = "https://github.com/EpicGames/UnrealEngine";
	FString Branch = Arguments[1];
	TArray<FString> Paths = {
		"Engine/Source/Runtime",
		"Engine/Source/Editor",
		"Engine/Source/Developer"
	};
	FString SourceDirectory = "/Source";
	FString ResultsDirectory = "/Results";

	FPaths::CreateTempDirectory();

	FRepository::Clone
	(
		RepoURL, Branch, Paths, 
		FPaths::TempDirectory().append(SourceDirectory));

	FSpecifierCollector SpecifierCollector;
	{
	//	SpecifierCollector.ParseSpecifiers(StagingDirectory, ResultsDirectory);
	//	SpecifierCollector.TrimResults(ResultsDirectory, 5);
	//	SpecifierCollector.Upload();
	//	SpecifierCollector.Cleanup(ResultsDirectory);
	}

	//FRepository::Cleanup(StagingDirectory);
}

#include <iostream>
#include <filesystem>
#include "Repository.h"
#include "SpecifierCollector.h"
#include "CoreTypes.h"
#include "CorePath.h"

int main(int ArgumentCount, char* Arguments[])
{
	const FString RepoURL = "https://github.com/EpicGames/UnrealEngine";
	const FString Branch = ArgumentCount > 1 ? Arguments[1] : "";
	const TArray<FString> Directories = 
	{
		"Engine/Source/Runtime",
		"Engine/Source/Editor",
		"Engine/Source/Developer"
	};

	FPaths::CreateTempDirectory();

	FRepository::Clone(RepoURL, Branch, Directories);

	FSpecifierCollector SpecifierCollector;
	{
		SpecifierCollector.ParseSpecifiers();
		SpecifierCollector.TrimResults(5);
		SpecifierCollector.Upload(Branch);
	}

	FPaths::CleanupTempDirectory();
}

#include <iostream>
#include <filesystem>
#include "Repository.h"
#include "SpecifierCollector.h"
#include "CoreTypes.h"
#include "CorePath.h"

int main(int ArgumentCount, char* Arguments[])
{
	FString RepoURL = "https://github.com/EpicGames/UnrealEngine";
	FString Branch = Arguments[1];
	TArray<FString> Directories = 
	{
		//"Engine/Source/Runtime",
		//"Engine/Source/Editor",
		"Engine/Source/Runtime/AugmentedReality/Public"
	};

	FPaths::CreateTempDirectory();

	FRepository::Clone(RepoURL, Branch, Directories);

	FSpecifierCollector SpecifierCollector;
	{
		SpecifierCollector.ParseSpecifiers();
		SpecifierCollector.TrimResults(5);
	//	SpecifierCollector.Upload();
	}

	std::filesystem::remove_all(FPaths::TempDirectory());
}

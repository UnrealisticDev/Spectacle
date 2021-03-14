#include "Repository.h"
#include "SpecifierCollector.h"
#include "CoreTypes.h"
#include "CorePath.h"
#include "iostream"

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

	FRepository::Clone
	(
		RepoURL, Branch, Directories, 
		FPaths::SourceDirectory()
	);

	//FSpecifierCollector SpecifierCollector;
	{
	//	SpecifierCollector.ParseSpecifiers(StagingDirectory, ResultsDirectory);
	//	SpecifierCollector.TrimResults(ResultsDirectory, 5);
	//	SpecifierCollector.Upload();
	//	SpecifierCollector.Cleanup(ResultsDirectory);
	}

	FRepository::Cleanup
	(
		FPaths::SourceDirectory()
	);
}

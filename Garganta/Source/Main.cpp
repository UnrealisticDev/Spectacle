#include "Repository.h"
#include "SpecifierCollector.h"

int main(int ArgumentCount, char* Arguments[])
{
	const char* RepoURL = "https://github.com/EpicGames/UnrealEngine";
	const char* Branch = Arguments[1];
	TArray<const char*> Paths = {
		"Engine/Source/Runtime",
		"Engine/Source/Editor",
		"Engine/Source/Developer"
	};
	const char* StagingDirectory = "Staging";
	const char* ResultsDirectory = "Results";

	FRepository::Clone(RepoURL, Branch, Paths, StagingDirectory);

	FSpecifierCollector SpecifierCollector;
	{
		SpecifierCollector.ParseSpecifiers(StagingDirectory, ResultsDirectory);
		//SpecifierCollector.Upload();
		//SpecifierCollector.Cleanup(ResultsDirectory);
	}

	FRepository::Cleanup(StagingDirectory);
}

#include "Repository.h"
#include "SpecifierCollector.h"

int main()
{
	const char* RepoURL = "https://github.com/EverNewJoy/VictoryPlugin.git";
	const char* StagingDirectory = "Temp";

	FRepository::Clone(RepoURL, StagingDirectory);

	FSpecifierStats SpecifierStats;
	FSpecifierCollector SpecifierCollector;
	SpecifierCollector.GatherSpecifiers(StagingDirectory, SpecifierStats);

	FRepository::Cleanup(StagingDirectory);
}

#include "Repository.h"
#include "SpecifierCollector.h"

int main()
{
	const char* RepoURL = "https://github.com/EverNewJoy/VictoryPlugin.git";
	const char* StagingDirectory = "Temp";

	FRepository::Clone(RepoURL, StagingDirectory);

	FSpecifierCollection SpecifierCollection;
	FSpecifierCollector SpecifierCollector;
	if ( !SpecifierCollector.GatherSpecifiers(StagingDirectory, SpecifierCollection) )
	{
		return -1;
	}

	FRepository::Cleanup(StagingDirectory);
}

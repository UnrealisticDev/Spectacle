#include "Repository.h"
#include "SpecifierCollector.h"

int main()
{
	const char* RepoURL = "https://github.com/EverNewJoy/VictoryPlugin.git";
	const char* StagingDirectory = "Temp";

	FRepository::Clone(RepoURL, StagingDirectory);

	FSpecifierCollector SpecifierCollector;
	{
		SpecifierCollector.ParseSpecifiers(StagingDirectory);
		SpecifierCollector.Dump();
		SpecifierCollector.Upload();
	}

	FRepository::Cleanup(StagingDirectory);
}

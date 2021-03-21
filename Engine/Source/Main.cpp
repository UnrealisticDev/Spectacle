#include <iostream>
#include <filesystem>
#include <stdexcept>
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

	try
	{
		FRepository::Clone(RepoURL, Branch, Directories);
	}
	catch (std::invalid_argument e)
	{
		std::cerr << "Failed to clone repository: " << e.what() << std::endl;
		return -1;
	}
	catch (std::runtime_error e)
	{
		std::cerr << e.what() << std::endl;
		return -1;
	}

	FSpecifierCollector SpecifierCollector;
	try
	{
		SpecifierCollector.ParseSpecifiers();
		SpecifierCollector.TrimResults(5);
		SpecifierCollector.Upload(Branch);
	}
	catch (std::invalid_argument e)
	{
		std::cerr << "Passed invalid argument for spec collection: " << e.what() << std::endl;
	}
	catch (std::ios_base::failure e)
	{
		std::cout << "Error occurred in input/output: " << e.what() << std::endl;
	}
	catch (std::runtime_error e)
	{
		std::cout << "Encountered error while collecting specs: " << e.what() << std::endl;
	}

	FPaths::CleanupTempDirectory();
}

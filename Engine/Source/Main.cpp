#include <iostream>
#include <filesystem>
#include <stdexcept>
#include "Repository.h"
#include "SpecifierCollector.h"
#include "CoreTypes.h"
#include "CorePath.h"

enum class EReturn : uint8
{
	Success			= 0 << 0,
	Fail			= 1	<< 0
};

int main(int ArgumentCount, char* Arguments[])
{
	EReturn ExitCode = EReturn::Success;

	const FString RepoURL = "https://github.com/EpicGames/UnrealEngine";
	const FString Branch = ArgumentCount > 1 ? Arguments[1] : "";
	const TArray<FString> Directories =
	{
		"Engine/Source/Runtime",
		"Engine/Source/Editor",
		"Engine/Source/Developer"
	};

	FPaths::CreateTempDirectory();

	bool bCloneSuccess = false;
	try
	{
		FRepository::Clone(RepoURL, Branch, Directories);
		bCloneSuccess = true;
	}
	catch (std::invalid_argument e)
	{
		std::cerr << "Failed to clone repository: " << e.what() << std::endl;
		ExitCode = EReturn::Fail;
	}
	catch (std::runtime_error e)
	{
		std::cerr << e.what() << std::endl;
		ExitCode = EReturn::Fail;
	}

	if ( bCloneSuccess )
	{
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
			ExitCode = EReturn::Fail;
		}
		catch (std::ios_base::failure e)
		{
			std::cout << "Error occurred in input/output: " << e.what() << std::endl;
			ExitCode = EReturn::Fail;
		}
		catch (std::runtime_error e)
		{
			std::cout << "Encountered error while collecting specs: " << e.what() << std::endl;
			ExitCode = EReturn::Fail;
		}
	}

	FPaths::CleanupTempDirectory();

	return (uint8)ExitCode;
}

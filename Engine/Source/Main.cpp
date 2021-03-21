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

/** 
 * The Engine puts various pieces of the Spectacle project
 * together to create a comprehensive UE4 parser.
 * It clones the Unreal Engine source code from GitHub
 * at the specified branch, parses relevant source files for USpecifiers,
 * outputs that information to result files on a per-specifier basis,
 * and uploads those results to the server of your choosing
 * (in this case Contentful) for further processing.
 * 
 * @param 1: Branch: Should be in the form "4.*" (e.g. 4.23). If not specified, defaults to master.
 */
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

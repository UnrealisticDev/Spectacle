#include "Repository.h"
#include "git2.h"
#include <iostream>
#include <filesystem>
#include <stdexcept>
#include "CoreTypes.h"

bool FRepository::Clone(FString RepoURL, FString Branch, TArray<FString> Paths, const char* Destination)
{
	if ( DirectoryAlreadyExists(Destination) )
	{
		Cleanup(Destination);
	}

	FString RepoRoot = RepoURL;

	if ( !Branch.empty() )
	{
		RepoRoot.append() += "/branches/";
		RepoRoot += Branch;
		RepoRoot += "/";
	}

	else
	{
		RepoRoot += "/trunk/"; // In SVN, refers to main branch
	}

	bool bRepoNotFound = std::system(("svn ls " + RepoRoot).c_str());
	if ( bRepoNotFound )
	{
		throw std::invalid_argument("Repository does not exist: " + RepoRoot);
	}

	if ( Paths.size() > 0 )
	{
		for (const char* SubPath : Paths)
		{
			FString FullPath = RepoRoot + SubPath;
			std::cout << "Exporting: " + FullPath << std::endl;
			FString ExportCommand = "svn";
			{
				ExportCommand += " export";
				ExportCommand += " " + FullPath + " ";
				ExportCommand += Destination;
				ExportCommand += "/";
				ExportCommand += SubPath;
			}
			std::system(ExportCommand.c_str());
		}
	}

	else
	{
		FString ExportCommand = "svn";
		{
			ExportCommand += " export";
			ExportCommand += " " + RepoRoot + " ";
			ExportCommand += Destination;
		}
		std::system(ExportCommand.c_str());
	}

	return true;
}

bool FRepository::Cleanup(const char* Directory)
{
	namespace fs = std::filesystem;
	try
	{
		fs::remove_all(Directory);
	}
	catch (fs::filesystem_error E)
	{
		std::cerr << "Failed to clear working directory: " << E.what();
		return false;
	}

	return true;
}

bool FRepository::DirectoryAlreadyExists(const char* Directory)
{
	namespace fs = std::filesystem;
	return fs::exists(Directory) && fs::is_directory(Directory);
}

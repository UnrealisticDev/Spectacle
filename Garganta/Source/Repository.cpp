#include "Repository.h"
#include "git2.h"
#include <iostream>
#include <filesystem>
#include "CoreTypes.h"

bool FRepository::Clone(const char* RepoURL, const char* Branch, TArray<const char*> Paths, const char* Destination)
{
	if ( DirectoryAlreadyExists(Destination) )
	{
		Cleanup(Destination);
	}

	FString RepoRoot = RepoURL;

	if ( Branch )
	{
		RepoRoot += "/branches/";
		RepoRoot += Branch;
		RepoRoot += "/";
	}

	else
	{
		RepoRoot += "/trunk/"; // In SVN, refers to main branch
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

bool FRepository::FixupDirectoryPermissions(const char* Directory)
{
	namespace fs = std::filesystem;
	for (const fs::directory_entry& Path : fs::recursive_directory_iterator(Directory))
	{
		try
		{
			fs::permissions(Path, fs::perms::all);
		}
		catch (std::exception&)
		{
			std::cout << "Error setting permissions on repository.";
			return false;
		}
	}

	return true;
}

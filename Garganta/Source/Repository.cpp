#include "Repository.h"
#include "git2.h"
#include <iostream>
#include <filesystem>

bool FRepository::Clone(const char* RepoURL, const char* Destination)
{
	if ( DirectoryAlreadyExists(Destination) )
	{
		FixupDirectoryPermissions(Destination);
		Cleanup(Destination);
	}

	git_libgit2_init();

	git_repository* Repository = nullptr;
	if ( git_clone(&Repository, RepoURL, Destination, nullptr) != 0 )
	{
		std::cerr << "Clone failed: " << git_error_last()->message << std::endl;
		return false;
	}

	git_repository_free(Repository);
	git_libgit2_shutdown();

	return FixupDirectoryPermissions(Destination);
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

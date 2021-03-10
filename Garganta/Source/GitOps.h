#pragma once

#include "git2.h"
#include <iostream>
#include <filesystem>

struct FGitOps
{
	/** Clones the requested repository into a working directory. */
	static bool CloneDirectory(const char* RepoURL, const char* WorkingDirectory)
	{
		git_libgit2_init();

		namespace fs = std::filesystem;
		if (fs::exists(WorkingDirectory) && fs::is_directory(WorkingDirectory))
		{
			try
			{
				fs::remove_all(WorkingDirectory);
			}
			catch (std::filesystem::filesystem_error E)
			{
				std::cerr << "Failed to clear working directory: " << E.what();
				return false;
			}
		}

		git_repository* repo = nullptr;
		if ( git_clone(&repo, RepoURL, WorkingDirectory, nullptr) != 0 )
		{
			std::cerr << "Clone failed: " << git_error_last()->message << std::endl;
			return false;
		}

		git_libgit2_shutdown();

		return FixupWorkingDirectoryPermissions(WorkingDirectory);
	}

private:

	/**
	 * Removes permission restrictions set by
	 * git clone process from provided directory.
	 */
	static bool FixupWorkingDirectoryPermissions(std::string WorkingDirectory)
	{
		namespace fs = std::filesystem;

		for (const fs::directory_entry& Path : fs::recursive_directory_iterator(WorkingDirectory))
		{
			try
			{
				fs::permissions(Path, fs::perms::all);
			}
			catch (std::exception&)
			{
				std::cout << "Error setting permissions on working directory.";
				return false;
			}
		}

		return true;
	}
};
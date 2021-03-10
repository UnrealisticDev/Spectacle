#pragma once

/** 
 * Helper class to clone and cleanup 
 * a repository that will be parsed.
 */
struct FRepository
{
	/** Clones the requested repository into the destination directory. */
	static bool Clone(const char* RepoURL, const char* Destination);

	/** Cleans up the repository in the provided directory. */
	static bool Cleanup(const char* Directory);

private:

	/** Checks if a directory already exists. */
	static bool DirectoryAlreadyExists(const char* Directory);

	/**
	 * Removes permission restrictions set by
	 * git clone process from provided directory.
	 */
	static bool FixupDirectoryPermissions(const char* Directory);
};
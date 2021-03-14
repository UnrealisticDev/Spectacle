#pragma once

#include <filesystem>
#include "CoreTypes.h"

/** 
 * Helper class to clone and cleanup 
 * a repository that will be parsed.
 */
struct FRepository
{
	/** 
	 * Clones the requested repository into the destination directory.
	 * Note: Switched to using SVN on the backend because GitHub doesn't allow you
	 * to git-archive specific directories yet allows svn export.
	 * This is imperative for handling huge monorepos (like Unreal Engine) when
	 * we only care about a few folders.
	 *
	 * @param RepoURL		The base URL to clone from, stripped of .git extension.
	 * @param Branch		The branch to clone. If not specified, defaults to main/master.
	 * @param Directories	The directories to clone. If not specified, defaults to root (full clone).
	 * @param Destination	The working directory to clone to.
	 * @see https://stackoverflow.com/questions/7106012/download-a-single-folder-or-directory-from-a-github-repo
	 */
	static bool Clone(FString RepoURL, FString Branch, TArray<FString> Directories, std::filesystem::path Destination);

	/** Cleans up the repository in the provided directory. */
	static bool Cleanup(const std::filesystem::path& Directory);
};
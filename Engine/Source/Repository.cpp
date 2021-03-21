#include "Repository.h"
#include <iostream>
#include <filesystem>
#include <stdexcept>
#include "CoreTypes.h"
#include "CorePath.h"

bool FRepository::Clone(const FString& RepoURL, const FString& Branch, const TArray<FString>& Directories)
{
	std::filesystem::path Destination = FPaths::SourceDirectory();
	FString Repo = RepoURL;

	if ( !Branch.empty() )
	{
		Repo
			.append("/branches/")
			.append(Branch)
			.append("/");
	}

	else
	{
		Repo.append("/trunk/"); // In SVN, refers to main branch
	}

	bool bRepoNotFound = std::system(("svn ls > nul " + Repo).c_str());
	if ( bRepoNotFound )
	{
		throw std::invalid_argument("Repository does not exist: " + Repo);
	}

	if ( Directories.size() > 0 )
	{
		for (const FString& Directory : Directories)
		{
			std::cout << "Exporting: " + Repo + Directory << std::endl;

			FString ExportCommand = "svn export --force > nul";
			ExportCommand
				.append(" ")
				.append
				(
					Repo + Directory
				)
				.append(" ")
				.append
				(
					Destination.string()
					.append(Directory)
				);

			std::system(ExportCommand.c_str());
		}
	}

	else
	{
		FString ExportCommand = "svn export";
		ExportCommand
			.append(" ")
			.append(Repo)
			.append(" ")
			.append(Destination.string());

		std::system(ExportCommand.c_str());
	}

	return true;
}

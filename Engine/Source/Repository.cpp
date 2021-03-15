#include "Repository.h"
#include <iostream>
#include <filesystem>
#include <stdexcept>
#include "CoreTypes.h"
#include "CorePath.h"

bool FRepository::Clone(FString RepoURL, FString Branch, TArray<FString> Directories)
{
	std::filesystem::path Destination = FPaths::SourceDirectory();

	if ( !Branch.empty() )
	{
		RepoURL
			.append("/branches/")
			.append(Branch)
			.append("/");
	}

	else
	{
		RepoURL.append("/trunk/"); // In SVN, refers to main branch
	}

	bool bRepoNotFound = std::system(("svn ls > nul " + RepoURL).c_str());
	if ( bRepoNotFound )
	{
		throw std::invalid_argument("Repository does not exist: " + RepoURL);
	}

	if ( Directories.size() > 0 )
	{
		for (const FString& Directory : Directories)
		{
			std::cout << "Exporting: " + RepoURL + Directory << std::endl;

			FString ExportCommand = "svn export";
			ExportCommand
				.append(" ")
				.append
				(
					RepoURL
					.append(Directory)
				)
				.append(" ")
				.append
				(
					Destination
					.append(Directory)
					.string()
				);

			std::system(ExportCommand.c_str());
		}
	}

	else
	{
		FString ExportCommand = "svn export";
		ExportCommand
			.append(" ")
			.append(RepoURL)
			.append(" ")
			.append(Destination.string());

		std::system(ExportCommand.c_str());
	}

	return true;
}

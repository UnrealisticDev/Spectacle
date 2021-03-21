#include "Repository.h"
#include <iostream>
#include <filesystem>
#include <stdexcept>
#include "CoreTypes.h"
#include "CorePath.h"

void FRepository::Clone(const FString& RepoURL, const FString& Branch, const TArray<FString>& Directories)
{
	std::filesystem::path Destination = FPaths::SourceDirectory();
	
	FString Repo = RepoURL;
	if ( Repo.empty() )
	{
		throw std::invalid_argument("No repo URL provided.");
	}

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
		throw std::invalid_argument("Repository was not found: " + Repo);
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

			bool bExportFail = std::system(ExportCommand.c_str());
			if ( bExportFail )
			{
				throw std::runtime_error("Encountered export error.");
			}
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

		bool bExportFail = std::system(ExportCommand.c_str());
		if ( bExportFail )
		{
			throw std::runtime_error("Encountered export error.");
		}
	}
}

#include <iostream>
#include <filesystem>
#include "GitOps.h"

bool CleanupWorkingDirectory()
{
	try
	{
		std::filesystem::remove_all("Temp");
	}
	catch (std::filesystem::filesystem_error E)
	{
		std::cerr << "Working directory cleanup failed: " << E.what();
		return false;
	}

	return true;
}

int main()
{
	const std::string WorkingDirectory = "Temp";
	if ( !FGitOps::CloneDirectory("https://github.com/Mowgl33/Rebaser.git", WorkingDirectory.c_str()) )
	{
		return -1;
	}

	// Step through files and compile list of specifier counts/file

	
	// @TODO: Post data to relevant server (Contentful most likely)]
	
	if ( !CleanupWorkingDirectory() )
	{
		return -1;
	}

	return 0;
}

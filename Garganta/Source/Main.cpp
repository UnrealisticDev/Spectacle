#include <iostream>
#include <filesystem>
#include "corecrt_io.h"
#include "direct.h"
#include "GitOps.h"

int main()
{
	// Clone relevant files into working directory
	const std::string WorkingDirectory = "Temp";
	if ( !FGitOps::CloneDirectory("https://github.com/Mowgl33/Rebaser.git", WorkingDirectory.c_str()) )
	{
		return -1;
	}

	// Step through files and compile list of specifier counts/file

	
	// @TODO: Post data to relevant server (Contentful most likely)]
	
	// Cleanup working directory
	try 
	{
		std::filesystem::remove_all("Temp");
	}
	catch (std::filesystem::filesystem_error e)
	{
		std::cerr << "Working directory cleanup failed: " << e.what();
		return -1;
	}

	return 0;
}

#include "SpecifierCollector.h"
#include <filesystem>
#include <iostream>

bool FSpecifierCollector::GatherSpecifiers(const char* WorkingDirectory, FSpecifierCollection& OutCollection)
{
	namespace fs = std::filesystem;

	if ( !fs::exists(WorkingDirectory) || !fs::is_directory(WorkingDirectory) )
	{
		std::cerr << "Working directory" << WorkingDirectory << "does not exist.";
		return false;
	}

	// @TODO: Clear out collection
	for (const fs::directory_entry& Path : fs::recursive_directory_iterator(WorkingDirectory))
	{
		if ( !Path.is_directory() && Path.path().extension() == ".h" )
		{
			std::cout << Path.path() << std::endl;
			
			// Run parser on file (sync)
			// Read results from output file
			// Append to OutCollection
		}
	}

	return true;
}

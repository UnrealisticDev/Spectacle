#pragma once

#include "CollectionTypes.h"

/** Does something... */
class FSpecifierCollector
{
public:

	/** 
	 * Gather specifiers found in the provided directory 
	 * and output relevant stats. 
	 */
	bool GatherSpecifiers(const char* Directory, FSpecifierStats& OutStats);

	/** Prints out debug information regarding specifier stats. */
	static void Dump(const FSpecifierStats& Stats);

private:

	/** The root directory from which we begin parsing. */
	const char* _WorkingDirectory;
};

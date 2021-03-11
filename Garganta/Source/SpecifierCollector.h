#pragma once

#include "CollectionTypes.h"

/** 
 * Collects specifiers from a provided repository
 * and farms out upload of stats
 * related to same. 
 */
class FSpecifierCollector
{
public:

	/** Identify specifiers in the provided directory. */
	void ParseSpecifiers(const char* Directory);

	/** Upload specifier stats to server. */
	void Upload();

	/** Print out debug information regarding parsed stats. */
	void Dump();

private:

	/** Collection of specifier stats. */
	FSpecifierStats Stats;
};

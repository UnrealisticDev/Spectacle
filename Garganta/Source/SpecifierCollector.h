#pragma once

#include "CollectionTypes.h"
#include "json.hpp"
#include <filesystem>

/** 
 * Collects specifiers from a provided repository
 * and farms out upload of stats
 * related to same. 
 */
class FSpecifierCollector
{
public:

	/** 
	 * Identify specifiers in the provided directory. 
	 * and output results to file.
	 */
	void ParseSpecifiers(const char* SourceDirectory, const char* ResultsDirectory);

	/** Upload specifier stats to server. */
	void Upload();

	/** Cleans up the results directory. */
	void Cleanup(const char* ResultsDirectory);

private:

	using FJson = nlohmann::json;

private:

	/** 
	 * Saves specifiers to file.
	 * Each unique specifier will have its 
	 * own file associated with it.
	 */
	void SaveResults(const FJson& Results, std::filesystem::path RelativeSourcePath);
};

#pragma once

#include "CollectionTypes.h"
#include "json.hpp"
#include <filesystem>
#include "CoreTypes.h"

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
	 * Ensures that a result has valid type, key, and meta values
	 * and that it has at least one occurrence.
	 */
	void ValidateResult(const FJson& Result);

	/** 
	 * Returns a unique filename that depends on the
	 * specifier type and key provided.
	 */
	FString GetResultFilename(const FJson& Result) const;

public:

	/** 
	 * Trims all results down to desired size on a
	 * per-specifier basis.
	 */
	void TrimResults(const char* ResultsDirectory, int Size);

private:

	/** Trims result down to desired size. */
	void TrimResult(FJson& Result, int Size);

	/** 
	 * Saves specifiers to file.
	 * Each unique specifier will have its 
	 * own file associated with it.
	 */
	void SaveResults(const FJson& Results, const std::filesystem::path& RelativeSourcePath);

	/** Saves a result to file. */
	void SaveResult(const FJson& Result);
};

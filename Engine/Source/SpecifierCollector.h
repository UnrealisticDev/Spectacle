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
	 * Identify specifiers in the source directory. 
	 * and output results to results directory.
	 */
	void ParseSpecifiers();

private:

	using FJson = nlohmann::json;

private:

	/** Ensures that parsed specifier has valid type, key, meta, and count values. */
	void ValidateParsedSpecifier(const FJson& ParsedSpecifier);

	/** Converts parsed specifier to result. */
	void ConvertParsedSpecifierToResult(const FJson& ParsedSpecifier, const std::filesystem::path& RelativeSourcePath, FJson& Result);

	/** 
	 * Ensures that a result has valid type, key, and meta values
	 * and that it has at least one occurrence.
	 */
	void ValidateResult(const FJson& Result);

	/** 
	 * Returns a unique filename that depends on the
	 * specifier type and key provided.
	 */
	FString GetResultFilename(const FJson& Type, const FJson& Key) const;
	FString GetResultFilename(const FJson& Result) const;

	/** Saves a result to file. */
	void SaveResult(const FJson& Result);

public:

	/** 
	 * Trims all results down to desired size on a
	 * per-specifier basis.
	 */
	void TrimResults(int Size);

private:

	/** Trims result down to desired size. */
	void TrimResult(FJson& Result, int Size);

public:

	/** Upload specifier stats to server. */
	void Upload();
};

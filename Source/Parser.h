#pragma once

#include "CoreTypes.h"
#include "ParsingTypes.h"
#include "map"

/** 
 * A custom parser that identifies
 * Unreal Engine 4 specifiers in a series of tokens.
 */
class FParser
{
public:

	FParser();

	/** 
	 * Parse tokens and identify relevant specifiers.
	 *  
	 * @return A map of specifier counts by specifier type.
	 */
	void FindSpecifiers(TArray<FToken> InTokens);

private:

	/** 
	 * Parse specifiers within Unreal Engine macro.
	 * Expected to be called after prompting macro identifier (e.g. UMETA).
	 * 
	 * @param Index			Current index of iteration over token list.
	 * @param Specifiers	List of specifiers to modify.
	 */
	void IdentifySpecifiersWithinMacro(int32& Index, std::map<FString, int32>& SpecCountMap);

	/** 
	 * Capitalize specifier string.
	 */
	FString CapitalizeSpecifier(FString Specifier);

	/** Cached tokens. */
	TArray<FToken> Tokens;
};
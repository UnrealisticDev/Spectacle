#pragma once

#include "CoreTypes.h"
#include "ParsingTypes.h"

/** 
 * A custom parser that identifies
 * Unreal Engine 4 specifiers in a series of tokens.
 */
class FParser
{
public:

	FParser();

	/** Parse tokens and identify relevant specifiers. */
	void IdentifyUnrealSpecifiers(TArray<FToken> InTokens);

	/** Output map of specifier counts to JSON. */
	void ToJSON(const FString& OutFilepath);

	/** Print debug info for specifier counts. */
	void Dump(bool bVerbose = false);

private:

	/** Gets the next token in the stream. */
	FToken GetToken();

	/** Peek the next token, without affecting cursor position. */
	FToken PeekToken();

	/** 
	 * Reverts the last token get,
	 * updating cursor position to match.
	 */
	void UngetToken();

	/** Match the next token as punctuation. */
	bool MatchPunctuator(const FString& Query);

	/** Require the next token to be punctuation. */
	void RequirePunctuator(const FString& Query);

	/** Determines if a token is a UE4 macro. */
	bool IsUnrealMacroToken(const FToken& Token);

	/** 
	 * Finds the next Unreal Engine 4 macro in the stream. 
	 * 
	 * @return The macro token, or invalid token if not found. 
	 */
	FToken GetUnrealMacroToken();

	/** Derives the specifier type enum value from a macro token. */
	EUnrealSpecifierType DeriveUnrealSpecifierType(FToken& MacroToken);

	/** 
	 * Parse specifiers within Unreal Engine 4 macro.
	 * Expected to be called after prompting macro identifier (e.g. UMETA).
	 * 
	 * @param Index			Current index of iteration over token list.
	 * @param Specifiers	List of specifiers to modify.
	 */
	void IdentifySpecifiersWithinMacro(EUnrealSpecifierType SpecifierType, FSpecifierCountMap& SpecifierCountMap);

private:

	/** Cached tokens. */
	TArray<FToken> Tokens;

	/** Current position of the cursor in the token stream. */
	int32 CursorPos;

	/** Previous position of the cursor in the token stream. */
	int32 PreviousPos;

	/** Cached results of parsing which maps specifiers to count. */
	FSpecifierCountMap SpecifierCountMap;
};
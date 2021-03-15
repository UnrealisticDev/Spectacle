#pragma once

#include "CoreTypes.h"
#include "ParsingTypes.h"

/** 
 * Character utilities. 
 */
struct FChar
{
	static bool IsNewline(char Char);
	static bool IsWhitespace(char Char);

	static bool IsAlphabetic(char Char);
	static bool IsNumeric(char Char);
	static bool IsAlphanumeric(char Char);

	static bool IsPunctuator(char Char);
	static bool IsForwardSlash(char Char);
	static bool IsSingleQuote(char Char);
	static bool IsDoubleQuote(char Char);
};

/** 
 * This is a general purpose lexer that is capable of 
 * creating tokens from a stream of C++ input text.
 */
class FLexer
{
public:

	/** 
	 * Generate tokens from the provided input text.
	 */
	TArray<FToken> Tokenize(const char* InSource);

	/** Print debug info for an array of tokens. */
	static void Dump(TArray<FToken> Tokens);

private:

	/** 
	 * Reset stream with new source.
	 */
	void Reset(const char* InSource);

	/** 
	 * Get the next character in the stream (at cursor position).
	 */
	char GetChar();

	/** 
	 * Revert the last character get in the stream, 
	 * updating the cursor position accordingly. 
	 */
	void UngetChar();

	/** 
	 * Peek the next character in the stream,
	 * without incrementing the cursor position.
	 */
	char PeekChar();

	/** 
	 * Get the next character in the stream that is not whitespace. 
	 */
	char GetLeadingChar();

	/** 
	 * Get next token in the stream.
	 * 
	 * @return True if token identified, false otherwise (e.g. EOF reached)
	 */
	bool GetToken(FToken& Token);

private:

	/** Source text. */
	const char* Source;

	/** Source length. */
	int32 SourceLength;

	/** Current position of the cursor in the text. */
	int32 CursorPos;

	/** Previous position of the cursor in the text. */
	int32 PreviousPos;
};
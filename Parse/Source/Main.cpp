#include <iostream>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include "CoreTypes.h"
#include "CorePath.h"
#include "Lexer.h"
#include "Parser.h"

/** 
 * This tool parses an Unreal Engine 4 header file 
 * for specifiers and outputs specifiers and their respective
 * occurrence counts.
 * 
 * @param 1 - Filepath of the source file to be parsed.
 * @param 2 - (Optional) Filename + extension of file to output results to. Default: "Parsed.json"
 * @param 3 - (Optional) -debug = Prints output to console.
 */
int main(int ArgumentCount, char* Arguments[])
{
	FString SourcePath = ArgumentCount > 1 ? Arguments[1] : "";
	if ( !std::filesystem::exists(SourcePath) )
	{
		throw std::invalid_argument(FString("Source file not found: ") + SourcePath + ". Nothing to parse.");
	}

	FString SourceContent;
	std::ifstream SourceFile(SourcePath);
	if ( SourceFile.is_open() )
	{
		FString Line;
		while ( !SourceFile.eof() )
		{
			std::getline(SourceFile, Line);
			SourceContent += Line + "\n\r"; // Normalize line endings
		}
	}

	else
	{
		throw std::runtime_error("Unable to open source file.");
	}

	FLexer Lexer;
	TArray<FToken> Tokens = Lexer.Tokenize(SourceContent.c_str());

	FParser Parser;
	Parser.IdentifyUnrealSpecifiers(Tokens);
	Parser.ToJSON
	(
		FPaths::TempDirectory()
		.append
		(
			ArgumentCount > 2 && std::filesystem::path(Arguments[2]).has_extension() 
			? Arguments[2] 
			: "Parsed.json"
		)
		.string()
	);

	if ( ArgumentCount > 3 && Arguments[3] == "-debug" )
	{
		Parser.Dump(false);
	}
}
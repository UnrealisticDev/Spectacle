#include <iostream>
#include <fstream>
#include <filesystem>
#include "CoreTypes.h"
#include "Lexer.h"
#include "Parser.h"

/** 
 * This tool parses an Unreal Engine 4 header file
 * for specifiers and outputs specifiers and their respective
 * occurrence counts.
 * 
 * @param 1 - Filepath of file to be parsed.
 * @param 2 - (Optional) Filepath of file to output results to. 
 * @param 3 - (Optional) -debug = Prints output to console.
 */
int main(int ArgumentCount, char* Arguments[])
{
	if ( ArgumentCount < 2 )
	{
		std::cerr << "No filepath argument provided." << std::endl;
		return -1;
	}

	FString Filename = Arguments[1];
	if ( !std::filesystem::exists(Filename) )
	{
		std::cerr << "File not found: " << Arguments[1] << std::endl;
		return -1;
	}

	FString FileContent;

	std::ifstream File(Filename);
	if ( File.is_open() )
	{
		FString Line = "";
		while ( !File.eof() )
		{
			std::getline(File, Line);
			FileContent += Line + "\n\r"; // Normalize line endings
		}
	}

	FLexer Lexer;
	FParser Parser;

	FSpecifierCountMap SpecifierCountMap = Parser.IdentifyUnrealSpecifiers(Lexer.Tokenize(FileContent.c_str()));
	FParser::ToJSON(SpecifierCountMap, Arguments[2] ? Arguments[2] : "Out/Output.json");

	if ( ArgumentCount > 3 && Arguments[3] == "-debug" )
	{
		FParser::Dump(SpecifierCountMap);
	}
}
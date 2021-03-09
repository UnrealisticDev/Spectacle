#include <iostream>
#include <fstream>
#include <filesystem>
#include "CoreTypes.h"
#include "Lexer.h"
#include "Parser.h"

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

	FParser::Dump(Parser.IdentifyUnrealSpecifiers(Lexer.Tokenize(FileContent.c_str())));
}
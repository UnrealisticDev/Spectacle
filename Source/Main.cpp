#include <iostream>
#include <fstream>
#include "CoreTypes.h"
#include "Lexer.h"
#include "Parser.h"

int main()
{
	FString Filename = "Test.txt";
	FString FileContents = "";

	std::ifstream File(Filename);
	if (File.is_open())
	{
		FString Line = "";
		while (!File.eof())
		{
			std::getline(File, Line);
			FileContents += Line + "\n\r"; // Normalize line endings
		}
	}

	FLexer Lexer;
	FParser Parser;

	Parser.IdentifyUnrealSpecifiers(Lexer.Tokenize(FileContents.c_str()));
}
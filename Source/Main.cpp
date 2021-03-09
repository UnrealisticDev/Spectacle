#include <fstream>
#include "CoreTypes.h"
#include "Lexer.h"
#include "Parser.h"

// Currently just for testing
int main()
{
	FString FileContents = "";

	std::ifstream File("Test.txt");
	if (File.is_open())
	{
		FString Line = "";
		while (!File.eof())
		{
			std::getline(File, Line);
			FileContents += Line + "\n\r";
		}
	}

	FLexer Lexer;
	TArray<FToken> Tokens = Lexer.Tokenize(FileContents.c_str());
	FLexer::Dump(Tokens);
}
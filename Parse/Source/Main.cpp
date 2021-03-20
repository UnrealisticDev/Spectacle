#include <iostream>
#include <fstream>
#include <filesystem>
#include <bitset>
#include <stdexcept>
#include "CoreTypes.h"
#include "CorePath.h"
#include "Lexer.h"
#include "Parser.h"

enum class EReturn : uint8
{
	Success				= 1 << 0,
	BadFilePath			= 1 << 1,
	FileInaccessible	= 1 << 2
};

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
	if (SourcePath.empty() || !std::filesystem::exists(SourcePath) || !std::filesystem::is_regular_file(SourcePath))
	{
		std::cerr
			<< "Source file not found: ["
			<< SourcePath << "]. "
			<< "Nothing to parse."
			<< std::endl;

		return (uint8)EReturn::BadFilePath;
	}

	namespace fs = std::filesystem;
	fs::perms Permissions = fs::status(SourcePath).permissions() & fs::perms::owner_read;
	if (Permissions == fs::perms::none)
	{
		std::cerr
			<< "Source file inaccessible: ["
			<< SourcePath << "]. "
			<< std::endl;

		return (uint8)EReturn::FileInaccessible;
	}

	FString SourceContent;
	std::ifstream SourceFile(SourcePath);
	FString Line;
	while (std::getline(SourceFile, Line))
	{
		SourceContent += Line + "\n\r"; // Normalize line endings
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

	if (ArgumentCount > 3 && Arguments[3] == "-debug")
	{
		Parser.Dump(false);
	}
}
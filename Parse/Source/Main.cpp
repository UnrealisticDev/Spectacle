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
	Success				= 0 << 0,
	BadFilePath			= 1 << 0,
	FileInaccessible	= 1 << 1,
	FailedtoReadFile	= 1 << 2,
	LexingError			= 1 << 3,
	ParsingError		= 1 << 4,
	OutputError			= 1 << 5
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
	try
	{
		std::ifstream SourceFile(SourcePath);
		FString Line;
		while (std::getline(SourceFile, Line))
		{
			SourceContent += Line + "\n\r"; // Normalize line endings
		}
	}
	catch (std::ifstream::failure e)
	{
		std::cerr << "Failed to read source file: " << e.what() << std::endl;
		return (uint8)EReturn::FailedtoReadFile;
	}
	
	TArray<FToken> Tokens;
	try
	{
		FLexer Lexer;
		Tokens = Lexer.Tokenize(SourceContent.c_str());
	}
	catch (std::runtime_error e)
	{
		std::cerr << "Encountered lexing error: " << e.what() << std::endl;
		return (uint8)EReturn::LexingError;
	}

	try 
	{
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
	}
	catch (std::ofstream::failure e)
	{
		std::cerr << "Encountered output error: " << e.what() << std::endl;
		return (uint8)EReturn::OutputError;
	}
	catch (std::runtime_error e)
	{
		std::cerr << "Encountered parsing error: " << e.what() << std::endl;
		return (uint8)EReturn::ParsingError;
	}

	return (uint8)EReturn::Success;
}

#include "Parser.h"
#include <iostream>
#include <ios>
#include <iomanip>
#include <fstream>
#include <stdexcept>
#include <assert.h>
#include "json.hpp"

FParser::FParser()
{
	CursorPos = 0;
	PreviousPos = 0;
}

void FParser::IdentifyUnrealSpecifiers(TArray<FToken> InTokens)
{
	Tokens = InTokens;

	FToken MacroToken;
	while ( PeekToken().IsValid() )
	{
		MacroToken = GetUnrealMacroToken();
		if ( MacroToken.IsValid() )
		{
			IdentifySpecifiersWithinMacro
			(
				DeriveUnrealSpecifierType(MacroToken), 
				SpecifierCountMap
			);
		}
	}
}

void FParser::ToJSON(const FString& OutFilepath)
{
	std::ofstream OutFile(OutFilepath);
	if (OutFile.is_open())
	{
		using FJson = nlohmann::json;
		FJson Output;
		{
			Output["specifiers"] = {};

			FJson Specifier;
			for (const std::pair<FUnrealSpecifier, int32>& SpecifierCount : SpecifierCountMap)
			{
				Specifier.clear();
				{
					Specifier["type"] = ToString(SpecifierCount.first.Type);
					Specifier["key"] = SpecifierCount.first.Key;
					Specifier["meta"] = SpecifierCount.first.bMetadata;
					Specifier["count"] = SpecifierCount.second;
				}
				Output["specifiers"] += Specifier;
			}
		}
		OutFile << Output.dump();
	}
}

void FParser::Dump(bool bVerbose /*= false*/)
{
	if (bVerbose)
	{
		for (std::pair<FUnrealSpecifier, int32> SpecifierCount : SpecifierCountMap)
		{
			std::cout << std::left << std::setfill(' ')
				<< std::setw(16) << "(" + ToString(SpecifierCount.first.Type) + ")"
				<< std::setw(8) << (SpecifierCount.first.bMetadata ? "meta" : " ")
				<< std::setw(40) << SpecifierCount.first.Key
				<< " --> "
				<< std::setw(8) << SpecifierCount.second
				<< std::endl;
		}
	}

	else
	{
		int Index = 0;
		std::cout << "Identified specifiers: [";
		for (std::pair<FUnrealSpecifier, int32> SpecifierCount : SpecifierCountMap)
		{
			if (Index > 0)
			{
				std::cout << ", ";
			}
			std::cout << SpecifierCount.first.Key;

			++Index;
		}
		std::cout << "]" << std::endl;
	}
}

FToken FParser::GetToken()
{
	PreviousPos = CursorPos;
	return (size_t)CursorPos < Tokens.size() ? Tokens[CursorPos++] : FToken();
}

FToken FParser::PeekToken()
{
	return (size_t)CursorPos < Tokens.size() ? Tokens[CursorPos] : FToken();
}

void FParser::UngetToken()
{
	CursorPos = PreviousPos;
}

bool FParser::MatchPunctuator(const FString& Query)
{
	FToken Token = GetToken();
	if (Token.IsValid())
	{
		if (Token.Type == ETokenType::Punctuator && Token.Matches(Query))
		{
			return true;
		}

		else
		{
			UngetToken();
		}
	}

	return false;
}

void FParser::RequirePunctuator(const FString& Query)
{
	if ( !MatchPunctuator(Query) )
	{
		throw std::runtime_error(FString("Missing punctuator: ") + Query);
	}
}

bool FParser::IsUnrealMacroToken(const FToken& Token)
{
	if (Token.Type == ETokenType::Identifier)
	{
		static FString Macros[8] =
		{
			"UCLASS",
			"USTRUCT",
			"UPROPERTY",
			"UFUNCTION",
			"UENUM",
			"UMETA",
			"UINTERFACE"
		};

		for (const FString& Macro : Macros)
		{
			if (Token.Value == Macro)
			{
				return true;
			}
		}
	}

	return false;
}

FToken FParser::GetUnrealMacroToken()
{
	FToken Token;
	do 
	{
		Token = GetToken();
		if ( IsUnrealMacroToken(Token) )
		{
			break;
		}

	} while ( Token.IsValid() );

	return Token;
}

EUnrealSpecifierType FParser::DeriveUnrealSpecifierType(FToken& MacroToken)
{
	assert(MacroToken.IsValid());

	static std::unordered_map<FString, EUnrealSpecifierType> Macros =
	{
		{"UCLASS", EUnrealSpecifierType::Class},
		{"USTRUCT",	EUnrealSpecifierType::Struct},
		{"UPROPERTY", EUnrealSpecifierType::Property},
		{"UFUNCTION", EUnrealSpecifierType::Function},
		{"UENUM", EUnrealSpecifierType::Enum},
		{"UMETA", EUnrealSpecifierType::EnumMeta},
		{"UINTERFACE", EUnrealSpecifierType::Interface}
	};

	return Macros.find(MacroToken.Value)->second;
}

void FParser::IdentifySpecifiersWithinMacro(EUnrealSpecifierType SpecifierType, FSpecifierCountMap& SpecifierCountMap)
{
	RequirePunctuator("(");

	int32 SpecifierCount = 0;

	while ( !MatchPunctuator(")") )
	{
		if ( SpecifierCount > 0 )
		{
			RequirePunctuator(",");
		}

		if ( PeekToken().Type != ETokenType::Identifier )
		{
			GetToken();
			continue;
		}

		++SpecifierCount;

		FToken Specifier = GetToken();
		if ( !Specifier.IsValid() )
		{
			throw std::runtime_error("Found invalid token.");
		}

		// Metadata
		if ( Specifier.Matches("meta", false) )
		{
			RequirePunctuator("=");
			RequirePunctuator("(");

			int32 MetaSpecifierCount = 0;
			while ( !MatchPunctuator(")") )
			{
				if ( MetaSpecifierCount > 0 )
				{
					RequirePunctuator(",");
				}
				++MetaSpecifierCount;

				Specifier = GetToken();
				FUnrealSpecifier USpecifier(SpecifierType, true, Specifier.Value);
				if (SpecifierCountMap.find(USpecifier) != SpecifierCountMap.end())
				{
					SpecifierCountMap[USpecifier] += 1;
				}

				else
				{
					SpecifierCountMap[USpecifier] = 1;
				}

				if ( MatchPunctuator("=") )
				{
					while ( PeekToken().Value != "," && PeekToken().Value != ")" )
					{
						GetToken(); // Value
					}
				}
			}
		}

		// Top-level specifier
		else
		{
			FUnrealSpecifier USpecifier(SpecifierType, false, Specifier.Value);
			if ( SpecifierCountMap.find(USpecifier) != SpecifierCountMap.end() )
			{
				SpecifierCountMap[USpecifier] += 1;
			}

			else
			{
				SpecifierCountMap[USpecifier] = 1;
			}

			if ( MatchPunctuator("=") )
			{
				if ( MatchPunctuator("(") )
				{
					while ( !MatchPunctuator(")") )
					{
						GetToken(); // Value(s)
					}
				}

				else
				{
					while ( PeekToken().Value != "," && PeekToken().Value != ")" )
					{
						GetToken(); // Value
					}
				}
			}

			else if ( MatchPunctuator("(") )
			{
				while ( !MatchPunctuator(")") )
				{
					GetToken(); // Value
				}
			}

			else
			{
				// no op
			}
		}
	}
}

#include "Parser.h"
#include <iostream>

FParser::FParser()
{
	// noop
}

void FParser::FindSpecifiers(TArray<FToken> InTokens)
{
	Tokens = InTokens;

	using FSpecCountMapByType = std::map<EUnrealSpecifierType, std::map<FString, int32>>;
	FSpecCountMapByType SpecCountMapByType;

	for ( int32 Index = 0; Index < (int32)Tokens.size(); )
	{
		FToken Token = Tokens[Index];

		if (Token.Matches("UCLASS"))
		{
			std::cout << "Found a UCLASS spec.\n";
			IdentifySpecifiersWithinMacro
			(
				Index, 
				SpecCountMapByType[EUnrealSpecifierType::Class]
			);
		}

		else if (Token.Matches("USTRUCT"))
		{
			std::cout << "Found a USTRUCT spec.\n";
			IdentifySpecifiersWithinMacro
			(
				Index,
				SpecCountMapByType[EUnrealSpecifierType::Struct]
			);
		}

		else if (Token.Matches("UPROPERTY"))
		{
			std::cout << "Found a UPROPERTY spec.\n";
			IdentifySpecifiersWithinMacro
			(
				Index,
				SpecCountMapByType[EUnrealSpecifierType::Property]
			);
		}

		else if (Token.Matches("UFUNCTION"))
		{
			std::cout << "Found a UFUNCTION spec.\n";
			IdentifySpecifiersWithinMacro
			(
				Index,
				SpecCountMapByType[EUnrealSpecifierType::Function]
			);
		}

		else if (Token.Matches("UENUM"))
		{
			std::cout << "Found a UENUM spec.\n";
			IdentifySpecifiersWithinMacro
			(
				Index,
				SpecCountMapByType[EUnrealSpecifierType::Enum]
			);
		}

		else if (Token.Matches("UMETA"))
		{
			std::cout << "Found a UMETA spec.\n";
			IdentifySpecifiersWithinMacro
			(
				Index,
				SpecCountMapByType[EUnrealSpecifierType::EnumMeta]
			);
		}

		else if (Token.Matches("UINTERFACE"))
		{
			std::cout << "Found a UINTERFACE spec.\n";
			IdentifySpecifiersWithinMacro
			(
				Index,
				SpecCountMapByType[EUnrealSpecifierType::Interface]
			);
		}

		else
		{
			++Index;
		}
	}

	for (auto Map : SpecCountMapByType)
	{
		std::printf("Spec type: %s\n", ToString(Map.first).c_str());
		for (auto Pair : Map.second)
		{
			std::printf("\tSpecifier: %s, Count: %d\n", Pair.first.c_str(), Pair.second);
		}
	}
}

void FParser::IdentifySpecifiersWithinMacro(int32& Index, std::map<FString, int32>& SpecCountMap)
{
	bool bRequireComma = false;

	// Step up to opening paren
	FToken Token;
	do
	{
		Token = Tokens[++Index];

	} while ( !Token.Matches("(") );

	// Parse next tokens, until you hit closing paren
	do 
	{
		Token = Tokens[++Index];

		// If token is meta, move into subroutine
		if (Token.Matches("meta"))
		{
			// Introductory symbols =(
			do 
			{
				Token = Tokens[++Index];

			} while ( !Token.Matches("=") );

			do
			{
				Token = Tokens[++Index];

			} while ( !Token.Matches("(") );

			// 
			do 
			{
				Token = Tokens[++Index];

				if (Token.Type == ETokenType::Identifier)
				{
					// Add to count map
					std::cout << "\tKeyword: " << Token.Value << "\n";
					if (SpecCountMap.find(CapitalizeSpecifier(Token.Value)) != SpecCountMap.end())
					{
						SpecCountMap[CapitalizeSpecifier(Token.Value)] += 1;
					}
					
					else
					{
						SpecCountMap.emplace(CapitalizeSpecifier(Token.Value), 1);
					}

					// Check next
					Token = Tokens[++Index];

					// If comma, lets loop again
					if (Token.Matches(","))
					{
						continue;
					}

					// If equal symbol...
					else if (Token.Matches("="))
					{
						// Get next, which should be value
						Token = Tokens[++Index];
						// Do nothing with value for the time being
					}
				}

			} while ( !Token.Matches(")") );
		}

		else if (Token.Type == ETokenType::Identifier)
		{
			// Add to list
			std::cout << "\tKeyword: " << Token.Value << "\n";
			if (SpecCountMap.find(CapitalizeSpecifier(Token.Value)) != SpecCountMap.end())
			{
				SpecCountMap[CapitalizeSpecifier(Token.Value)] += 1;
			}

			else
			{
				SpecCountMap.emplace(CapitalizeSpecifier(Token.Value), 1);
			}

			// Check next
			Token = Tokens[++Index];

			// If comma, lets loop again
			if (Token.Matches(","))
			{
				continue;
			}

			// If equal symbol...
			else if (Token.Matches("="))
			{
				// Get next, which should be value
				Token = Tokens[++Index];
				// Do nothing with value for the time being
			}
		}

	} while ( !Token.Matches(")") );

	// Step to next token
	++Index;
}

FString FParser::CapitalizeSpecifier(FString Specifier)
{
	Specifier[0] = toupper(Specifier[0]);
	return Specifier;
}

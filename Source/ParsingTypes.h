#pragma once

#include "CoreTypes.h"

/** 
 * Type of token.
 */
enum class ETokenType : uint8
{
	None,			// Not a token.
	Comment,		// Comment.
	Identifier,		// An alphanumeric identifier.
	Punctuator,		// A symbol.
	Literal,		// A constant literal.
	Max
};

inline FString ToString(const ETokenType& TokenType)
{
	switch (TokenType)
	{
	case ETokenType::None:
		return "None";
		break;
	case ETokenType::Comment:
		return "Comment";
		break;
	case ETokenType::Identifier:
		return "Identifier";
		break;
	case ETokenType::Punctuator:
		return "Punctuator";
		break;
	case ETokenType::Literal:
		return "Literal";
		break;
	case ETokenType::Max:
		return "Max";
		break;
	default:
		break;
	}

	return "";
}

/** 
 * Information about a token, 
 * the smallest element of a C++ program.
 */
struct FToken
{
	/** The type of token. */
	ETokenType Type;

	/** The raw value of the token. */
	FString Value;

	/** Compares a token to a string. */
	bool Matches(const FString Query) const
	{
		return Value == Query;
	}

	/** Clears the contents of the token. */
	void Reset()
	{
		Type = ETokenType::None;
		Value.clear();
	}
};

/** 
 * Listing of the types of
 * Unreal Engine 4 specifiers.
 */
enum class EUnrealSpecifierType
{
	Class,
	Enum,
	EnumMeta,
	Function,
	Interface,
	Property,
	Struct,
	Max
};

inline FString ToString(EUnrealSpecifierType E)
{
	switch (E)
	{
	case EUnrealSpecifierType::Class:
		return "Class";
		break;
	case EUnrealSpecifierType::Enum:
		return "Enum";
		break;
	case EUnrealSpecifierType::EnumMeta:
		return "EnumMeta";
		break;
	case EUnrealSpecifierType::Function:
		return "Function";
		break;
	case EUnrealSpecifierType::Interface:
		return "Interface";
		break;
	case EUnrealSpecifierType::Property:
		return "Property";
		break;
	case EUnrealSpecifierType::Struct:
		return "Struct";
		break;
	case EUnrealSpecifierType::Max:
		return "Max";
		break;
	default:
		return "Error";
		break;
	}
}
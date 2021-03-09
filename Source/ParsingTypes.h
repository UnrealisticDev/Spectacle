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
	Punctuator,		// A punctuation mark.
	Literal			// A constant literal.
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
	default:
		return "Error";
		break;
	}
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
	bool Matches(const FString Query, bool bCaseSensitive = true) const
	{
		return Value == Query;
	}

	/** Clears the contents of the token. */
	void Reset()
	{
		Type = ETokenType::None;
		Value.clear();
	}

	/** Returns true so long as the token has been set. */
	bool IsValid() const
	{
		return Type != ETokenType::None;
	}
};

/** 
 * Enumeration of the types of
 * Unreal Engine 4 specifiers.
 */
enum class EUnrealSpecifierType
{
	None,
	Class,
	Enum,
	EnumMeta,
	Function,
	Interface,
	Property,
	Struct,
	Max
};

inline FString ToString(const EUnrealSpecifierType& SpecifierType)
{
	switch (SpecifierType)
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

/** 
 * An Unreal Engine 4 specifier with a key/name
 * and optional value(s).
 */
struct FUnrealSpecifier
{
	EUnrealSpecifierType Type;
	bool bMetadata;
	FString Key;
	TArray<FString> Values;

	FUnrealSpecifier()
		: Type(EUnrealSpecifierType::None)
		, bMetadata(false)
	{}

	FUnrealSpecifier(EUnrealSpecifierType InType, bool bInMetadata, FString InKey)
		: Type(InType)
		, bMetadata(bInMetadata)
		, Key(InKey)
	{}

	bool operator==(const FUnrealSpecifier& Other) const
	{
		return Type == Other.Type
			&& bMetadata == Other.bMetadata
			&& Key == Other.Key;
	}
};

namespace std
{
	template<>
	struct hash<FUnrealSpecifier>
	{
		std::size_t operator()(const FUnrealSpecifier& Specifier) const
		{
			return hash<int>()((int)Specifier.Type)
				* hash<FString>()(Specifier.Key)
				* hash<bool>()(Specifier.bMetadata);
		}
	};
}

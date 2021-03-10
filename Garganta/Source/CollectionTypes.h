#pragma once

#include "CoreTypes.h"
#include <algorithm>
#include <unordered_map>

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
	FString Type;
	bool bMetadata;
	FString Key;
	TArray<FString> Values;

	FUnrealSpecifier()
		: Type("")
		, bMetadata(false)
	{
	}

	FUnrealSpecifier(FString InType, bool bInMetadata, FString InKey)
		: Type(InType)
		, bMetadata(bInMetadata)
		, Key(InKey)
	{
		std::transform(Key.begin(), Key.end(), Key.begin(),
			[](unsigned char c) { return std::tolower(c); });
	}

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
			return hash<FString>()(Specifier.Type)
				* hash<FString>()(Specifier.Key)
				* hash<bool>()(Specifier.bMetadata);
		}
	};
}

using FSpecifierStats = std::unordered_map<FUnrealSpecifier, std::unordered_map<FString, int32>>;

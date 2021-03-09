#include "Lexer.h"
#include "iostream"
#include "iomanip"
#include "ios"
#include "ostream"

bool FChar::IsNewline(char Char)
{
	return Char == '\n';
}

bool FChar::IsWhitespace(char Char)
{
	return Char == '\n' || Char == '\t' || Char == '\r' || Char == ' ' || Char == '\v';
}

bool FChar::IsAlphabetic(char Char)
{
	return (Char >= 'a' && Char <= 'z') || (Char >= 'A' && Char <= 'Z');
}

bool FChar::IsNumeric(char Char)
{
	return Char >= '0' && Char <= '9';
}

bool FChar::IsAlphanumeric(char Char)
{
	return IsAlphabetic(Char) || IsNumeric(Char);
}

bool FChar::IsPunctuator(char Char)
{
	char PunctuatorList[28] = 
	{
		'!', '%', '^', '&', '*', '(', ')', 
		'-','+', '=', '{', '}', '|', '~',
		'[', ']', '\\', ';', '\'', ':', '"',
		'<', '>', '?', ',', '.', '/', '#'
	};

	for (char c : PunctuatorList)
	{
		if (c == Char)
		{
			return true;
		}
	}

	return false;
}

bool FChar::IsForwardSlash(char Char)
{
	return Char == '/';
}

bool FChar::IsSingleQuote(char Char)
{
	return Char == '\'';
}

bool FChar::IsDoubleQuote(char Char)
{
	return Char == '"';
}

TArray<FToken> FLexer::Tokenize(const char* InSource)
{
	Reset(InSource);

	TArray<FToken> Tokens;
	FToken Token;

	while (GetToken(Token))
	{
		Tokens.push_back(Token);
	}

	return Tokens;
}

void FLexer::Dump(TArray<FToken> Tokens)
{
	int32 Index = 0;
	for (const FToken& Token : Tokens)
	{
		if ( Token.Type == ETokenType::Comment )
		{
			continue;
		}

		std::cout << std::left << std::setfill(' ')
			<< std::setw(12) << "Token[" + std::to_string(Index) + "]"
			<< std::setw(10) << std::right << ToString(Token.Type)
			<< std::left << " --> "
			<< std::setw(80) << Token.Value
			<< std::endl;

		Index += 1;
	}
}

void FLexer::Reset(const char* InSource)
{
	Source = InSource;
	SourceLength = strlen(Source);
	CursorPos = 0;
	PreviousPos = 0;
}

char FLexer::GetChar()
{
	PreviousPos = CursorPos;
	return Source[CursorPos++];
}

void FLexer::UngetChar()
{
	CursorPos = PreviousPos;
}

char FLexer::PeekChar()
{
	return Source[CursorPos];
}

char FLexer::GetLeadingChar()
{
	char Char;

	do 
	{
		Char = GetChar();

	} while (Char != 0 && FChar::IsWhitespace(Char));

	return Char;
}

bool FLexer::GetToken(FToken& Token)
{
	Token.Reset();

	char Char = GetLeadingChar();

	// End of file reached
	if (Char == 0)
	{
		UngetChar();
		return false;
	}

	// Start @ punctuator
	else if ( FChar::IsPunctuator(Char) )
	{
		Token.Type = ETokenType::Punctuator;

		// Check for comments
		if ( FChar::IsForwardSlash(Char) )
		{
			// Single-line comment
			if (PeekChar() == '/')
			{
				Token.Type = ETokenType::Comment;

				// Move past '//'
				GetChar();

				while (!FChar::IsNewline(PeekChar()))
				{
					Char = GetChar();
					Token.Value += Char;
				}

				return true;
			}

			// Multi-line comment
			if (PeekChar() == '*')
			{
				Token.Type = ETokenType::Comment;

				// Move past '*' to avoid '/*/' situation
				GetChar();

				Char = GetChar();
				while (Char != '*' || PeekChar() != '/')
				{
					if (Char == 0)
					{
						std::cout << "End of file reached before comment terminated.\n";
						return false;
					}

					Token.Value += Char;
					Char = GetChar();
				}

				// Move cursor past terminating '/'
				GetChar();
				return true;
			}

			else
			{
				Token.Value += Char;
			}
		}

		// Check for char literals
		else if ( FChar::IsSingleQuote(Char) )
		{
			Token.Type = ETokenType::Literal;

			bool bEscaped = false;

			Token.Value += Char;
			while ( !FChar::IsSingleQuote(PeekChar()) || bEscaped )
			{
				Char = GetChar();
				if ( Char == '\\' && FChar::IsSingleQuote(PeekChar()) )
				{
					bEscaped = true;
				}

				else if (bEscaped)
				{
					bEscaped = false;
				}

				Token.Value += Char;
			}
			Token.Value += GetChar();
		}

		// Check for string literals
		else if ( FChar::IsDoubleQuote(Char) )
		{
			Token.Type = ETokenType::Literal;

			bool bEscaped = false;

			Token.Value += Char;
			while ( !FChar::IsDoubleQuote(PeekChar()) || bEscaped )
			{
				Char = GetChar();
				if (Char == '\\' && FChar::IsDoubleQuote(PeekChar()))
				{
					bEscaped = true;
				}

				else if (bEscaped)
				{
					bEscaped = false;
				}

				Token.Value += Char;
			}
			Token.Value += GetChar();
		}

		else
		{
			Token.Value += Char;
		}
	}

	// Start @ underscore
	else if ( Char == '_' )
	{
		Token.Type = ETokenType::Identifier;
		
		Token.Value += Char;
		while ( FChar::IsAlphanumeric(PeekChar()) || PeekChar() == '_' )
		{
			Char = GetChar();
			Token.Value += Char;
		}
	}

	// Start @ alphabetic
	else if (FChar::IsAlphabetic(Char))
	{
		Token.Type = ETokenType::Identifier;

		// Check for char/string literals
		if (Char == 'L' || Char == 'U' || Char == 'u')
		{
			bool bCharLiteral = false;
			bool bStringLiteral = false;

			if (FChar::IsSingleQuote(PeekChar()))
			{
				bCharLiteral = true;

				Token.Value += Char;
				Char = GetChar();
			}

			else if (FChar::IsDoubleQuote(PeekChar()))
			{
				bStringLiteral = true;

				Token.Value += Char;
				Char = GetChar();
			}

			else if (Char == 'u' && PeekChar() == '8')
			{
				GetChar();

				if (FChar::IsSingleQuote(PeekChar()))
				{
					bCharLiteral = true;

					Token.Value += "u8";
					Char = GetChar();
				}

				else if (FChar::IsDoubleQuote(PeekChar()))
				{
					bStringLiteral = true;

					Token.Value += "u8";
					Char = GetChar();
				}

				else
				{
					UngetChar();
				}
			}

			if (bCharLiteral)
			{
				Token.Type = ETokenType::Literal;

				bool bEscaped = false;

				Token.Value += Char;
				while (!FChar::IsSingleQuote(PeekChar()) || bEscaped)
				{
					Char = GetChar();
					if (Char == '\\' && FChar::IsSingleQuote(PeekChar()))
					{
						bEscaped = true;
					}

					else if (bEscaped)
					{
						bEscaped = false;
					}

					Token.Value += Char;
				}
				Token.Value += GetChar();

				return true;
			}

			else if (bStringLiteral)
			{
				Token.Type = ETokenType::Literal;

				bool bEscaped = false;

				Token.Value += Char;
				while (!FChar::IsDoubleQuote(PeekChar()) || bEscaped)
				{
					Char = GetChar();
					if (Char == '\\' && FChar::IsDoubleQuote(PeekChar()))
					{
						bEscaped = true;
					}

					else if (bEscaped)
					{
						bEscaped = false;
					}

					Token.Value += Char;
				}
				Token.Value += GetChar();

				return true;
			}

			else
			{
				// no op
			}
		}

		Token.Value += Char;
		while (FChar::IsAlphanumeric(PeekChar()) || PeekChar() == '_')
		{
			Char = GetChar();
			Token.Value += Char;
		}
	}

	// Start @ number
	else if ( FChar::IsNumeric(Char) || Char == '.' )
	{
		Token.Type = ETokenType::Literal;

		Token.Value += Char;
		while ( FChar::IsAlphanumeric(PeekChar()) 
			|| PeekChar() == '.' 
			|| PeekChar() == '\''
			|| PeekChar() == '+')
		{
			Char = GetChar();
			Token.Value += Char;
		}
	}

	return true;
}

#ifdef _DEBUG
#include "TokenType.h"

std::ostream& operator << (std::ostream& os, const TokenType& tokenType)
	{
	std::string tokenTypeName;

	switch (tokenType)
		{
		case TokenType::ID: tokenTypeName = "ID"; break;
		case TokenType::BRAC_OPEN: tokenTypeName = "{"; break;
		case TokenType::BRAC_CLOSE: tokenTypeName = "}"; break;
		case TokenType::NUMBER: tokenTypeName = "NUMBER"; break;
		case TokenType::BLOCK_COMMENT_OPEN: tokenTypeName = "/{"; break;
		case TokenType::STRING: tokenTypeName = "STRING"; break;
		case TokenType::DEGREES: tokenTypeName = "DEGREES"; break;
		case TokenType::RADIANS: tokenTypeName = "RADIANS"; break;
		//operators
		case TokenType::PAR_OPEN: tokenTypeName = "("; break;
		case TokenType::PAR_CLOSE: tokenTypeName = ")"; break;
		case TokenType::SQR_OPEN: tokenTypeName = "["; break;
		case TokenType::SQR_CLOSE: tokenTypeName = "]"; break;
		case TokenType::ASSIGN: tokenTypeName = "="; break;
		case TokenType::EQ: tokenTypeName = "=="; break;
		case TokenType::TRIPLE_EQ: tokenTypeName = "==="; break;
		case TokenType::PLUS: tokenTypeName = "+"; break;
		case TokenType::MINUS: tokenTypeName = "-"; break;
		case TokenType::MULTIPLY: tokenTypeName = "*"; break;
		case TokenType::DIVIDE: tokenTypeName = "/"; break;
		case TokenType::RANGE: tokenTypeName = "RANGE"; break;
		case TokenType::DOT: tokenTypeName = "DOT"; break;
		default: tokenTypeName = "???"; break;
		}
	os << tokenTypeName;
	return os;
	}
#endif // DEBUG



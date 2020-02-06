#include "Token.h"

#ifdef _DEBUG
std::ostream& operator << (std::ostream& os, const Token& token)
	{
	os << "[" << token.line << "," << token.pos << "]" << token.type << ": " << token.str_value;
	return os;
	}
#endif // DEBUG




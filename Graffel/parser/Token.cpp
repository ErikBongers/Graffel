#include "Token.h"


Token::Token(TokenType type, char c, unsigned int line, unsigned int pos)
    : line(line), pos(pos), type(type), stringValue(1, c)
    {}

Token::Token(TokenType type, double n, unsigned int line, unsigned int pos)
    : line(line), pos(pos), type(type), numberValue(n)
    {}

Token::Token(TokenType type, std::string str, unsigned int line, unsigned int pos)
    : line(line), pos(pos), type(type), stringValue(str)
    {}

Token::Token(TokenType type, unsigned int line, unsigned int pos)
    : Token(type, 0.0, line, pos)
    {}


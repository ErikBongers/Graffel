#pragma once
#include "TokenType.h"
#include <string>

class Token
    {
    public:
        TokenType type;
        std::string str_value = "";
        double double_value;
        long line;
        long pos;

    public:
        Token(TokenType type, const std::string& str_value, double double_value, long line, long pos) :type(type), str_value(str_value), double_value(double_value), line(line), pos(pos) {}
        Token(TokenType type, const std::string& str_value, long line, long pos) :Token(type, str_value, 0.0, line, pos) {}
        Token(TokenType type, long line, long pos) :Token(type, "", 0.0, line, pos) {}
        bool operator==(TokenType tt) { return type == tt; }
        bool operator!=(TokenType tt) { return type != tt; }
        static Token UNDEFINED;
        Token() :Token(TokenType::UNDEFINED, -1, -1) {}
    };

std::ostream& operator << (std::ostream& os, const Token& token);


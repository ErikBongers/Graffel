#pragma once
#include <istream>
#include <iostream>
#include <map>

namespace json {
enum class TokenType : int
    {
    UNDEFINED,
    UNKNOWN,
    BRAC_OPEN, // {
    BRAC_CLOSE, // }
    SQR_OPEN, // [
    SQR_CLOSE, // ]
    COMMA, // ,
    COLON, // :
    MINUS,
    NUMBER,
    STRING,
    TRUE_,
    FALSE_,
    NULL_,


    EOT // end of tokens 
    };

class Token
    {
    public:
        json::TokenType type;
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


class Tokenizer
    {
    private:
        std::istream& is;
        bool nextTokenChar();
        void putBack();
        bool getChar();
        char curr_c = 0; //current char
        char back_c = -1;
        int curr_line = 1;
        int curr_pos = 0;
        int back_line = -1;
        int back_pos = -1;

    public:
        Tokenizer(std::istream& is);
        Token next();
        Token parseNumber();
        Token parseString();
        Token parseAlpha();
        std::map<std::string, TokenType> keywords;
    };

}

#ifdef _DEBUG

std::ostream& operator << (std::ostream& os, const json::TokenType& tokenType);
std::ostream& operator << (std::ostream& os, const json::Token& token);

#endif // DEBUG


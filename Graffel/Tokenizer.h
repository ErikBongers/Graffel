#pragma once
#include <istream>
#include "Token.h"

class Tokenizer
    {
    private:
        std::istream &is;
        Token parseIdentifier();
        bool nextTokenChar();
        void putBack();
        bool getChar();
        char curr_c = 0; //current char
        char back_c= -1;
        int curr_line = 1;
        int curr_pos = 0;
        int back_line = -1;
        int back_pos = -1;

    public:
        Tokenizer(std::istream &is) :is(is) {}
        Token next();
        Token parseEquals();
        Token parseNumber();
        Token parseString();
        void parseLineComment();
        void parseComment();
        void parseBlockComment();
    };


#pragma once
#include "Tokenizer.h"

class Parser
    {
    private:
        Tokenizer& tok;
        Token t;
        Token back;
        void next();
        void push_back();
        bool parseAssign();
        bool parseBlock();
        bool parseStatement();
        void parseRVal();
        void parseTerm();
        void parseFactor();
    public:
        Parser(Tokenizer& tok) : tok(tok) {}
        void reportError(const std::string& msg, Token& tok);
        void reportError(const std::string& msg) { reportError(msg, t); }
        void parse();


    };


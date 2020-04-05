#pragma once
#include "Tokenizer.h"
#include "..\syntaxtree\nodes.h"

class Parser
    {
    private:
        Tokenizer& tok;
        Token t;
        Token back;
        void next();
        void push_back();
        Statement* parseStatement();
        Assign* parseAssign();
        bool parseBlock();
        AddExpr* parseRVal();
        MultExpr* parseTerm();
        Factor* parseFactor();
    public:
        Parser(Tokenizer& tok) : tok(tok) {}
        void reportError(const std::string& msg, Token& tok);
        void reportError(const std::string& msg) { reportError(msg, t); }
        void parse();


    };


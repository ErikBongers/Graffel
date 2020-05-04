#pragma once
#include "json.h"
#include "Tokenizer.h"

namespace json {
class Parser
    {
    private:
        Tokenizer& tok;
        Token t;
        Token back;
        void next();
        void push_back();
    public:
        Parser(Tokenizer& tok) : tok(tok) {}
        void reportError(const std::string& msg, Token& tok);
        void reportError(const std::string& msg) { reportError(msg, t); }
        Object parse();

        Value parseValue();
        Object parseObject();
        Value parseArray();
    };

}
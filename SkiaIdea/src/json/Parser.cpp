#include "Parser.h"

using namespace json;

void Parser::next()
    {
    if (back != TokenType::UNDEFINED)
        {
        t = back;
        back = Token::UNDEFINED;
        }
    else
        t = tok.next();
    }



void Parser::push_back()
    {
    if (back != TokenType::UNDEFINED)
        reportError("COMPILER FAULT: 2nd push_back not allowed.", back);
    back = t;
    t = Token::UNDEFINED;
    }



void Parser::reportError(const std::string& msg, Token& tok)
    {
    std::cout << msg << std::endl;
    }

Object Parser::parse()
    {
    return parseObject();
    }

Value Parser::parseValue()
    {
    next();
    if (t == TokenType::STRING)
        return Value(t.str_value);
    else if (t == TokenType::NUMBER)
        return Value(t.double_value);
    else if (t == TokenType::TRUE_)
        return Value(Literal::TRUE_);
    else if (t == TokenType::FALSE_)
        return Value(Literal::FALSE_);
    else if (t == TokenType::NULL_)
        return Value(Literal::NULL_);
    else if (t == TokenType::BRAC_OPEN)
        {
        push_back();
        return Value(parseObject());
        }
    else if (t == TokenType::SQR_OPEN)
        {
        push_back();
        return parseArray();
        }
    else
        push_back();
    return Value(-1); //error
    }

Object Parser::parseObject()
    {
    Object object;
    next();
    if (t != TokenType::BRAC_OPEN)
        return object; //error
    next();
    while (t != TokenType::EOT)
        {
        if (t != TokenType::STRING)
            return object; //error
        std::string name = t.str_value;
        next();
        if (t != TokenType::COLON)
            return object; //error
        object.add(name, parseValue());
        next();
        if (t != TokenType::COMMA)
            break;
        next();
        }
    if (t != TokenType::BRAC_CLOSE)
        return object; //error
    return object;
    }

Value json::Parser::parseArray()
    {
    std::vector<Value> values;
    next();
    if (t != TokenType::SQR_OPEN)
        return Value(-1);//error
    while (t != TokenType::EOT)
        {
        values.push_back(parseValue());
        next();
        if (t != TokenType::COMMA)
            break;
        }
    if (t != TokenType::SQR_CLOSE)
        return Value(-1); //error
    return Value(values);
    }

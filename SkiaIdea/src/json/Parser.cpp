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

Object* Parser::parse()
    {
    return parseObject();
    }

Value Parser::parseValue()
    {
    Value value;
    next();
    if (t == TokenType::STRING)
        value.str = t.str_value;
    else if (t == TokenType::NUMBER)
        value.number = t.double_value;
    else if (t == TokenType::TRUE_)
        value.literal = Literal::TRUE_;
    else if (t == TokenType::FALSE_)
        value.literal = Literal::FALSE_;
    else if (t == TokenType::NULL_)
        value.literal = Literal::NULL_;
    else if (t == TokenType::BRAC_OPEN)
        {
        push_back();
        value.object = parseObject();
        }
    else if (t == TokenType::SQR_OPEN)
        {
        push_back();
        parseArray(value.array);
        }
    else
        push_back();
    return value;
    }

Object* Parser::parseObject()
    {
    Object* object = new Object();
    next();
    if (t != TokenType::BRAC_OPEN)
        return object; //error
    Value value;
    next();
    while (t != TokenType::EOT)
        {
        if (t != TokenType::STRING)
            return object; //error
        std::string name = t.str_value;
        next();
        if (t != TokenType::COLON)
            return object; //error
        value = parseValue();
        object->add(name, value);
        next();
        if (t != TokenType::COMMA)
            break;
        next();
        }
    if (t != TokenType::BRAC_CLOSE)
        return object; //error
    return object;
    }

void json::Parser::parseArray(std::vector<Value> &array)
    {
    next();
    if (t != TokenType::SQR_OPEN)
        return;//error
    while (t != TokenType::EOT)
        {
        array.push_back(parseValue());
        next();
        if (t != TokenType::COMMA)
            break;
        }
    if (t != TokenType::SQR_CLOSE)
        return; //error
    return;
    }

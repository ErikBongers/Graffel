#include "Parser.h"

void Parser::next()
    {
    if (back != TokenType::UNDEFINED)
        {
        t = back;
        back = Token::UNDEFINED;
        }
    t = tok.next();
    }

void Parser::push_back()
    {
    if (back != TokenType::UNDEFINED)
        reportError("COMPILER FAULT: 2nd push_back not allowed.", back);
    }

void Parser::reportError(const std::string& msg, Token& tok)
    {
    std::cout << msg << std::endl;
    }

/// graffel = comment* | assign*
/// comment = xxx // already removed by tokenizer
/// assignment = id "=" ( rval | block)
/// block = "{" statement* "}" ";"* // zero or one statement allowed, semi-colon optional
/// statement = ( assignment ";" | block )
// expressions: top line has lowest precedence:
///rval = term { ("+" | "-") rval } // left or right associativity: https://stackoverflow.com/questions/41784524/preferred-way-of-removing-left-recursion-of-ebnf
///term = factor { ("*" | "/") term }
///factor = const | id | "(", rval, ")"
void Parser::parse()
    {
    if (!parseAssign())
        reportError("Expecting identifier");
    }

bool Parser::parseAssign()
    {
    next();
    if (t != TokenType::ID)
        return false;
    next();
    if (t != TokenType::ASSIGN)
        reportError("Expecting '='", t);
    if(!parseBlock())
        parseRVal();
    next();
    if (t != TokenType::SEMICOLON)
        reportError("Expected ';'.", t);
    return true;
    }

bool Parser::parseBlock()
    {
    next();
    if (t != TokenType::BRAC_OPEN)
        return false;
    while (parseStatement()) {}
    next();

    if (t != TokenType::BRAC_CLOSE)
        reportError("Expecting '}'", t);
    next();
    if (t != TokenType::SEMICOLON)
        push_back();
    return true;
    }

bool Parser::parseStatement()
    {
    if (parseBlock())
        return true;
    return parseAssign();
    }
void Parser::parseRVal()
    {
    parseTerm();
    next();
    if (t == TokenType::PLUS || t == TokenType::MINUS)
        {
        parseRVal();
        }
    }
void Parser::parseTerm()
    {
    parseFactor();
    next();
    if (t == TokenType::MULTIPLY || t == TokenType::DIVIDE)
        {
        parseRVal();
        }
    //else : finished with term
    }

void Parser::parseFactor()
    {
    next();
    if (t == TokenType::PAR_OPEN)
        {
        parseRVal();
        next();
        if (t != TokenType::PAR_CLOSE)
            reportError("Expecting '('");
        }
    else if (t == TokenType::ID)
        {
        //great!
        }
    else if (t == TokenType::NUMBER)
        {
        //also great!
        }
    else
        reportError("Expecting identifier, value or expression");
    }

#include "Parser.h"
#include "..\syntaxtree\printTree.h"

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

/*
EBNF of C++ : http://www.externsoft.ch/download/cpp-iso.html
left or right associativity: https://stackoverflow.com/questions/41784524/preferred-way-of-removing-left-recursion-of-ebnf

graffel = comment* | assign*
comment = xxx // already removed by tokenizer
assignment = id "=" ( rval | block)
block = "{" statement* "}" ";"* // zero or one statement allowed, semi-colon optional
statement = ( assignment ";" | block )

expressions: top line has lowest precedence:
addexpr = multexpr |  
       addexpr ("+" | "-") multexpr 
multexpr = factor |
       multexpr ("*" | "/") factor
factor = const | id | "(", addexpr, ")"
*/

void Parser::parse()
    {
    Statement* statement = NULL;
    statement = parseStatement();
    if (!statement)
        reportError("Expecting statement");
    else
        {
        reportError("Compiled successfully.");
        printProgram(*statement);
        statement->optimize();
        reportError("Optimized:");
        printProgram(*statement);
        }
    }

 Assign* Parser::parseAssign()
    {
    next();
    if (t != TokenType::ID)
        return NULL;
    Assign* assign = new Assign();
    assign->id = t;
    next();
    if (t != TokenType::ASSIGN)
        reportError("Expecting '='", t);
    if(!parseBlock())
        assign->rval = parseRVal();
    if (!assign->rval)
        reportError("Expecting r-value for assignment.");
    return assign;
    }

bool Parser::parseBlock()
    {
    next();
    if (t != TokenType::BRAC_OPEN)
        {
        push_back();
        return false;
        }
    while (parseStatement()) {}
    next();

    if (t == TokenType::BRAC_CLOSE)
        next();
    else
        reportError("Expecting '}'", t);
    
    if (t != TokenType::SEMICOLON)
        push_back();
    return true;
    }

Statement* Parser::parseStatement()
    {
    Statement* stat = new Statement();
    if (parseBlock())
        {
        stat->assignment = NULL; //TODO: should be block!
        return stat;
        }
    stat->assignment = parseAssign();
    next();
    if (t != TokenType::SEMICOLON)
        {
        push_back();
        reportError("Expected ';'", t);
        }
    return stat;
    }

AddExpr* Parser::parseRVal()
    {
    AddExpr* rval = new AddExpr();
    rval->term = parseTerm();
    if (!rval->term)
        return NULL;
    next();
    while(t == TokenType::PLUS || t == TokenType::MINUS)
        {
        rval = new AddExpr(rval, t);
        rval->term = parseTerm();
        next();
        }
    push_back();
    return rval;
    }

MultExpr* Parser::parseTerm()
    {
    MultExpr* term = new MultExpr();
    term->factor = parseFactor();
    if (!term->factor)
        return NULL;
    next();
    while(t == TokenType::MULTIPLY || t == TokenType::DIVIDE)
        {
        term = new MultExpr(term, t);
        term->factor = parseFactor();
        next();
        }
    push_back();
    return term;
    }

Factor* Parser::parseFactor()
    {
    Factor* factor = NULL;
    next();
    if (t == TokenType::PAR_OPEN)
        {
        factor = new Factor();
        factor->rval = parseRVal();
        next();
        if (t != TokenType::PAR_CLOSE)
            reportError("Expecting '('");
        }
    else if (t == TokenType::ID)
        {
        factor = new Factor();
        factor->const_or_id = t;
        }
    else if (t == TokenType::NUMBER)
        {
        factor = new Factor();
        factor->const_or_id = t;
        }
    else
        {
        reportError("Expecting identifier, value or expression");
        push_back();
        }
    return factor;
    }


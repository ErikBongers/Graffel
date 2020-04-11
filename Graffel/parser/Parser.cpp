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

void Parser::parse()
    {
    Statement* statement = NULL;
    statement = parseStatement();
    if (!statement)
        reportError("Expecting statement");
    else
        {
        reportError("Compiled successfully.");
        printProgram(*statement, 999);
        statement->optimize();
        reportError("Optimized:");
        printProgram(*statement, 999);
        }
    }

Statement* Parser::parseStatement()
    {
    parseBlockComment();
    Statement* stat = new Statement();
    stat->block = parseBlock();
    if (stat->block)
        {
        stat->assignment = NULL;
        return stat;
        }
    stat->assignment = parseAssign();
    if (!stat->assignment)
        return NULL;
    next();
    if (t != TokenType::SEMICOLON)
        {
        push_back();
        reportError("Expected ';'", t);
        }
    return stat;
    }

Assign* Parser::parseAssign()
    {
    next();
    if (t != TokenType::ID)
        {
        push_back();
        return NULL;
        }
    Assign* assign = new Assign();
    assign->id = t;
    next();
    if (t != TokenType::ASSIGN)
        reportError("Expecting '='", t);
    if(!(assign->block = parseBlock()))
        if (!(assign->addExpr = parseAddExpr()))
            {
            next();
            if (t == TokenType::STRING)
                assign->str = t.str_value;
            else
                reportError("Expecting value for assignment.");
            }
    return assign;
    }

Block* Parser::parseBlock()
    {
    next();
    if (t != TokenType::BRAC_OPEN)
        {
        push_back();
        return NULL;
        }
    Block* block = new Block();
    Statement* stat = parseStatement();
    while (stat) 
        {
        block->statements.push_back(stat);
        stat = parseStatement();
        }
    next();

    if (t == TokenType::BRAC_CLOSE)
        next();
    else
        reportError("Expecting '}'", t);
    
    if (t != TokenType::SEMICOLON)
        push_back();
    return block;
    }

bool Parser::parseBlockComment()
    {
    next();
    if (t != TokenType::BLOCK_COMMENT_OPEN)
        {
        push_back();
        return false;
        }
    int braceCounter = 0;
    do
        {
        next();
        if (t == TokenType::BRAC_OPEN || t == TokenType::BLOCK_COMMENT_OPEN)
            braceCounter++;
        else
            if (t == TokenType::BRAC_CLOSE)
                braceCounter--;
        } while (braceCounter >= 0);
        return true;
    }

AddExpr* Parser::parseAddExpr()
    {
    AddExpr* addExpr = new AddExpr();
    addExpr->mulExpr = parseMultExpr();
    if (!addExpr->mulExpr)
        return NULL;
    next();
    while(t == TokenType::PLUS || t == TokenType::MINUS)
        {
        addExpr = new AddExpr(addExpr, t);
        addExpr->mulExpr = parseMultExpr();
        next();
        }
    push_back();
    return addExpr;
    }

MultExpr* Parser::parseMultExpr()
    {
    MultExpr* mulExpr = new MultExpr();
    mulExpr->unaryExpr = parseUnaryExpr();
    if (!mulExpr->unaryExpr)
        return NULL;
    next();
    while(t == TokenType::MULTIPLY || t == TokenType::DIVIDE)
        {
        mulExpr = new MultExpr(mulExpr, t);
        mulExpr->unaryExpr = parseUnaryExpr();
        next();
        }
    push_back();
    return mulExpr;
    }

UnaryExpr* Parser::parseUnaryExpr()
    {
    UnaryExpr* unaryExpr = new UnaryExpr();
    next();
    if (t == TokenType::MINUS)
        {
        unaryExpr->unaryOp = t;
        unaryExpr->unaryExpr = parseUnaryExpr();
        }
    else
        {
        push_back();
        unaryExpr->primExpr = parsePrimaryExpr();
        }
    if (unaryExpr->primExpr || unaryExpr->unaryExpr)
        return unaryExpr;
    else
        return NULL;
    }

PrimaryExpr* Parser::parsePrimaryExpr()
    {
    PrimaryExpr* primary = NULL;
    next();
    if (t == TokenType::PAR_OPEN)
        {
        primary = new PrimaryExpr();
        primary->addExpr = parseAddExpr();
        next();
        if (t != TokenType::PAR_CLOSE)
            reportError("Expecting '('");
        }
    else if (t == TokenType::ID)
        {
        primary = new PrimaryExpr();
        primary->const_or_id = t;
        }
    else if (t == TokenType::NUMBER)
        {
        primary = new PrimaryExpr();
        primary->const_or_id = t;
        }
    else
        {
        reportError("Expecting identifier, value or expression");
        push_back();
        }
    return primary;
    }

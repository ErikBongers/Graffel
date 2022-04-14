#pragma once
#include "..\parser\Token.h"
#include <vector>

struct AddExpr;

struct PrimaryExpr
    {
    Token const_or_id;
    //or
    AddExpr* addExpr = NULL;
    Token getConstValue();
    void optimize();
    };


struct UnaryExpr
    {
    PrimaryExpr* primExpr;
    //or
    Token unaryOp;
    UnaryExpr* unaryExpr;
    Token getConstValue()
        {
        if (primExpr)
            return primExpr->getConstValue();
        Token const1 = unaryExpr->getConstValue();
        if (const1.type == TokenType::NUMBER)
            {
            double sign = (unaryOp == TokenType::MINUS) ? -1 : 1;
            return Token(TokenType::NUMBER, "", sign * const1.double_value, 0, 0);
            }
        return Token();
        }
    void optimize()
        {
        if (primExpr)
            return primExpr->optimize();
        Token const1 = unaryExpr->getConstValue();
        if (const1.type == TokenType::NUMBER)
            {
            double sign = (unaryOp == TokenType::MINUS) ? -1 : 1;
            primExpr = new PrimaryExpr();
            primExpr->const_or_id = Token(TokenType::NUMBER, "", sign * const1.double_value, 0, 0);
            unaryOp = Token();
            unaryExpr = NULL;
            }
        }
    };

struct MultExpr
    {
    //optional
    MultExpr* mulExpr = NULL;
    Token op; // MULT or DIV
    
    //mandatory
    UnaryExpr* unaryExpr = NULL;
    
    MultExpr() = default;
    MultExpr(MultExpr* mulExpr, Token op) :mulExpr(mulExpr), op(op) {}
    Token getConstValue() 
        { 
        if (!mulExpr)
            return unaryExpr->getConstValue();

        Token const1 =  mulExpr->getConstValue();
        Token const2 = unaryExpr->getConstValue();
        if (const1.type != TokenType::NUMBER || const2.type != TokenType::NUMBER)
            return Token();
        
        double d;
        if (op.type == TokenType::MULTIPLY)
            d = const1.double_value * const2.double_value;
        else
            d = const1.double_value / const2.double_value;
        return Token(TokenType::NUMBER, "", d, op.line, op.pos);
        }
    void optimize()
        {
        unaryExpr->optimize();
        if (mulExpr)
            {
            mulExpr->optimize();
            Token const1 = mulExpr->getConstValue();
            Token const2 = unaryExpr->getConstValue();
            if (const1 != TokenType::NUMBER || const2 != TokenType::NUMBER)
                return;

            double d;
            if (op.type == TokenType::MULTIPLY)
                d = const1.double_value * const2.double_value;
            else
                d = const1.double_value / const2.double_value;
            mulExpr = NULL;
            op = Token();
            unaryExpr->primExpr->const_or_id = Token(TokenType::NUMBER, "", d, 0, 0);
            unaryExpr->primExpr->addExpr = NULL;
            }
        }
    };

struct AddExpr
    {
    // optional
    AddExpr* addExpr = NULL;
    Token op; // PLUS or MIN

    //mandatory
    MultExpr* mulExpr = NULL;
    AddExpr() = default;
    AddExpr(AddExpr* addExpr, Token op) :addExpr(addExpr), op(op) {}
    Token getConstValue()
        {
        if (!addExpr)
            return mulExpr->getConstValue();

        Token const1 = addExpr->getConstValue();
        Token const2 = mulExpr->getConstValue();
        if (const1 != TokenType::NUMBER || const2 != TokenType::NUMBER)
            return Token();

        double d;
        if (op == TokenType::PLUS)
            d = const1.double_value + const2.double_value;
        else
            d = const1.double_value - const2.double_value;
        return Token(TokenType::NUMBER, "", d, op.line, op.pos);
        }
    void optimize()
        {
        mulExpr->optimize();
        if (addExpr)
            {
            addExpr->optimize();
            Token const1 = addExpr->getConstValue();
            Token const2 = mulExpr->getConstValue();
            if (const1 != TokenType::NUMBER || const2 != TokenType::NUMBER)
                return;
            
            double d;
            if (op == TokenType::PLUS)
                d = const1.double_value + const2.double_value;
            else
                d = const1.double_value - const2.double_value;
            addExpr = NULL;
            op = Token();
            mulExpr->unaryExpr->primExpr->const_or_id = Token(TokenType::NUMBER, "", d, 0, 0);
            }
        }
    };

struct Block;
struct Assign
    {
    Token id;
    AddExpr* addExpr = NULL;
    std::string str;
    Block* block;
    void optimize() 
        { 
        if(addExpr)
            addExpr->optimize(); 
        }
    };

struct Statement
    {
    Assign* assignment = NULL;
    Block* block;
    void optimize();
    };


#pragma once
#include "..\parser\Token.h"

struct AddExpr;

struct Factor
    {
    Token const_or_id;
    //or
    AddExpr* rval = NULL;
    Token getConstValue();
    void optimize();
    };

struct MultExpr
    {
    //optional
    MultExpr* term = NULL;
    Token op; // MULT or DIV
    
    //mandatory
    Factor* factor = NULL;
    
    MultExpr() = default;
    MultExpr(MultExpr* term, Token op) :term(term), op(op) {}
    Token getConstValue() 
        { 
        if (!term)
            return factor->getConstValue();

        Token const1 =  term->getConstValue();
        Token const2 = factor->getConstValue();
        if (const1 != TokenType::NUMBER || const2 != TokenType::NUMBER)
            return Token();
        
        double d;
        if (op == TokenType::MULTIPLY)
            d = const1.double_value * const2.double_value;
        else
            d = const1.double_value / const2.double_value;
        return Token(TokenType::NUMBER, "", d, op.line, op.pos);
        }
    void optimize()
        {
        factor->optimize();
        if (term)
            {
            term->optimize();
            Token const1 = term->getConstValue();
            Token const2 = factor->getConstValue();
            if (const1 != TokenType::NUMBER || const2 != TokenType::NUMBER)
                return;

            double d;
            if (op == TokenType::MULTIPLY)
                d = const1.double_value * const2.double_value;
            else
                d = const1.double_value / const2.double_value;
            term = NULL;
            op = Token();
            factor->const_or_id = Token(TokenType::NUMBER, "", d, 0, 0);
            }
        }
    };

struct AddExpr
    {
    // optional
    AddExpr* rval = NULL;
    Token op; // PLUS or MIN

    //mandatory
    MultExpr* term = NULL;
    AddExpr() = default;
    AddExpr(AddExpr* rval, Token op) :rval(rval), op(op) {}
    Token getConstValue()
        {
        if (!rval)
            return term->getConstValue();

        Token const1 = rval->getConstValue();
        Token const2 = term->getConstValue();
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
        term->optimize();
        if (rval)
            {
            rval->optimize();
            Token const1 = rval->getConstValue();
            Token const2 = term->getConstValue();
            if (const1 != TokenType::NUMBER || const2 != TokenType::NUMBER)
                return;
            
            double d;
            if (op == TokenType::PLUS)
                d = const1.double_value + const2.double_value;
            else
                d = const1.double_value - const2.double_value;
            rval = NULL;
            op = Token();
            term->factor->const_or_id = Token(TokenType::NUMBER, "", d, 0, 0);
            }
        }
    };

struct Assign
    {
    Token id;
    AddExpr* rval = NULL;
    void optimize() { rval->optimize(); }
    };

struct Statement
    {
    Assign* assignment = NULL;
    //TODO: or Block* block;
    void optimize() { assignment->optimize(); }
    };
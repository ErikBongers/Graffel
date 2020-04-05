#include "nodes.h"
#include <iomanip>
#include <string>

const int indent = 2;

void printNumber(Token t, int lvl)
    {
    std::string number = t.str_value;
    if (t.str_value == "")
        number = std::to_string(t.double_value);
    std::cout << std::setw(lvl) << ' ' << "number: " << number << std::endl;
    }

void printId(Token t, int lvl)
    {
    std::cout << std::setw(lvl) << ' ' << "id: " << t.str_value << std::endl;
    }

void printOp(Token t, int lvl)
    {
    std::cout << std::setw(lvl) << ' ' << "op: " << t.type << std::endl;
    }

void printAddExpr(AddExpr& rval, int lvl);
void printFactor(Factor& factor, int lvl)
    {
    if (!&factor)
        {
        std::cout << std::setw(lvl) << ' ' << "factor: ???" << std::endl;
        return;
        }
    std::cout << std::setw(lvl) << ' ' << "factor: " << std::endl;
    if (factor.const_or_id == TokenType::ID)
        printId(factor.const_or_id, lvl + indent);
    else if (factor.const_or_id == TokenType::NUMBER)
        printNumber(factor.const_or_id, lvl + indent);
    else
        printAddExpr(*factor.rval, lvl+indent);
    }

void printMultExpr(MultExpr& term, int lvl)
    {
    if (!&term)
        {
        std::cout << std::setw(lvl) << ' ' << "multexpr: ???" << std::endl;
        return;
        }
    std::cout << std::setw(lvl) << ' ' << "multexpr: " << std::endl;
    if (term.op != TokenType::UNDEFINED)
        {
        printMultExpr(*term.term, lvl + indent);
        printOp(term.op, lvl + indent);
        }
    printFactor(*term.factor, lvl + indent);
    }

void printAddExpr(AddExpr& rval, int lvl)
    {
    if (!&rval)
        {
        std::cout << std::setw(lvl) << ' ' << "addexpr: ???" << std::endl;
        return;
        }
    std::cout << std::setw(lvl) << ' ' << "addexpr: " << std::endl;
    if (rval.op != TokenType::UNDEFINED)
        {
        printAddExpr(*rval.rval, lvl + indent);
        printOp(rval.op, lvl + indent);
        }
    printMultExpr(*rval.term, lvl + indent);
    }

void printAssign(Assign& assign, int lvl)
    {
    std::cout << std::setw(lvl) << ' ' << "assign: " << std::endl;
    printId(assign.id, lvl + indent);
    printAddExpr(*assign.rval, lvl + indent);
    }

void printStatement(Statement& statement, int lvl)
    {
    std::cout << std::setw(lvl) << ' ' << "statement: " << std::endl;
    //if(statement.block == NULL)
    printAssign(*statement.assignment, lvl + indent);
    }

void printProgram(Statement& statement)
    {
    printStatement(statement, 0);
    }

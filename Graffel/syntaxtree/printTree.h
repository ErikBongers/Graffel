#include "nodes.h"
#include <iomanip>
#include <string>

const std::streamsize indent = 2;
std::streamsize maxLvl = 999;

void printNumber(Token t, int lvl)
    {
    if (lvl > maxLvl)
        return;
    std::string number = t.str_value;
    if (t.str_value == "")
        number = std::to_string(t.double_value);
    std::cout << std::setw(lvl*indent) << ' ' << "number: " << number << std::endl;
    }

void printId(Token t, int lvl)
    {
    if (lvl > maxLvl)
        return;
    std::cout << std::setw(lvl*indent) << ' ' << "id: " << t.str_value << std::endl;
    }

void printOp(Token t, int lvl)
    {
    if (lvl > maxLvl)
        return;
    std::cout << std::setw(lvl*indent) << ' ' << "op: " << t.type << std::endl;
    }

void printAddExpr(AddExpr& addExpr, int lvl);

void printPrimary(PrimaryExpr& primary, int lvl)
    {
    if (lvl > maxLvl)
        return;
    if (!&primary)
        {
        std::cout << std::setw(lvl*indent) << ' ' << "primary: ???" << std::endl;
        return;
        }
    std::cout << std::setw(lvl*indent) << ' ' << "primary: " << std::endl;
    if (primary.const_or_id == TokenType::ID)
        printId(primary.const_or_id, ++lvl);
    else if (primary.const_or_id == TokenType::NUMBER)
        printNumber(primary.const_or_id, ++lvl);
    else
        printAddExpr(*primary.addExpr, ++lvl);
    }

void printUnary(UnaryExpr& unary, int lvl)
    {
    if (lvl > maxLvl)
        return;
    std::cout << std::setw(lvl*indent) << ' ' << "unary: " << std::endl;
    if (unary.primExpr)
        printPrimary(*unary.primExpr, ++lvl);
    else
        {
        printOp(unary.unaryOp, ++lvl);
        printUnary(*unary.unaryExpr, ++lvl);
        }
    }

void printMultExpr(MultExpr& mulExpr, int lvl)
    {
    if (lvl > maxLvl)
        return;
    if (!&mulExpr)
        {
        std::cout << std::setw(lvl*indent) << ' ' << "multexpr: ???" << std::endl;
        return;
        }
    std::cout << std::setw(lvl*indent) << ' ' << "multexpr: " << std::endl;
    if (mulExpr.op != TokenType::UNDEFINED)
        {
        printMultExpr(*mulExpr.mulExpr, ++lvl);
        printOp(mulExpr.op, ++lvl);
        }
    printUnary(*mulExpr.unaryExpr, ++lvl);
    }

void printAddExpr(AddExpr& addExpr, int lvl)
    {
    if (lvl > maxLvl)
        return;
    if (!&addExpr)
        {
        std::cout << std::setw(lvl*indent) << ' ' << "addexpr: ???" << std::endl;
        return;
        }
    std::cout << std::setw(lvl*indent) << ' ' << "addexpr: " << std::endl;
    if (addExpr.op != TokenType::UNDEFINED)
        {
        printAddExpr(*addExpr.addExpr, ++lvl);
        printOp(addExpr.op, ++lvl);
        }
    printMultExpr(*addExpr.mulExpr, ++lvl);
    }

void printStatement(Statement& statement, int lvl);
void printBlock(Block& block, int lvl)
    {
    std::cout << std::setw(lvl*indent) << ' ' << "block: " << std::endl;
    lvl++;
    for (auto it = std::begin(block.statements); it != std::end(block.statements); ++it) {
        printStatement(**it, lvl);
        }
    }

void printString(const std::string& str, int lvl)
    {
    std::cout << std::setw(lvl) << ' ' << "string: \"" << str << "\"" << std::endl;
    }

void printAssign(Assign& assign, int lvl)
    {
    if (lvl > maxLvl)
        return;
    std::cout << std::setw(lvl*indent) << ' ' << "assign: " << std::endl;
    printId(assign.id, ++lvl);
    if (assign.addExpr)
        printAddExpr(*assign.addExpr, ++lvl);
    else if (assign.block)
        printBlock(*assign.block, ++lvl);
    else if (!assign.str.empty())
        printString(assign.str, ++lvl);
    }

void printStatement(Statement& statement, int lvl)
    {
    if (lvl > maxLvl)
        return;
    std::cout << std::setw(lvl*indent) << ' ' << "statement: " << std::endl;
    if (statement.block)
        printBlock(*statement.block, ++lvl);
    else if(statement.assignment)
        printAssign(*statement.assignment, ++lvl);
    }

void printProgram(Statement& statement, int maxLevel)
    {
    maxLvl = maxLevel;
    printStatement(statement, 0);
    }

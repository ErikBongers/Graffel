#include "nodes.h"

Token PrimaryExpr::getConstValue()
    {
    if (addExpr)
        return addExpr->getConstValue();
    else
        return const_or_id == TokenType::NUMBER ? const_or_id : Token();
    }
void PrimaryExpr::optimize()
    {
    if (addExpr)
        {
        addExpr->optimize();
        Token const1 = addExpr->getConstValue();
        if (const1 == TokenType::NUMBER)
            {
            addExpr = NULL;
            const_or_id = const1;
            }
        }
    }

void Statement::optimize()
    {
    if (block)
        block->optimize();
    else
        assignment->optimize();
    }

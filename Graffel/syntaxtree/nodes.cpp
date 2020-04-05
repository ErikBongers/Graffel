#include "nodes.h"

Token Factor::getConstValue()
    {
    if (rval)
        return rval->getConstValue();
    else
        return const_or_id == TokenType::NUMBER ? const_or_id : Token();
    }
void Factor::optimize()
    {
    if (rval)
        {
        rval->optimize();
        Token const1 = rval->getConstValue();
        if (const1 == TokenType::NUMBER)
            {
            rval = NULL;
            const_or_id = const1;
            }
        }
    }

#include "Parser.h"

Parser::Parser(const char* stream)
    : tok(stream)
    {
    }
void Parser::parse()
    {
    while (tok.peek().type != TokenType::EOT)
        {
        tok.skipWhiteSpace();
        statementStartPos = tok.getPos();
        auto stmt = parseStatement();
        statements.push_back(stmt);
        }
    }

Statement* Parser::parseStatement()
    {
    auto stmt = createStatement();
    stmt->node = parseAssignExpr();
    if (stmt->node == nullptr)
        stmt->node = parseAddExpr();
    auto t = tok.peek();
    if (t.type == TokenType::SEMI_COLON)
        {
        tok.peekComments = true;//from here on, include comments in the next peek! Must be set BEFORE next(), as next() will already peek.
        tok.next(); //consume
        if(stmt->echo)
            {
            stmt->text = t.stringValue;
            if(stmt->text.starts_with("\r\n"))
                stmt->text.erase(0, 2);
            else if(stmt->text.starts_with("\n"))
                stmt->text.erase(0, 1);
            }
        }
    else if (t.type == TokenType::EOT)
        {
        //ignore
        }
    else
        {
        tok.next();
        stmt->error = Error(ErrorId::EXPECTED, Range(t), ";");
        }
    if(echoBlock)
        stmt->echo = true;
    return stmt;
    }

Node* Parser::parseAssignExpr()
    {
    auto t = tok.peek();
    if (t.type == TokenType::ID)
        {
        auto id = t;
        t = tok.peekSecond();
        if (t.type == TokenType::EQ)
            {
            tok.next();//consume ID
            tok.next();//consume EQ
            AssignExpr* assign = createAssign();
            assign->Id = id;
            auto list = parseListExpr();
            if(list.size() == 1)
                assign->expr = list[0];
            else
                {
                if (list.size() == 0)
                    assign->error = Error(ErrorId::UNKNOWN_EXPR, Range(t));

                auto listExpr = createList();
                listExpr->list = list;
                assign->expr = listExpr;
                }

            ids.emplace(assign->Id.stringValue, Variable{ assign->Id, assign->expr });
            return assign;
            }
        else if (t.type == TokenType::EQ_PLUS || t.type == TokenType::EQ_MIN || t.type == TokenType::EQ_MULT || t.type == TokenType::EQ_DIV || t.type == TokenType::EQ_UNIT)
            {
            tok.next(); //consume ID
            tok.next();//consume EQ
            AssignExpr* assign = createAssign();
            assign->Id = id;
            //prepare components to fabricate the new add or mult expression.
            IdExpr* idExpr = createIdExpr();
            idExpr->Id = id;
            TokenType oper;
            switch (t.type)
                {
                case TokenType::EQ_PLUS: oper = TokenType::PLUS; break;
                case TokenType::EQ_MIN: oper = TokenType::MIN; break;
                case TokenType::EQ_MULT: oper = TokenType::MULT; break;
                case TokenType::EQ_DIV: oper = TokenType::DIV; break;
                default:break;
                }

            //build expression
            if (t.type == TokenType::EQ_PLUS || t.type == TokenType::EQ_MIN || t.type == TokenType::EQ_MULT || t.type == TokenType::EQ_DIV)
                {
                BinaryOpExpr* binOpExpr = createBinaryOp();
                binOpExpr->n1 = idExpr;
                binOpExpr->op = Token(oper, tok.getLine(), tok.getLinePos());
                binOpExpr->n2 = parseAddExpr();
                assign->expr = binOpExpr;
                }
            else if (t.type == TokenType::EQ_UNIT)
                {
                PostfixExpr* pfix = createPostfix();
                pfix->expr = idExpr;
                assign->Id = id;
                assign->expr = pfix;
                auto t = tok.peek();
                if (t.type == TokenType::ID)
                    {
                    tok.next();
                    pfix->postfixId = t;
                    }
                else
                    { //valid syntax: clear the unit, if any.
                    pfix->postfixId = Token::Null();
                    }
                }
            return assign;
            }
        else
            {
            return nullptr;
            }
        }
    else
        {
        return nullptr;
        }
    }

Node* Parser::parseAddExpr()
    {
    Node* node = parseMultExpr();
    auto t = tok.peek();
    while (t.type == TokenType::PLUS || t.type == TokenType::MIN)
        {
        tok.next();
        BinaryOpExpr* addExpr = createBinaryOp();
        addExpr->n1 = node;
        addExpr->op = t;
        addExpr->n2 = parseMultExpr();
        node = addExpr;
        t = tok.peek();
        }
    return node;
    }

Node* Parser::parseMultExpr()
    {
    Node* node = parsePowerExpr();
    auto t = tok.peek();
    while (t.type == TokenType::MULT || t.type == TokenType::DIV)
        {
        tok.next();
        BinaryOpExpr* multExpr = createBinaryOp();
        multExpr->n1 = node;
        multExpr->op = t;
        multExpr->n2 = parsePowerExpr();
        node = multExpr;
        //give warning if expr of form a/2b:
        if (multExpr->op.type == TokenType::DIV)
            {
            if (multExpr->n2->is(NodeType::BINARYOPEXPR))
                {
                BinaryOpExpr* n2 = static_cast<BinaryOpExpr*>(multExpr->n2);
                if (n2->implicitMult)
                    {
                    multExpr->error = Error(ErrorId::W_DIV_IMPL_MULT, node->range());
                    }
                }
            }
        t = tok.peek();
        }
    return node;
    }

Node* Parser::parsePowerExpr()
    {
    Node* node = parseImplicitMult();
    auto t = tok.peek();
    while (t.type == TokenType::POWER)
        {
        tok.next();
        BinaryOpExpr* powerExpr = createBinaryOp();
        powerExpr->n1 = node;
        powerExpr->op = t;
        powerExpr->n2 = parsePowerExpr(); //right associative!
        node = powerExpr;
        if ((powerExpr->n1->is(NodeType::BINARYOPEXPR) && static_cast<BinaryOpExpr*>(powerExpr->n1)->implicitMult)
            || (powerExpr->n2->is(NodeType::BINARYOPEXPR) && static_cast<BinaryOpExpr*>(powerExpr->n2)->implicitMult))
            {
            powerExpr->error = Error(ErrorId::W_POW_IMPL_MULT, node->range());
            }
        t = tok.peek();
        }
    return node;
    }

Node* Parser::parseImplicitMult()
    {
    Node* n1 = parseUnaryExpr();
    auto t = tok.peek();
    while ((t.type == TokenType::ID
            || t.type == TokenType::NUMBER)
           || t.type == TokenType::PAR_OPEN)
        {
        //don't consume the token yet...
        auto m = createBinaryOp();
        m->n1 = n1;
        m->op = Token(TokenType::MULT, '*', tok.getLine(), tok.getLinePos());
        m->n2 = parsePostFixExpr();
        m->implicitMult = true;
        n1 = m;
        t = tok.peek();
        }
    return n1;
    }

Node* Parser::parseUnaryExpr()
    {
    auto t = tok.peek();
    if (t.type == TokenType::MIN)
        {
        tok.next();
        auto node = createUnaryOp();
        node->op = t;
        node->n = parsePostFixExpr();
        return node;
        }
    return parsePostFixExpr();
    }

Node* Parser::parsePostFixExpr()
    {
    Node* node = parsePrimaryExpr();
    auto t = tok.peek();
    while (t.type == TokenType::DOT || t.type == TokenType::INC || t.type == TokenType::DEC)
        {
        node = parseOnePostFix(node);
        t = tok.peek();
        }
    return node;
    }

Node* Parser::parseOnePostFix(Node* node)
    {
    auto t = tok.peek();
    if (t.type == TokenType::DOT)
        {
        tok.next();
        t = tok.peek();
        auto postfixExpr = createPostfix();
        postfixExpr->expr = node;
        node = postfixExpr;
        if (t.type == TokenType::ID)
            {
            tok.next();
            postfixExpr->postfixId = t;
            }
        else
            { //a dot followed by nothing is valid syntax: clear the unit, if any.
            postfixExpr->postfixId = Token::Null();
            }
        }
    else if (t.type == TokenType::INC || t.type == TokenType::DEC)
        {
        tok.next();
        if (node->type != NodeType::IDEXPR)
            {
            node->error = Error(ErrorId::VAR_EXPECTED, node->range());
            return node;
            }

        IdExpr* idExpr = (IdExpr*)node;
        CallExpr* callExpr = createCall();
        callExpr->arguments.push_back(node);
        callExpr->functionName = Token(TokenType::ID, (t.type == TokenType::INC ? "_ inc" : "_ dec"), tok.getLine(), tok.getLinePos());

        AssignExpr* assignExpr = createAssign();
        assignExpr->Id = idExpr->Id;
        assignExpr->expr = callExpr;
        node = assignExpr;
        }
    return node;
    }

Node* Parser::parsePrimaryExpr()
    {
    auto t = tok.peek();
    switch (t.type)
        {
        case TokenType::NUMBER:
            return parseNumber(tok.next(), false);
        case TokenType::MIN:
            tok.next();
            if (tok.peek().type == TokenType::NUMBER)
                {
                return parseNumber(tok.next(), true);
                }
            //else error?
            break;
        case TokenType::ID:
            {
            tok.next();
            IdExpr* idExpr = createIdExpr();
            idExpr->Id = t;
            return idExpr;
            }
        case TokenType::PAR_OPEN:
            {
            tok.next();
            auto addExpr = parseAddExpr();
            t = tok.peek();
            if (t.type == TokenType::PAR_CLOSE)
                tok.next();
            else
                {
                if(addExpr->error.id == ErrorId::NONE)
                    addExpr->error = Error(ErrorId::EXPECTED, addExpr->range(), ")");
                }
            return addExpr;
            }
        case TokenType::PIPE:
            tok.next();
            return parseAbsOperator();
        default: 
            break;
        }
    auto none = createNoneExpr();
    none->token = t;
    return none;
    }

Node* Parser::parseAbsOperator()
    {
    auto addExpr = parseAddExpr();
    CallExpr* callExpr = createCall();
    callExpr->arguments.push_back(addExpr);
    callExpr->functionName = Token(TokenType::ID, "abs", tok.getLine(), tok.getLinePos());
    auto t = tok.peek();
    if (t.type == TokenType::PIPE)
        tok.next();
    else
        {
        if(callExpr->error.id == ErrorId::NONE)
            callExpr->error = Error(ErrorId::EXPECTED, callExpr->range(), "|");
        }
    return callExpr;
    }

ConstExpr* Parser::parseNumber(Token currentToken, bool negative)
    {
    auto constExpr = createConst();
    currentToken.numberValue = (negative ? -1 : 1) * currentToken.numberValue;
    constExpr->value = currentToken;
    return constExpr;
    }

CallExpr* Parser::parseCallExpr(Token functionName)
    {
    CallExpr* callExpr = createCall();
    callExpr->functionName = functionName;
    auto t = tok.peek();
    if (t.type != TokenType::PAR_OPEN)
        {
        callExpr->error = Error(ErrorId::FUNC_NO_OPEN_PAR, Range(t), functionName.stringValue.c_str());
        return callExpr;
        }
    tok.next();
    callExpr->arguments = parseListExpr();
    t = tok.peek();
    if (t.type == TokenType::PAR_CLOSE)
        tok.next();
    else
        {
        if(callExpr->error.id == ErrorId::NONE)
            callExpr->error = Error(ErrorId::EXPECTED, callExpr->range(), ")");
        }
    return callExpr;
    }

std::vector<Node*> Parser::parseListExpr()
    {
    std::vector<Node*> list;
    while (true)
        {
        auto expr = parseAddExpr();
        if(expr->is(NodeType::NONE))
            break;
        list.push_back(expr);
        if (tok.peek().type != TokenType::COMMA)
            break;
        tok.next();
        }
    return list;
    }


Block* Parser::createBlockExpr() { Block* p = new Block(); nodes.push_back(p); return p; }
NoneExpr* Parser::createNoneExpr() { NoneExpr* p = new NoneExpr(); nodes.push_back(p); return p; }
ConstExpr* Parser::createConst() { ConstExpr* p = new ConstExpr(); nodes.push_back(p); return p; }
BinaryOpExpr* Parser::createBinaryOp() { BinaryOpExpr* p = new BinaryOpExpr; nodes.push_back(p); return p; }
UnaryOpExpr* Parser::createUnaryOp() { UnaryOpExpr* p = new UnaryOpExpr; nodes.push_back(p); return p; }
IdExpr* Parser::createIdExpr() { IdExpr* p = new IdExpr; nodes.push_back(p); return p; }
PostfixExpr* Parser::createPostfix() { PostfixExpr* p = new PostfixExpr; nodes.push_back(p); return p; }
AssignExpr* Parser::createAssign() { AssignExpr* p = new AssignExpr; nodes.push_back(p); return p; }
ListExpr* Parser::createList() { ListExpr* p = new ListExpr; nodes.push_back(p); return p; }
Define* Parser::createDefine() { Define* p = new Define; nodes.push_back(p); return p; }
Statement* Parser::createStatement() { Statement* p = new Statement; nodes.push_back(p); return p; }
CallExpr* Parser::createCall() { CallExpr* p = new CallExpr; nodes.push_back(p); return p; }

Range ConstExpr::range() const
    {
    return Range(value.line, value.pos, value.line, value.pos + (unsigned)value.stringValue.size());
    }

Range IdExpr::range() const
    {
    return {Id.line, Id.pos, Id.line, Id.pos + (unsigned)Id.stringValue.size()};
    }

Range PostfixExpr::range() const
    {
    Range r = {postfixId.line, postfixId.pos, postfixId.line, postfixId.pos + (unsigned)postfixId.stringValue.size()};
    if(expr != nullptr)
        r += expr->range();
    return r;
    }

Range CallExpr::range() const
    {
    Range r = Range(functionName);
    if(!arguments.empty())
        r += arguments.back()->range();
    return r;
    }

Range BinaryOpExpr::range() const
    {
    Range r = Range(n1->range());
    r += Range(n2->range());
    return r;
    }

Range UnaryOpExpr::range() const
    {
    Range r = Range(n->range());
    r += Range(op);
    return r;
    }

Range AssignExpr::range() const
    {
    Range r = Range(Id);
    if(expr != nullptr)
        r += Range(expr->range());
    return r;
    }

Range ListExpr::range() const
    {
    Range r = Range(list.front()->range());
    r += list.back()->range();
    return r;
    }

Range Define::range() const
    {
    return Range(def);
    }

Range Statement::range() const
    {
    if(node != nullptr)
        return node->range();
    return Range(comment_line);
    }

Range Block::range() const
    {
    if(statements.empty())
        return Range();
    Range r = statements[0]->range();
    for( auto&stmt: statements)
        r += stmt->range();
    return r;
    }

Block* Parser::parseBlock()
    {
    auto t = tok.peek();
    if (t.type == TokenType::BRAC_OPEN)
        tok.next();
    else
        return NULL; //TODO: error
    Block* block = new Block();
    Statement* stat = parseStatement();
    while (stat) 
        {
        block->statements.push_back(stat);
        stat = parseStatement();
        }
    t = tok.peek();

    if (t.type == TokenType::BRAC_CLOSE)
        tok.next();
    else
        block->error = Error(ErrorId::EXPECTED, t, "}");
    t= tok.peek();
    if (t.type == TokenType::SEMI_COLON)
        tok.next();
    return block;
    }

bool Parser::parseBlockComment()
    {
    auto t = tok.peek();
    if (t.type == TokenType::BLOCK_COMMENT_OPEN)
        tok.next();
    else
        return false;
    int braceCounter = 0;
    do
        {
        t = tok.peek();
        if (t.type == TokenType::BRAC_OPEN || t.type == TokenType::BLOCK_COMMENT_OPEN)
            {
            tok.next();
            braceCounter++;
            }
        else
            if (t.type == TokenType::BRAC_CLOSE)
                {
                tok.next();
                braceCounter--;
                }
        } 
    while (braceCounter >= 0);
    
    return true;
    }

#pragma once
#include "Range.h"
#include "Error.h"
#include "Tokenizer.h"
#include <vector>

enum class NodeType {CONSTEXPR, POSTFIXEXPR, IDEXPR, CALLEXPR, BINARYOPEXPR, UNARYOPEXPR, ASSIGNMENT, STATEMENT, DEFINE, LIST, NONE, BLOCK, KEYFRAME, TIMELINE};
class Parser;

class Node
    {
    protected:
        Node(NodeType type) : type(type) {}
    public:
        NodeType type;
        Error error;
        bool is(NodeType type) { return this->type == type; }
        virtual Range range() const = 0;
        virtual ~Node() {}
    private:
        Node() : type(NodeType::CONSTEXPR) {}
        friend class Parser;
    };

class NoneExpr : public Node
    {
    public:
        Token token;
        Range range() const { return Range(token); }
    private:
        NoneExpr() : Node(NodeType::NONE) {};
        friend class Parser;
    };

class ConstExpr : public Node
    {
    public:
        Token value;
        Range range() const;
    private:
        ConstExpr() : Node(NodeType::CONSTEXPR) {};
        friend class Parser;
    };

class IdExpr : public Node
    {
    public:
        Token Id;
        Range range() const;
    private:
        IdExpr() : Node(NodeType::IDEXPR) {}
        friend class Parser;
    };

class PostfixExpr : public Node
    {
    public:
        Node* expr = nullptr;
        Token postfixId;
        Range range() const;
    private:
        PostfixExpr() : Node(NodeType::POSTFIXEXPR) {}
        friend class Parser;
    };


class CallExpr : public Node
    {
    public:
        Token functionName;
        std::vector<Node*> arguments;
        Range range() const;
    private:
        CallExpr() : Node(NodeType::CALLEXPR) {}
        friend class Parser;
    };

class BinaryOpExpr : public Node
    {
    public:
        Node* n1 = nullptr;
        Token op;
        Node* n2 = nullptr;
        bool implicitMult = false;
        Range range() const;
    private:
        BinaryOpExpr() : Node(NodeType::BINARYOPEXPR) {}
        friend class Parser;
    };

class UnaryOpExpr : public Node
    {
    public:
        Token op;
        Node* n = nullptr;
        Range range() const;
    private:
        UnaryOpExpr() : Node(NodeType::UNARYOPEXPR) {}
        friend class Parser;
    };

class AssignExpr : public Node
    {
    public: 
        Token Id;
        Node* expr = nullptr;
        Range range() const;
    private:
        AssignExpr() : Node(NodeType::ASSIGNMENT) {}
        friend class Parser;
    };

class ListExpr : public Node
    {
    public: 
        std::vector<Node*> list;
        Range range() const;
    private:
        ListExpr() : Node(NodeType::LIST) {}
        friend class Parser;
    };

class Define : public Node
    {
    public:
        Token def;
        Range range() const;
    private:
        Define() : Node(NodeType::DEFINE) {}
        friend class Parser;
    };

class Statement : public Node
    {
    public:
        Node* node = nullptr;
        std::string text;
        Token comment_line;
        bool mute = false; //mute output
        bool echo = false; //echo statement
        Range range() const;
    private:
        Statement() : Node(NodeType::STATEMENT) {}
        friend class Parser;
    };

class Variable
    {
    public:
        Token name;
        Node* addExpr = nullptr;
    };

struct Block : public Node
    {
    public:
        std::vector<Statement*> statements;
        Range range() const;
    private:
        Block() : Node(NodeType::BLOCK) {}
        friend class Parser;
    };

struct Keyframe : public Node
    {
    public:
        Token value; //TODO: should be a timepoint or duration
        bool isTimepoint = false; //false = duration
        Token markerId; //markerNo is determined by index of list of keyframes.
        Range range() const;
    private:
        Keyframe() : Node(NodeType::KEYFRAME) {}
        friend class Parser;
    };

struct Timeline: public Node
    {
    public:
        std::vector<Keyframe*> keyframes;
        Range range() const;
    private:
        Timeline() : Node(NodeType::TIMELINE) {}
        friend class Parser;
    };

class Parser
    {
    private:
        Tokenizer tok;
        bool muteBlock = false;
        bool echoBlock = false;
        bool echoTrailingComment = false;
    public:
        std::map<std::string, Variable> ids;
        std::vector<Statement*> statements;

        Parser(const char* stream);
        ~Parser() { for (auto node : nodes) delete node; }
        void parse();
    private:
        Statement* parseStatement();
        Node* parseAssignExpr();
        Node* parseAddExpr();
        Node* parseMultExpr();
        Node* parsePowerExpr();
        Node* parseUnaryExpr();
        Node* parseImplicitMult();
        Node* parsePostFixExpr();
        Node* parseOnePostFix(Node* node);
        Node* parsePrimaryExpr();
        ConstExpr* parseNumber(Token currentToken, bool negative);
        CallExpr* parseCallExpr(Token functionName);
        std::vector<Node*> parseListExpr();
        Node* parseKeyframe();
    public: //TEST
        Timeline* parseTimeline();
    private:
        Block* parseBlock();
        bool parseBlockComment();

        Keyframe* createKeyframeExpr();
        Timeline* createTimelineExpr();
        Block* createBlockExpr();
        NoneExpr* createNoneExpr();
        ConstExpr* createConst();
        BinaryOpExpr* createBinaryOp();
        UnaryOpExpr* createUnaryOp();
        IdExpr* createIdExpr();
        PostfixExpr* createPostfix();
        AssignExpr* createAssign();
        ListExpr* createList();
        Define* createDefine();
        CallExpr* createCall();
        Statement* createStatement();
        std::vector<Node*> nodes;
        unsigned int statementStartPos = 0;

        Node* parseAbsOperator();
    };


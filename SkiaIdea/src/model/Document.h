#pragma once
#include "../pch.h"

namespace idea{

class Node
    {
    private:
        Node() {}
    public:
        int id = -1;
        std::string title;
        std::string dscr;
        std::string body;
        friend class Document;
    };

//class View
//    {
//    public:
//        enum class Type { INFINITE_CANVAS };
//        Type type;
//        enum class Location { LEFT, TOP, RIGHT, BOTTOM};
//    };
//
class Document
    {
    private:
        int idCounter = 0;
    std::map<int, Node*> nodes;
    public:
        Node* createNode() { 
            auto node = new Node(); 
            node->id = idCounter++;
            nodes.insert({ node->id, node });
            return node;
            }
        void deleteNode(Node& node) { nodes.erase(node.id); delete &node; }
    };
}
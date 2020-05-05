#pragma once
#include "../pch.h"
#include "json/json.h"
#include "controls/texteditor/EditorDoc.h"

namespace idea{

class Node
    {
    private:
        Node() {}
    public:
        int id = -1;
        SkEd::EditorDoc title;
        SkEd::EditorDoc dscr;
        SkEd::EditorDoc body;
        friend class Document;
        const json::Object to_json() const;
        Node(json::Object& object);
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
        void addNode(const Node& node);
    public:
        std::map<int, Node*> nodes;
        Node* createNode();
        void deleteNode(Node& node) { nodes.erase(node.id); delete &node; }
        Document(json::Object& object);
        Document() {}
    };
}

std::ostream& operator << (std::ostream& out, const idea::Document& doc);
std::ostream& operator << (std::ostream& out, const idea::Node& node);

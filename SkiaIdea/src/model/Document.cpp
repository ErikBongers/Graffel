#include "Document.h"

using namespace json;

std::ostream& operator<<(std::ostream& out, const idea::Document& doc)
    {
    std::vector<Value> nodes;
    for (auto n : doc.nodes)
        nodes.push_back(Value(n.second->to_json()));
    Object o;
    o.add("nodes", nodes);
    out << o;
    return out;
    }

std::ostream& operator<<(std::ostream& out, const idea::Node& node)
    {
    out << node.to_json();
    return out;
    }

const json::Object idea::Node::to_json() const
    {
    Object o;
    o.add("id", id);
    o.add("title", title.toString());
    o.add("dscr", dscr.toString());
    o.add("body", body.toString());
    return o;
    }

void idea::Document::addNode(const idea::Node& node)
    {
    Node* newNode = new Node(node);
    idCounter = std::max(idCounter, newNode->id+1);
    nodes.insert({ newNode->id, newNode });
    }

idea::Node* idea::Document::createNode() {
    auto node = new Node();
    node->id = idCounter++;
    nodes.insert({ node->id, node });
    return node;
    }

idea::Document::Document(json::Object& object)
    {
    auto nodes = object.members["nodes"];
    for (auto nodeValue : nodes.array)
        {
        Object o = nodeValue.object;
        addNode(idea::Node(o));
        }
    }

idea::Node::Node(json::Object& object)
    {
    id = (int)object.members[ "id" ].number;
    title = object.members[ "title" ].str;
    dscr = object.members["dscr"].str;
    body = object.members["body"].str;
    }


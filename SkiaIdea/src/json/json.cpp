#include "json.h"

using namespace json;
void Object::stream(std::ostream& out) const
    {
    out << '{' << std::endl;
    bool comma = false;
    for (auto obj : members)// use const auto& ?
        {
        if (comma)
            out << ',' << std::endl;
        else
            comma = true;
        out << obj.first << ':' << obj.second;
        }
    out << std::endl << '}' << std::endl;
    }

void Value::stream(std::ostream& out) const
    {
    if (object)
        out << *object;
    else if (array.size())
        {
        bool comma = false;
        out << '[' << std::endl;
        for (auto obj : array)
            {
            if (comma)
                out << ',' << std::endl;
            else
                comma = true;
            out << obj;
            }
        out << ']';
        }
    else if (number != DBL_MIN)
        out << number;
    else if (literal != Literal::NONE)
        {
        switch (literal)
            {
            case Literal::TRUE_: out << "true"; break;
            case Literal::FALSE_: out << "false"; break;
            case Literal::NULL_: out << "null"; break;
            }
        }
    else
        out << '"' << str << '"';
    }

void Object::add(std::string name, std::string value)
    {
    Value valueObject;
    valueObject.str = value;
    members.insert({ name, valueObject });
    }

void Object::add(std::string name, Value value)
    {
    members.insert({ name, value});
    }

std::ostream& operator << (std::ostream& out, const json::Value& value)
    {
    value.stream(out);
    return out;
    }

std::ostream& operator << (std::ostream& out, const json::Object& object)
    {
    object.stream(out);
    return out;
    }

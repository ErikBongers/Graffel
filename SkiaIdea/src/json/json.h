#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>

namespace json {

class Value;
class Object
    {
    public:
    std::unordered_map<std::string, Value> members;
    void add(std::string name, Value value);
    void stream(std::ostream& out) const;
    };

enum class Literal : int {NONE, TRUE_, FALSE_, NULL_};

class Value
    {
    public:
        enum class ValueType { OBJECT, ARRAY, STRING, NUMBER, LITERAL};
        ValueType type;
        //mutually exclusive values:
        const Object object;
        std::vector<Value> array;
        std::string str; 
        double number = DBL_MIN;
        Literal literal = Literal::NONE;

        void stream(std::ostream& out) const;
    public:
        Value(){}
        Value(double number) : number(number), type(ValueType::NUMBER) {}
        Value(const Object& o) : object(o), type(ValueType::OBJECT) {}
        Value(std::string str) : str(str), type(ValueType::STRING) {}
        Value(Literal literal) : literal(literal), type(ValueType::LITERAL) {}
        Value(std::vector < Value> values) : array(values), type(ValueType::ARRAY) {}
    };


}

std::ostream& operator << (std::ostream& out, const json::Value& value);
std::ostream& operator << (std::ostream& out, const json::Object& object);

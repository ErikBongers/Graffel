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
    void add(std::string name, std::string value);
    void add(std::string name, Value value);
    void stream(std::ostream& out) const;
    };

enum class Literal : int {NONE, TRUE_, FALSE_, NULL_};

class Value
    {
    public:
        //mutually exclusive values:
        Object* object = nullptr;
        std::vector<Value> array;
        std::string str; 
        double number = DBL_MIN;
        Literal literal = Literal::NONE;

        void stream(std::ostream& out) const;
        Value(){}
        Value(double number) : number(number) {}
    };


}

std::ostream& operator << (std::ostream& out, const json::Value& value);
std::ostream& operator << (std::ostream& out, const json::Object& object);

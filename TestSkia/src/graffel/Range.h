# pragma once
#include <list>
#include <string>

namespace graffel
{

template <typename T=double>
class Slice
    {
    public:
        struct Value {
            enum class Type { NORMAL, UNDEFINED, RELATIVE, WEIGHTED }; //compiled separately for every template implementation. Not really needed. Move out of Slice template? SliceValueType?
            T value;
            Type type;
            Value(T value, Type type = Type::NORMAL) : value(value), type(type) {}
            Value(Type type) : type(type) {}
            };
        Value begin;
        std::string beginMarker;
        Value end;
        std::string endMarker;
        int repeat = 0;
        Slice(T begin, T end) : begin(begin), end(end) {};
        Slice(Value begin, Value end) : begin(begin), end(end) {};
        Slice(T begin) : begin(begin), end(Value{ T(), Value::Type::UNDEFINED }) {};
        Slice(typename Value::Type undefined, T end) : begin(Value{ T(), Value::Type::UNDEFINED }), end(end) {};

        class Iterator
            {
            private:
                Slice<T>& slice;
                T current;
                T step;
            public:
                Iterator(Slice<T>& slice, T step) : slice(slice), step(step) { current = slice.begin.value; }
                Iterator operator++(int) { current = current + step; return *this; }
                T operator*() { return current; }
                bool hasNext() { return current <= slice.end.value;  }
            };

        Iterator iterator(T step) { return Iterator(*this, step); }
    };

template <typename T>
class Range
    {
    std::list<Slice<T>> slices;
    public:
        Range() {}
        Range(Slice<T> slice)
            {
            slices.push_back(slice);
            }

    };


}
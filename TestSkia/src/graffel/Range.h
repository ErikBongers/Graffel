# pragma once
#include <list>

namespace graffel
{

template <typename T>
class Slice
    {
    public:
        struct Value {
            enum class Type { NORMAL, UNDEFINED, RELATIVE, WEIGHTED};
            T value;
            Type type;
            Value(T value, Type type) : value(value), type(type) {}
            Value(Type type) : type(type) {}
            };
        Value begin;
        Value end;
        int repeat = 0;
        Slice(T begin, T end) : begin(Value{ begin, Value::Type::NORMAL }), end(Value{ end, Value::Type::NORMAL }) {};
        Slice(T begin) : begin(Value{ begin, Value::Type::NORMAL }), end(Value{ Value::Type::UNDEFINED }) {}; //using 'begin' as value for 'end' is a bit of a hack here...
        Slice(bool beginUndefined, T end) : begin(Value{ end, true, false, false }), end({ end, false, false, false}) {};
    
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
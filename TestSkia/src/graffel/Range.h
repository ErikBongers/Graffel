# pragma once
#include <vector>
#include <string>
#include <graffel/Error.h>

namespace graffel
{

enum class ValueType { NORMAL, UNDEFINED, RELATIVE, WEIGHTED }; //compiled separately for every template implementation. Not really needed. Move out of Slice template? SliceValueType?

template <typename T=double>
struct Value {
    T value;
    ValueType type;
    Value(T value, ValueType type = ValueType::NORMAL) : value(value), type(type) {}
    Value(ValueType type) : type(type) {}
    };

template <typename T = double>
class Slice
    {
    public:
        Value<T> begin;
        std::string beginMarker;
        Value<T> end;
        std::string endMarker;
        int repeat = 0;
        Slice(T begin, T end) : begin(begin), end(end) {};
        Slice(Value<T> begin, Value<T> end) : begin(begin), end(end) {};
        Slice(T begin) : begin(begin), end(Value<T>{ T(), ValueType::UNDEFINED }) {};
        Slice(typename ValueType undefined, T end) : begin(Value<T>{ T(), ValueType::UNDEFINED }), end(end) {};

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
    std::vector<Slice<T>> slices;
    public:
        Range() {}
        Range(Slice<T> slice)
            {
            slices.push_back(slice);
            }

        Range& operator+=(Slice<T> slice) { slices.push_back(slice); return *this; }
        Errors validate();
    };


template<typename T>
Errors Range<T>::validate()
    {
    Errors errors;

    for (long i = 0; i < slices.size()-1; i++)
        {
        Slice<T>& left = slices[i];
        Slice<T>& right = slices[i+1];
        //absolute end, absolute begin
        if (left.end.type == ValueType::NORMAL && right.begin.type == ValueType::NORMAL)
            {
            if (left.end.value != right.begin.value)
                errors.push_back(((std::string)"Slice ends at ") + std::to_string(left.end.value) + " and next slice begins at " + std::to_string(right.end.value));
            }
        }
    return errors;
    }

}
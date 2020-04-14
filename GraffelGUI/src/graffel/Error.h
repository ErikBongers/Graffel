#pragma once
#include <string>
#include <vector>

namespace graffel {

class Error
    {
    public:
        std::string filename;
        int line = 0, pos = 0;
        std::string message;
        Error(const std::string& message) : message(message) {};
        Error(const char* message) : message(message) {};
        operator std::string() const { return message; } //TODO: add other data.
    };

typedef std::vector<Error> Errors;

}

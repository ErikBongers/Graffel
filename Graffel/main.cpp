#include <iostream>
#include <fstream>
#include <string>
#include "TokenType.h"
#include "Token.h"
#include "Tokenizer.h"
#include <graffel\Range.h>

std::string path = R"(D:\Documents\Programming\CppProjects\Graffel\Graffel\test.graffel)";
int parseGraffel(const std::string& filename);
void testRange();
void testSlices();

namespace gf = graffel;

int main(int argc, char* argv[])
    {
    //return parseGraffel(path);
    testRange();
    }

void testRange()
    {
    // [1..10]
    gf::Range range(gf::Slice(1.3, 10.3));
    // [10..+10]
    gf::Range range2(gf::Slice(gf::Slice<int>::Value(10), gf::Slice<int>::Value(10, gf::Slice<int>::Value::Type::RELATIVE)));
    //...
    }

void testSlices()
    {
    gf::Slice<int> beginner(1); // test of default value for 'end'
    gf::Slice<int> ender(gf::Slice<int>::Value::Type::UNDEFINED, 1); // test of default value for 'end'
    gf::Slice<double> slice(1.0, 5.0);
    for (auto it = slice.iterator(0.3); it.hasNext(); it++)
        {
        std::cout << *it << std::endl;
        }
    }


















int parseGraffel(const std::string& filename)
    {
    try {
        std::ifstream ifs(filename);

        Tokenizer tok(ifs);
        Token t = tok.next();
        while (t.type != TokenType::EOT)
            {
            std::cout << t << std::endl;
            t = tok.next();
            }

        ifs.close();
        return 0;
        }
    catch (const char* msg)
        {
        std::cerr << "ERROR: " << msg << std::endl;
        return -1;
        }

    }
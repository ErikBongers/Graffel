#include <iostream>
#include <fstream>
#include <string>
#include "parser/TokenType.h"
#include "parser/Token.h"
#include "parser/Tokenizer.h"
#include "parser/Parser.h"

std::string path = R"(D:\Documents\Programming\CppProjects\Graffel\Graffel\test.graffel)";
int parseTokens(const std::string& filename);
int parseGraffel(const std::string& filename);
void testRange();
void testSlices();

namespace gf = graffel;

int main(int argc, char* argv[])
    {
    //return parseTokens(path);
    //testRange();
    parseGraffel(path);
    }

void testRange()
    {
    gf::Slice oneTen(1, 11);
    gf::Slice<int> tenPlusTen(gf::Value(10), gf::Value(10, gf::ValueType::RELATIVE));
    // [1..10]
    gf::Range range(oneTen);
    // [10..+10]
    gf::Range range2(tenPlusTen);
    //...
    // [1..10, 10..+10]
    range += tenPlusTen;
    auto errors = range.validate();
    for(auto const &error : errors)
        std::cout << (std::string)error;
    }

void testSlices()
    {
    gf::Slice<int> beginner(1); // test of default value for 'end'
    gf::Slice<int> ender(gf::ValueType::UNDEFINED, 1);
    gf::Slice<double> slice(1.0, 5.0);
    for (auto it = slice.iterator(0.3); it.hasNext(); it++)
        {
        std::cout << *it << std::endl;
        }
    }





int parseTokens(const std::string& filename)
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

int parseGraffel(const std::string& filename)
    {
    try {
        std::ifstream ifs(filename);

        Tokenizer tok(ifs);

        Parser parser(tok);

        parser.parse();

        ifs.close();
        return 0;
        }
    catch (const char* msg)
        {
        std::cerr << "ERROR: " << msg << std::endl;
        return -1;
        }
    }


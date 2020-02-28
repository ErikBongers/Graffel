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

int main(int argc, char* argv[])
    {
    //return parseGraffel(path);
    testRange();
    }

void testRange()
    {
    //graffel::Range<int> range;
    graffel::Slice<double> slice(1.0, 5.0);
    //graffel::Slice<int>::Iterator it = slice.iterator(1);
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
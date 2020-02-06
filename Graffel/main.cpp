#include <iostream>
#include <fstream>
#include <string>
#include "TokenType.h"
#include "Token.h"
#include "Tokenizer.h"

std::string path = R"(D:\Documents\Programming\CppProjects\Graffel\Graffel\test.graffel)";

int main(int argc, char* argv[])
    {
    try {
        std::ifstream ifs(path);

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
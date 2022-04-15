#include <iostream>
#include <fstream>
#include <string>
#include "parser/Token.h"
#include "parser/Tokenizer.h"
#include "parser/Parser.h"
#include <sstream>

std::string path = R"(D:\Documents\Programming\CppProjects\Graffel\Graffel\test.graffel)";
int parseGraffel(const std::string& filename);

int main(int argc, char* argv[])
    {
    parseGraffel(path);
    }

int parseGraffel(const std::string& filename)
    {
    std::ifstream ifs(filename);

    std::stringstream buffer;
    buffer << ifs.rdbuf();

    Parser parser(buffer.str().c_str());

    parser.parse();

    ifs.close();
    return 0;
    }


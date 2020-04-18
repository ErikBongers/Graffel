#include "Tokenizer.h"
#include <string>

using namespace json;

Tokenizer::Tokenizer(std::istream& is) 
    :is(is) 
    {
    keywords.insert({ "true", TokenType::TRUE_ });
    keywords.insert({ "false", TokenType::FALSE_ });
    keywords.insert({ "null", TokenType::NULL_ });
    }

bool Tokenizer::nextTokenChar()
    {
    while (getChar())
        {
        if (!isspace(this->curr_c))
            return true;
        }
    this->curr_c = 0;
    return false;
    }

Token Tokenizer::next()
    {
    while (nextTokenChar())
        {
        if (isdigit(curr_c))
            return parseNumber();
        else if (isalpha(curr_c))
            return parseAlpha();
        else if (curr_c == '"')
            return parseString();
        else if (curr_c == '{')
            return Token(TokenType::BRAC_OPEN, curr_line, curr_pos);
        else if (curr_c == '}')
            return Token(TokenType::BRAC_CLOSE, curr_line, curr_pos);
        else if (curr_c == '[')
            return Token(TokenType::SQR_OPEN, curr_line, curr_pos);
        else if (curr_c == ']')
            return Token(TokenType::SQR_CLOSE, curr_line, curr_pos);
        else if (curr_c == '-')
             return Token(TokenType::MINUS, curr_line, curr_pos);
        else if (curr_c == ':')
             return Token(TokenType::COLON, curr_line, curr_pos);
        else if (curr_c == ',')
             return Token(TokenType::COMMA, curr_line, curr_pos);
        else
            return Token(TokenType::UNKNOWN, curr_line, curr_pos);

        }
    return Token(TokenType::EOT, curr_line, curr_pos);
    }

Token Tokenizer::parseAlpha()
    {
    long line = this->curr_line;
    long pos = this->curr_pos;
    std::string alpha;
    alpha.push_back(curr_c);
    while (getChar())
        {
        if (isalpha(curr_c) || curr_c == '_')
            alpha.push_back(curr_c);
        else
            break;
        }
    putBack();
    auto found = keywords.find(alpha);
    if (found != keywords.end())
        return Token(found->second, alpha, line, pos);
    else
        return Token::UNDEFINED;
    }

Token Tokenizer::parseNumber()
    {
    long line = this->curr_line;
    long pos = this->curr_pos;
    TokenType type = TokenType::NUMBER;
    std::string numstr;
    bool decimalAlreadySet = false;
    numstr.push_back(curr_c);
    while (getChar())
        {
        if (isdigit(curr_c))
            {
            numstr.push_back(curr_c);
            }
        else if (curr_c == '.')
            {
            if (decimalAlreadySet)
                {
                putBack();
                break;
                }
            getChar();
            if (curr_c == '.') // range operator ..
                {
                putBack();
                break;
                }
            else
                {
                numstr.push_back('.');
                decimalAlreadySet = true;
                putBack();
                }
            }
        else
            {
            putBack();
            break;
            }
        }
    return Token(type, numstr, std::stod(numstr), line, pos);
    }

Token Tokenizer::parseString()
    {
    long line = this->curr_line;
    long pos = this->curr_pos;
    std::string str;
    while (getChar())
        {
        if (curr_c == '"')
            {
            return Token(TokenType::STRING, str, line, pos);
            }
        else if (curr_c == '\\')
            {
            str.push_back(curr_c);
            getChar();
            str.push_back(curr_c);
            }
        else
            str.push_back(curr_c);
        }
    throw "Encountered end of file in a string";
    }

void Tokenizer::putBack()
    {
    if (back_c != -1)
        throw "Cannot put back more than one char.";
    back_c = curr_c;
    back_line = curr_line;
    back_pos = curr_pos;
    }

bool Tokenizer::getChar()
    {
    if (back_c != -1)
        {
        curr_c = back_c;
        curr_line = back_line;
        curr_pos = back_pos;
        back_c = -1;
        return true;
        }
    else
        {
        back_line = curr_line;
        back_pos = curr_pos;
        bool result = (bool)is.get(curr_c);
        this->curr_pos++;
        if (this->curr_c == '\n')
            {
            this->curr_line++;
            this->curr_pos = 0;
            }
        return result;
        }

    }

Token Token::UNDEFINED = Token(TokenType::UNDEFINED, -1, -1);

#ifdef _DEBUG

std::ostream& operator << (std::ostream& os, const json::TokenType& tokenType)
    {
    std::string tokenTypeName;

    switch (tokenType)
        {
        case TokenType::BRAC_OPEN: tokenTypeName = "{"; break;
        case TokenType::BRAC_CLOSE: tokenTypeName = "}"; break;
        case TokenType::NUMBER: tokenTypeName = "NUMBER"; break;
        case TokenType::STRING: tokenTypeName = "STRING"; break;
        case TokenType::SQR_OPEN: tokenTypeName = "["; break;
        case TokenType::SQR_CLOSE: tokenTypeName = "]"; break;
        case TokenType::MINUS: tokenTypeName = "-"; break;
        case TokenType::COLON: tokenTypeName = ":"; break;
        default: tokenTypeName = "???"; break;
        }
    os << tokenTypeName;
    return os;
    }

std::ostream& operator << (std::ostream& os, const Token& token)
    {
    os << "[" << token.line << "," << token.pos << "]" << token.type << ": " << token.str_value;
    return os;
    }

#endif // DEBUG


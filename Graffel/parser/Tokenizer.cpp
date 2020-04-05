#include "Tokenizer.h"
#include "..\defines.h"
#include "Token.h"

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
        if (isalpha(curr_c) || curr_c == '_')
            return parseIdentifier();
        else if (isdigit(curr_c))
            return parseNumber();
        else if (curr_c == '"')
            return parseString();
        else if (curr_c == ';')
            return Token(TokenType::SEMICOLON, curr_line, curr_pos);
        else if (curr_c == '(')
            return Token(TokenType::PAR_OPEN, curr_line, curr_pos);
        else if (curr_c == ')')
            return Token(TokenType::PAR_CLOSE, curr_line, curr_pos);
        else if (curr_c == '{')
            return Token(TokenType::BRAC_OPEN, curr_line, curr_pos);
        else if (curr_c == '}')
            return Token(TokenType::BRAC_CLOSE, curr_line, curr_pos);
        else if (curr_c == '[')
            return Token(TokenType::SQR_OPEN, curr_line, curr_pos);
        else if (curr_c == ']')
            return Token(TokenType::SQR_CLOSE, curr_line, curr_pos);
        else if (curr_c == '=')
            return parseEquals();
        else if (curr_c == '+')
            return Token(TokenType::PLUS, curr_line, curr_pos);
        else if (curr_c == '-')
            return Token(TokenType::MINUS, curr_line, curr_pos);
        else if (curr_c == '*')
            return Token(TokenType::MULTIPLY, curr_line, curr_pos);
        else if (curr_c == '.')
            {
            getChar();
            if (curr_c == '.')
                {
                return Token(TokenType::SLICE, curr_line, curr_pos);
                }
            else
                {
                putBack();
                return Token(TokenType::DOT, curr_line, curr_pos);
                }
            }
        else if (curr_c == '/')
            {
            if (getChar())
                {
                if (curr_c == '/')
                    parseLineComment();
                else if (curr_c == '*')
                    parseComment();
                else if (curr_c == '{')
                    return Token(TokenType::BLOCK_COMMENT_OPEN, curr_line, curr_pos);
                else
                    {
                    putBack();
                    return Token(TokenType::DIVIDE, curr_line, curr_pos);
                    }
                }
            else
                {
                return Token(TokenType::DIVIDE, curr_line, curr_pos);
                }
            }
        else
            return Token(TokenType::UNKNOWN, curr_line, curr_pos);

        }
    return Token(TokenType::EOT, curr_line, curr_pos);
    }

Token Tokenizer::parseEquals()
    {
    long line = curr_line;
    long pos = curr_pos;
    if (getChar())
        {
        if (curr_c == '=') // 2nd =
            {
            if (getChar())
                {
                if (curr_c == '=') // 3rd =
                    {
                    return Token(TokenType::TRIPLE_EQ, line, pos);
                    }
                else
                    {
                    putBack();
                    return Token(TokenType::EQ, line, pos);
                    }
                }
            else // no next token.
                {
                return Token(TokenType::EQ, line, pos);
                }
            }
        else // no 2nd =
            {
            putBack();
            return Token(TokenType::ASSIGN, line, pos);
            }
        }
    else //n o next token.
        {
        return Token(TokenType::ASSIGN, line, pos);
        }
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
        else if (curr_c == 'd')
            {
            type = TokenType::DEGREES;
            break;
            }
        else if (curr_c == 'r')
            {
            type = TokenType::RADIANS;
            break;
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
        else
            str.push_back(curr_c);
        }
    throw "Encountered end of file in a string";
    }

void Tokenizer::parseLineComment()
    {
    while (getChar())
        {
        if (curr_c == '\n')
            return;
        }
    return;
    }

void Tokenizer::parseComment()
    {
    while (getChar())
        {
        if (curr_c == '*')
            {
            if (!getChar())
                break;
            if (curr_c == '/')
                return;
            }
        }
    throw "Encounter enf of file while parsing a block comment /*...*/.";
    }

void Tokenizer::parseBlockComment()
    {
    }

Token Tokenizer::parseIdentifier()
    {
    long line = this->curr_line;
    long pos = this->curr_pos;
    std::string id;
    id.reserve(MAX_ID_LENGTH);
    id.push_back(curr_c);
    while (getChar())
        {
        if (isalpha(curr_c) || curr_c == '_')
            {
            id.push_back(curr_c);
            }
        else
            {
            putBack();
            return Token(TokenType::ID, id, line, pos);
            }
        }
    return Token(TokenType::ID, id, line, pos);
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

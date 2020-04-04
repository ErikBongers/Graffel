#pragma once

#include <iostream>

enum class TokenType : int
	{
	UNDEFINED,
	//operators
	PLUS, MINUS, MULTIPLY, DIVIDE,
	ARROW, // ->
	FAT_ARROW, // =>
	ASSIGN, // =
	EQ, // ==
	NEQ,
	GT, LT, GE, LE,
	SLICE, // ..
	TRIPLE_EQ, // === just for test...
	DOT,


	ID, //identifier
	PAR_OPEN, // (
	PAR_CLOSE, // )
	BRAC_OPEN, // {
	BRAC_CLOSE, // }
	SQR_OPEN, // [
	SQR_CLOSE, // ]
	COMMA, // ,
	COLON, // :
	SEMICOLON, // ;
	BLOCK_COMMENT_OPEN, // /{
	NUMBER,
	STRING,
	DEGREES,
	RADIANS,









	EOT // end of tokens 
	};

#ifdef _DEBUG
std::ostream& operator << (std::ostream& os, const TokenType& tokenType);
#endif // DEBUG


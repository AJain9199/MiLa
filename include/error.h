#ifndef MILA_ERROR_H
#define MILA_ERROR_H

#include <string>
#include <lexer.h>

#define LEX 1
#define PARSE 2

void err(const Lexer &lex, const std::string &msg, int code);

#endif // MILA_ERROR_H

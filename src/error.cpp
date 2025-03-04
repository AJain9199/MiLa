#include <error.h>
#include <iostream>

void err(const Lexer &lex, const std::string &msg, int code) {
    std::cerr << lex.filename() << ": " << lex.line() << ": " << msg << std::endl;
    exit(1);
}
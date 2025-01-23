#include <iostream>
#include <lexer.h>

using namespace std;

int main() {
    Lexer lex("tst.micro");

    Lexer::TokenType e;
    while ((e = lex.getToken()) != Lexer::END) {
        cout << e << '\n';
    }
    return 0;
}

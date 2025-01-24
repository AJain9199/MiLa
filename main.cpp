#include <iostream>
#include <lexer.h>
#include <parse.h>

using namespace std;

int main() {
    Parser parse( "MOV.micro");

    parse.parseCode();
    parse.resolve();

    return 0;
}

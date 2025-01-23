#include <iostream>
#include <lexer.h>
#include <parse.h>

using namespace std;

int main() {
    Parser parse( "tst.micro");

    parse.parseCode();

    return 0;
}

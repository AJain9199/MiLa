#include <iostream>
#include <lexer.h>
#include <parse.h>
#include <codegen.h>

using namespace std;

int main() {
    Parser parse( "MOV.micro");

    parse.parseCode();
    parse.resolve();

    v3HexAddressed("file.out", parse.ins_width, parse.ctrl_word_width, parse.output, parse.default_value);

    return 0;
}

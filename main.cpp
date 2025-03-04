#include <iostream>
#include <lexer.h>
#include <parse.h>
#include <codegen.h>

using namespace std;

int main(const int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: MiLa <file>.micro" << endl;
        return 0;
    }

    Parser parse( argv[1]);

    parse.parseCode();
    parse.resolve();

    string fname = "file.out";
    if (argc > 2) {
        fname = argv[2];
    }

    v3HexAddressed(fname, parse.ins_width, parse.ctrl_word_width, parse.output, parse.default_value);

    return 0;
}

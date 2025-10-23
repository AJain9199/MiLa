#include <iostream>
#include <lexer.h>
#include <parse.h>
#include <codegen.h>

using namespace std;

int main(const int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: MiLa <input file>.micro <output file>.bin" << endl;
        return 0;
    }

    Parser parse( argv[1]);

    parse.parseCode();
    parse.resolve();

    cout << "Clock bit-width: " << parse.counter_width << endl;
    cout << "Instruction width: " << parse.ins_width << endl;
    cout << "Control word width: " << parse.ctrl_word_width << endl;

    string fname = "file.out";
    if (argc > 2) {
        fname = argv[2];
    }

    if (argc > 3 && string(argv[3]) == "--raw") {
        v2Raw(fname, parse.ctrl_word_width, parse.output, parse.default_value);
    } else {
        v3HexAddressed(fname, parse.ins_width, parse.ctrl_word_width, parse.output, parse.default_value);
    }

    return 0;
}

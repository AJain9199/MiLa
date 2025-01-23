#include <parse.h>
#include <error.h>
#include <string>
#include <format>

#define perr(msg) err(msg, PARSE)

using namespace std;

void Parser::parseAssignmentDecl() {
    string name = eat_id();
    bool bit_def = false;

    if (lexer == ':') {
        bit_def = true;
    }

    eat('=');

    if (lexer.getToken() == Lexer::PUNCTUATION) {
        if (lexer.punc() != '[') {
            perr(format("Expected numeric literal, macro, or expansion list. Found '{}' instead.", lexer.punc()));
        }

        if (bit_def) {
            perr("Cannot have bit-definition with an expansion list.");
        }

        num_list val = parseNumList();
        eat(';');

        expansion_list_symtab[depth][name] = val;
    } else {
        macro_symtab[depth][name] = parseExpr(bit_def);
    }
}

void Parser::parseCtrlWordDecl() {
    eat(Lexer::CTRL);

    eat('{');
    int pos = 0;
    while (lexer != '}') {
        string name = eat_id();

        int width = 1;

        if (lexer == ':') {
            eat(':');

            width = eat_num();
            bitset_tab[name] = make_pair(pos, pos+width-1);
        } else {
            macro_symtab[0][name] = 1 << pos;
        }
        eat(',');

        pos += width;
    }
    eat('}');
}

NUM_TYPE Parser::parseExpr(bool bit_def) {
    NUM_TYPE val;
    if (lexer.getToken() == Lexer::NUMERIC_LITERAL) {
        val = lexer.num();
    } else if (lexer.getToken() == Lexer::IDENTIFIER) {
        val = resolve_macro(lexer.id());
    } else{
        perr("Unexpected lvalue for assignment.");
    }
    return (bit_def?1<<val:val);
}




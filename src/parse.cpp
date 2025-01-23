#include <parse.h>
#include <error.h>
#include <string>
#include <format>

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
            err(format("Expected numeric literal, macro, or expansion list. Found '{}' instead.", lexer.punc()), PARSE);
        }

        if (bit_def) {
            err("Cannot have bit-definition with an expansion list.", PARSE);
        }

        num_list val = parseNumList();
        eat(';');

        expansion_list_symtab[depth][name] = val;
    } else if (lexer.getToken() == Lexer::NUMERIC_LITERAL) {
        macro_symtab[depth][name] = (bit_def ? 1 << lexer.num() : lexer.num());
    } else if (lexer.getToken() == Lexer::IDENTIFIER) {
        NUM_TYPE val = resolve_macro(lexer.id());
        macro_symtab[depth][name] = (bit_def?1<<val:val);
    }
}


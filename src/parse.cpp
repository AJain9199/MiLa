#include <parse.h>
#include <error.h>
#include <string>
#include <format>

using namespace std;

void Parser::parseAssignmentDecl() {
    string name = eat_id();
    eat('=');

    if (lexer.getToken() == Lexer::PUNCTUATION) {
        if (lexer.punc() != '[') {
            err(format("Expected numeric literal, macro, or expansion list. Found '{}' instead.", lexer.punc()), PARSE);
        }

        num_list val = parseNumList();
        eat(';');

        expansion_list_symtab[depth][name] = val;
    } else if (lexer.getToken() == Lexer::NUMERIC_LITERAL) {
        macro_symtab[depth][name] = lexer.num();
    } else if (lexer.getToken() == Lexer::IDENTIFIER) {
        macro_symtab[depth][name] = resolve_macro(lexer.id());
    }
}
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
            bitset_tab[name] = make_pair(pos, pos + width - 1);
        } else {
            macro_symtab[0][name] = 1 << pos;
        }

        pos += width;

        if (lexer == ',') {
            eat(',');
        } else {
            eat('}');
            break;
        }
    }
}

NUM_TYPE Parser::parseExpr(bool bit_def) {
    NUM_TYPE val;
    enum Lexer::TokenType t = lexer.getToken();
    if (t == Lexer::NUMERIC_LITERAL) {
        val = lexer.num();
    } else if (t == Lexer::IDENTIFIER) {
        val = resolve_macro(lexer.id());
    } else{
        perr("Unexpected lvalue for assignment.");
    }
    return (bit_def?1<<val:val);
}

Parser::Parser(const std::string &filename) : lexer(filename) {
}

num_list Parser::parseNumList() {
    num_list val;

    eat('[');
    while (lexer != ']') {
        val.push_back(eat_num());
        if (lexer == ',') {
            eat(',');
        } else {
            eat(']');
            break;
        }
    }
    return val;
}

NUM_TYPE Parser::resolve_macro(const string &name) {
    for (int i = depth; i >= 0; i--) {
        if (macro_symtab[i].find(name) != macro_symtab[i].end()) {
            return macro_symtab[i][name];
        }
    }
    perr(format("No value found for macro \"{}\" in current context", name));
    return 0;
}

void Parser::eat(char c) {
    if (lexer.current_token() == Lexer::PUNCTUATION && lexer.punc() == c) {
        lexer.getToken();
    } else {
        perr(format("Expected '{}'", c));
    }
}

std::string Parser::eat_id() {
    if (lexer.current_token() == Lexer::IDENTIFIER) {
        string s = lexer.id();
        lexer.getToken();
        return s;
    } else {
        perr(format("Expected valid identifier."));
        return "";
    }
}

void Parser::eat(Lexer::TokenType t) {
    if (lexer.current_token() == t) {
        lexer.getToken();
    } else {
        perr("Unexpected token type");
    }
}

void Parser::eat(Lexer::Keyword k) {
    if (lexer.current_token() == Lexer::KEYWORD && lexer.key() == k) {
        lexer.getToken();
    } else {
        perr("Expected keyword.");
    }
}

NUM_TYPE Parser::eat_num() {
    if (lexer.current_token() == Lexer::NUMERIC_LITERAL) {
        NUM_TYPE v = lexer.num();
        lexer.getToken();
        return v;
    } else {
        perr("Expected numeric literal.");
        return 0;
    }
}

void Parser::parseCode() {
    init_var_depth(0);
    while (lexer.getToken() != Lexer::END) {
        if (lexer == Lexer::CTRL) {
            parseCtrlWordDecl();
        }
    }
}

void Parser::init_var_depth(int d) {
    if (macro_symtab.size() > d) {
        macro_symtab[d] = {};
    }
    macro_symtab.push_back({});

    if (expansion_list_symtab.size() > d) {
        expansion_list_symtab[d] = {};
    }
    expansion_list_symtab.emplace_back();
}




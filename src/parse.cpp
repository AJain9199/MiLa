#include <parse.h>
#include <error.h>
#include <string>
#include <format>
#include <stack>

#define perr(msg) err(msg, PARSE)

using namespace std;

template<typename T>
void
cartesian_recurse(vector<vector<T>> &accum, vector<T> frontier, vector<T> indices, typename vector<T>::size_type idx) {
    int u = indices[idx];
    for (int i = 0; i < u; i++) {
        frontier.push_back(i);
        if (idx == 0) {
            accum.push_back(frontier);
        } else {
            cartesian_recurse(accum, frontier, indices, idx - 1);
        }
        frontier.pop_back();
    }
}


/*
 * Returns a cartesian product for indices, given the ending indices.
 * E.g. With input, [2, 3, 1],
 * The output is a vector of [0, 0, 0], [0, 1, 0], [0, 2, 0], [1, 0, 0]...
 * */
template<typename T>
vector<vector<int>> cartesian_product(const vector<T> &indices) {
    vector<vector<int>> accum;
    vector<int> frontier;

    if (!indices.empty()) {
        cartesian_recurse(accum, frontier, indices, indices.size() - 1);
    }
    return accum;
}

void Parser::parseAssignmentDecl() {
    string name = eat_id();
    bool bit_def = false;

    if (lexer == ':') {
        bit_def = true;
    }

    eat('=');

    if (lexer == Lexer::PUNCTUATION) {
        if (lexer.punc() != '[') {
            perr(format("Expected numeric literal, macro, or expansion list. Found '{}' instead.", lexer.punc()));
        }

        if (bit_def) {
            perr("Cannot have bit-definition with an expansion list.");
        }

        num_list val = parseNumList();
        // calculate maximum bit width
        expansion_list_symtab[depth][name] = {val, bit_width(*max_element(val.begin(), val.end()))};
    } else {
        macro_symtab[depth][name] = parseExpr(bit_def);
    }
    eat(';');
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
    if (lexer.current_token() == Lexer::NUMERIC_LITERAL) {
        val = lexer.num();
        lexer.getToken();
    } else if (lexer.current_token() == Lexer::IDENTIFIER) {
        val = resolve_macro(lexer.id());
        lexer.getToken();
    } else {
        perr("Unexpected lvalue for assignment.");
    }
    return (bit_def ? 1 << val : val);
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
    lexer.getToken();
    while (lexer.current_token() != Lexer::END) {
        if (lexer == Lexer::CTRL) {
            parseCtrlWordDecl();
        } else if (lexer == '(') {
            parseContextBlock();
        } else {
            parseAssignmentDecl();
        }
    }
}

template<typename T>
void init_depth_list(vector<T> l, int d) {
    if (l.size() > d) {
        l[d] = {};
    }
    l.emplace_back();
}

void Parser::init_var_depth(int d) {
    init_depth_list(macro_symtab, d);
    init_depth_list(expansion_list_symtab, d);
    init_depth_list(context, d);
    init_depth_list(statement_tab, d);
}

void Parser::parseContextBlock() {
    depth++;
    init_var_depth(depth);
    parseContextList();
    eat('{');
    while (lexer != '}') {
        if (lexer == '(') {
            parseContextBlock();
        } else {
            statement_tab[depth].push_back(parseStmt());
        }
    }

    vector<context_expr> context_list;
    for (const auto &i: context) {
        for (const auto &j: i) {
            context_list.push_back(j);
        }
    }

    translation_table.emplace_back(context_list, statement_tab[depth]);
}

void Parser::parseContextList() {
    while (lexer == '(') {
        eat('(');
        context[depth].push_back(parseContextExpr());
        eat(')');
    }
}

context_expr Parser::parseContextExpr() {
    if (lexer == '%') {
        string name = eat_id();

        eat(':');
        NUM_TYPE idx = eat_num();
        eat('%');
        return {true, make_shared<expansion_list_param>(name, idx), 0};
    } else {
        return {false, nullptr, parseExpr()};
    }
}

statement_list Parser::parseStmtList() {
    statement_list stmt_list;

    eat('{');
    while (lexer != '}') {
        stmt_list.push_back(parseStmt());
    }
    eat('}');
    return stmt_list;
}

statement Parser::parseStmt() {
    statement stmt;
    stmt.first = 0;
    while (lexer != ';') {
        NUM_TYPE val;
        if (lexer.current_token() == Lexer::IDENTIFIER) {
            string name = eat_id();

            if (lexer == '=') {
                if (bitset_tab.find(name) == bitset_tab.end()) {
                    perr(format("No bitset declaration found for {}", name));
                }

                eat('=');
                context_expr ex = parseContextExpr();
                stmt.second.emplace_back(name, ex);
            } else {
                stmt.first |= resolve_macro(name);
            }
        } else {
            stmt.first |= parseExpr();
        }

        if (lexer == ',') {
            eat(',');
        } else {
            eat(';');
            break;
        }
    }
    return stmt;
}

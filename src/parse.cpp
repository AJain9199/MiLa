#include <parse.h>
#include <error.h>
#include <string>
#include <format>
#include <stack>
#include <bitset>
#include <iostream>

using namespace std;

string to_bin(NUM_TYPE n, NUM_TYPE w) {
    string ans;
    for (int i = 0; i <= w-1; i++) {
        if (n & (1 << i)) {
            ans += '1';
        } else {
            ans += '0';
        }
    }
    reverse(ans.begin(), ans.end());
    return ans;
}

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
 * The resolved is a vector of [0, 0, 0], [0, 1, 0], [0, 2, 0], [1, 0, 0]...
 * */
template<typename T>
vector<vector<T>> cartesian_product(const vector<T> &indices) {
    vector<vector<T>> accum;
    vector<T> frontier;

    if (!indices.empty()) {
        cartesian_recurse(accum, frontier, indices, indices.size() - 1);
    }
    return accum;
}

void Parser::parseAssignmentDecl() {
    bool is_inverted = false;
    if (lexer.current_token() == Lexer::PUNCTUATION) {
        eat('~');
        is_inverted = true;
    }

    const string name = eat_id();
    bool bit_def = false;

    if (lexer == ':') {
        eat(':');
        bit_def = true;
    }

    eat('=');

    if (lexer.current_token() == Lexer::PUNCTUATION) {
        if (lexer.punc() != '[') {
            perr(format("Expected numeric literal, macro, or expansion list. Found '{}' instead.", lexer.punc()));
        }

        if (bit_def) {
            perr("Cannot have bit-definition with an expansion list.");
        }

        num_list val = parseNumList();
        // calculate maximum bit width
        expansion_list_symtab[depth][name] = {val, (NUM_TYPE)bit_width(*ranges::max_element(val))};
    } else {
        macro_symtab[depth][name] = init_macro_decl(parseExpr(bit_def), is_inverted);
    }
    eat(';');
}

void Parser::parseCtrlWordDecl() {
    eat(Lexer::CTRL);

    eat('{');
    NUM_TYPE pos = 0;
    while (lexer != '}') {
        bool is_inverted = false;
        if (lexer.current_token() == Lexer::PUNCTUATION) {
            eat('~');
            is_inverted = true;
        }

        string name = eat_id();

        int width = 1;

        if (lexer == ':') {
            eat(':');

            width = eat_num();
            bitset_tab[name] = make_pair(pos, pos + width - 1);
        } else {
            macro_symtab[0][name] = init_macro_decl(pos, is_inverted);

            if (is_inverted) {
                default_value |= (1 << pos);
            }
        }

        pos += width;

        if (lexer == ',') {
            eat(',');
        } else {
            eat('}');
            break;
        }
    }
    ctrl_word_width = max(ctrl_word_width, pos);
}

NUM_TYPE Parser::parseExpr(bool bit_def) {
    NUM_TYPE val = 0;
    if (lexer.current_token() == Lexer::NUMERIC_LITERAL) {
        val = lexer.num();
        lexer.getToken();
    } else if (lexer.current_token() == Lexer::IDENTIFIER) {
        macro_concat(val, resolve_macro(lexer.id()));
        lexer.getToken();
    } else {
        perr("Unexpected lvalue for assignment.");
    }
    return (bit_def ? (1 << val) : val);
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

macro_decl Parser::resolve_macro(const string &name) {
    for (int i = depth; i >= 0; i--) {
        if (macro_symtab[i].contains(name)) {
            return macro_symtab[i][name];
        }
    }
    perr(format("No value found for macro \"{}\" in current context", name));
    return {0, false};
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
            auto stmts = parseContextBlock();
            for (const auto &i : stmts) {
                translation_table.push_back(i);
            }
        } else {
            parseAssignmentDecl();
        }
    }
}

template<typename T>
void init_depth_list(vector<T> &l, int d) {
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

vector<pair<vector<context_expr>, statement_list>> Parser::parseContextBlock() {
    depth++;
    init_var_depth(depth);
    parseContextList();
    eat('{');

    vector<pair<vector<context_expr>, statement_list>> result;
    statement_list starting, ending;

    while (lexer != '(' && lexer != '}') {
        starting.push_back(parseStmt());
    }
    bool nested = true;
    if (lexer == '}') {
        nested = false;
    }

    while (lexer != '}') {
        if (lexer == '(') {
            auto tmp = parseContextBlock();
            for (const auto& i : tmp) {
                result.push_back(i);
            }
        } else {
            ending.push_back(parseStmt());
        }
    }

    eat('}');

    vector<context_expr> context_list;
    for (int i = 0; i <= depth; i++) {
        for (const auto &j : context[i]) {
            context_list.push_back(j);
        }
    }

    depth--;
    if (nested) {
        for (auto &[ctxt, stmt] : result) {
            stmt.insert(stmt.begin(), starting.begin(), starting.end());
            for (const auto &x:ending) {
                stmt.push_back(x);
            }
        }

        return result;
    } else {
        return {{context_list, starting}};
    }
}

void Parser::parseContextList() {
    while (lexer == '(') {
        eat('(');
        auto expr = parseContextExpr();
        if (lexer == ':') {
            eat(':');
            NUM_TYPE w = eat_num();
            expr.width = w;
        }

        context[depth].push_back(expr);
        eat(')');
    }
}

context_expr Parser::parseContextExpr() {
    if (lexer == '<') {
        lexer.getToken();
        string name = eat_id();

        eat(':');
        NUM_TYPE idx = eat_num();
        eat('>');
        return {true, make_shared<expansion_list_param>(name, idx), 0, resolve_expansion_list(name).width};
    } else {
        NUM_TYPE val = parseExpr();
        return {false, nullptr, val, (NUM_TYPE)(bit_width(val))};
    }
}

int Parser::perr(const std::string &msg) {
    err(lexer, msg, PARSE);
    return 0;
}

statement Parser::parseStmt() {
    statement stmt;
    stmt.first = default_value;
    while (lexer != ';') {
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
                macro_concat(stmt.first, resolve_macro(name));
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

NUM_TYPE Parser::resolve_context_descriptor(const vector<context_expr> &desc, const vector<unsigned long long> &indices, const map<string, vector<int>> &exp_list_resolution_tab) {
    NUM_TYPE translation = 0;
    for (const auto& i : desc) {
        if (i.param) {
            auto [exp_list, width] = resolve_expansion_list(i.exp_param->exp_name);
            translation <<= i.width;
            translation |= exp_list[indices[exp_list_resolution_tab.at(i.exp_param->exp_name)[i.exp_param->idx]]];
        } else {
            translation <<= i.width;
            translation |= i.num_literal;
        }
    }
    return translation;
}

void Parser::resolve_context_block(const pair<vector<context_expr>, statement_list> &block) {
    auto ctxt = block.first;
    vector<unsigned long long> indices;
    map<string, vector<int>> exp_idx;

    int idx = 0;
    bool param = false;
    for (const auto &i: ctxt) {
        if (!i.param) {
            continue;
        }

        param = true;

        num_list exp = resolve_expansion_list(i.exp_param->exp_name).exp_list;
        exp_idx[i.exp_param->exp_name].resize(i.exp_param->idx + 1);
        exp_idx[i.exp_param->exp_name][i.exp_param->idx] = idx;
        indices.push_back(exp.size());
        idx++;
    }

    if (param) {
        vector<vector<unsigned long long>> cart_prod = cartesian_product(indices);
        for (const auto& perm : cart_prod) {
            NUM_TYPE key = resolve_context_descriptor(ctxt, perm, exp_idx);
            num_list val;
            for (const auto &stmt : block.second) {
                val.push_back(resolve_statement(stmt, perm, exp_idx));
            }

            resolved.emplace_back(key, val);
        }
    } else {
        NUM_TYPE key = resolve_context_descriptor(ctxt, {}, {});
        num_list val;
        for (const auto &stmt: block.second) {
            val.push_back(resolve_statement(stmt, {}, {}));
        }

        resolved.emplace_back(key, val);
    }
}

NUM_TYPE Parser::resolve_statement(const statement &stmt, const vector<unsigned long long> &indices,
                                                 const map<string, vector<int>> &exp_list_resolution_tab
                                                 ) {
    NUM_TYPE translation = stmt.first;

    for (const auto &i : stmt.second) {
        auto [bitset_var, expr] = i;
        auto [l, r] = resolve_bitset(bitset_var);
        NUM_TYPE val;
        if (expr.param) {
            auto [exp_list, width] = resolve_expansion_list(expr.exp_param->exp_name);
            val = exp_list[indices[exp_list_resolution_tab.at(expr.exp_param->exp_name)[expr.exp_param->idx]]];

            if (bit_width(val) > (r-l+1)) {
                perr(format("Value {} of expansion list parameter exceed bit-width {} of bitset {}", val, (r-l+1), bitset_var));
            }
        } else {
             val = expr.num_literal;
        }
        translation |= (val << l);
    }
    return translation;
}

macro_decl Parser::init_macro_decl(NUM_TYPE pos, const bool is_inverted) {
    return {is_inverted?~pos:pos, is_inverted};
}

void Parser::macro_concat(unsigned long long &val, macro_decl m_decl) {
    val = m_decl.is_inverted?(val&m_decl.value):(val|m_decl.value);
}

void Parser::resolve() {
    for (const auto &i : translation_table) {
        resolve_context_block(i);
    }

    // calculate maximum number of microcode instructions for any instruction
    // bit width of the maximum no. of instructions becomes the width of our clock counter
    unsigned long long max_n_is = 0, max_ins = 0;
    for (const auto &[inst, subi] : resolved) {
        max_n_is = max(max_n_is, subi.size());
        max_ins = max(max_ins, inst);
    }

    int clock_w = bit_width(max_n_is);
    int max_clock_val = (1<<clock_w)-1;

    ins_width = bit_width(max_ins) + clock_w;

    // shift all existing instructions max_n_is bits to the left
    // instead of a nested vector of subcommands, the index is encoded in the least significant max_n_is bits
    for (const auto &[ins, sub] : resolved) {
        for (int i = 0; i < max_clock_val; i++) {
            output[(ins << clock_w) | i] = i < sub.size()?sub[i]:default_value;

            cout << to_bin((ins << clock_w) | i, ins_width) << ": " << to_bin((i < sub.size()?sub[i]:default_value), ctrl_word_width) << '\n';
        }
    }
}

expansion_list Parser::resolve_expansion_list(const string &name) {
    for (int d = depth; d >= 0; d--) {
        if (expansion_list_symtab[d].find(name) != expansion_list_symtab[d].end()) {
            return expansion_list_symtab[d].at(name);
        }
    }

    perr(format("Requested expansion list {} not found in current context", name));
}

std::pair<int, int> Parser::resolve_bitset(const string &name) {
    if (bitset_tab.find(name) != bitset_tab.end()) {
        return bitset_tab[name];
    }

    perr(format("Requested bitset {} not defined.", name));
}


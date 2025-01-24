#ifndef MILA_PARSER_H
#define MILA_PARSER_H

#include <vector>
#include <map>
#include <string>
#include <lexer.h>
#include <memory>

typedef long long ll;

typedef std::vector<NUM_TYPE> num_list;
typedef std::map<std::string, NUM_TYPE> macro_tab;

typedef struct {
    num_list exp_list;
    int width; // stores the width of the largest element
} expansion_list;
typedef std::map<std::string, expansion_list> expansion_list_tab;

typedef struct {
    std::string exp_name;
    int idx;
} expansion_list_param;

typedef struct {
    bool param;
    std::shared_ptr<expansion_list_param> exp_param;
    NUM_TYPE num_literal;
} context_expr;
typedef std::pair<NUM_TYPE, std::vector<std::pair<std::string, context_expr>>> statement;
typedef std::vector<statement> statement_list;

class Parser {
public:
    explicit Parser(const std::string &);
    void parseCode();
    void parseAssignmentDecl();
    void parseContextBlock();
    statement_list parseStmtList();
    statement parseStmt();
    void parseContextList();
    void parseCtrlWordDecl();
    NUM_TYPE parseExpr(bool bit_def=false);
    context_expr parseContextExpr();


private:
    int depth = 0;
    std::vector<expansion_list_tab> expansion_list_symtab{};
    std::vector<macro_tab> macro_symtab{};
    std::map<std::string, std::pair<int, int>> bitset_tab{};
    std::vector<statement_list> statement_tab;

    std::vector<std::pair<std::vector<context_expr>, statement_list>> translation_table;

    Lexer lexer;

    std::vector<std::vector<context_expr>> context;

    num_list parseNumList();

    NUM_TYPE resolve_macro(const std::string &);
    expansion_list resolve_expansion_list(const std::string &);
    std::pair<int, int> resolve_bitset(const std::string &);
    NUM_TYPE resolve_context_descriptor(const std::vector<context_expr> &desc, const std::vector<unsigned long long> &indices, const std::map<std::string, std::vector<int>> &exp_list_resolution_tab);
    NUM_TYPE resolve_context_block(const std::pair<std::vector<context_expr>, statement_list> &ctxt);

    std::vector<std::pair<NUM_TYPE, std::vector<NUM_TYPE>>> output;

    void eat(char);
    std::string eat_id();
    void eat(Lexer::TokenType);
    void eat(Lexer::Keyword);
    NUM_TYPE eat_num();

    void init_var_depth(int d);
};

#endif //MILA_PARSER_H

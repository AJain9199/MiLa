#ifndef MILA_PARSER_H
#define MILA_PARSER_H

#include <vector>
#include <map>
#include <string>
#include <lexer.h>
#include <memory>

typedef long long ll;


typedef std::vector<NUM_TYPE> num_list;
typedef std::map<std::string, num_list> expansion_list_tab;
typedef std::map<std::string, NUM_TYPE> macro_tab;

typedef struct {
    std::string exp_name;
    int idx;
} expansion_list_param;

typedef struct {
    bool param;
    std::shared_ptr<expansion_list_param> exp_param;
    NUM_TYPE num_literal;
} context_expr;

class Parser {
public:
    explicit Parser(const std::string &);
    void parseCode();
    void parseAssignmentDecl();
    void parseContextBlock();
    void parseContextList();
    void parseCtrlWordDecl();
    NUM_TYPE parseExpr(bool bit_def=false);
    context_expr parseContextExpr();


private:
    int depth = 0;
    std::vector<expansion_list_tab> expansion_list_symtab{};
    std::vector<macro_tab> macro_symtab{};
    std::map<std::string, std::pair<int, int>> bitset_tab{};

    Lexer lexer;

    std::vector<std::vector<context_expr>> context;

    num_list parseNumList();

    NUM_TYPE resolve_macro(const std::string &);

    void eat(char);
    std::string eat_id();
    void eat(Lexer::TokenType);
    void eat(Lexer::Keyword);
    NUM_TYPE eat_num();

    void init_var_depth(int d);
};

#endif //MILA_PARSER_H

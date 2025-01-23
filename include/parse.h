#ifndef MILA_PARSER_H
#define MILA_PARSER_H


#include <vector>
#include <map>
#include <string>
#include <lexer.h>

typedef std::vector<NUM_TYPE> num_list;
typedef std::map<std::string, num_list> expansion_list_tab;
typedef std::map<std::string, NUM_TYPE> macro_tab;

class Parser {
public:
    explicit Parser(std::string&);
    void parseAssignmentDecl();

private:
    int depth = 0;
    std::vector<expansion_list_tab> expansion_list_symtab;
    std::vector<macro_tab> macro_symtab;

    Lexer lexer;

    num_list parseNumList();

    NUM_TYPE resolve_macro(const std::string &);

    void eat(char);
    std::string eat_id();
    void eat(Lexer::TokenType);
};

#endif //MILA_PARSER_H

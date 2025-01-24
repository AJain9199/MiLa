#ifndef MILA_LEXER_H
#define MILA_LEXER_H

#include <fstream>
#include <string>
#include <cctype>
#include <stdexcept>
#include <set>
#include <map>

#define NUM_TYPE unsigned long long

class Lexer {
public:
    enum TokenType {
        IDENTIFIER,
        NUMERIC_LITERAL,
        PUNCTUATION,
        KEYWORD,
        END
    };

    enum Keyword {
        CTRL,
    };

    explicit Lexer(const std::string& filename);
    TokenType getToken();
    std::string id() const;
    NUM_TYPE num() const;
    enum Keyword key() const;
    char punc() const;
    enum TokenType current_token() const;

    bool operator==(char c);
    bool operator==(Keyword k);

private:
    std::ifstream input_file_;
    std::string current_token_;
    char current_char_{};
    void skipWs();
    TokenType token;
    Lexer *subfile{nullptr};

    const std::map<std::string, enum Keyword> keywords = {{"CTRL", CTRL}};
    const std::set<char> punctuation = {'(', ')', '{', '}', '[', ']', ':', ',', ';', '<', '>', '=', '.'};
};

#endif // MILA_LEXER_H
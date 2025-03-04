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
    [[nodiscard]] std::string id() const;
    [[nodiscard]] NUM_TYPE num() const;
    [[nodiscard]] enum Keyword key() const;
    [[nodiscard]] char punc() const;
    [[nodiscard]] enum TokenType current_token() const;
    [[nodiscard]] int line() const;
    [[nodiscard]] std::string filename() const;


    bool operator==(char c) const;
    bool operator==(Keyword k) const;

private:
    std::ifstream input_file_;
    std::string filename_;
    std::string current_token_;
    char current_char_{};
    int line_ = 1;
    void skipWs();
    TokenType token;
    Lexer *subfile{nullptr};

    void lerr(const std::string &msg) const;

    [[nodiscard]] std::string current_token_str() const;

    const std::map<std::string, enum Keyword> keywords = {{"CTRL", CTRL}};
    const std::set<char> punctuation = {'(', ')', '{', '}', '[', ']', ':', ',', ';', '<', '>', '=', '.', '~'};
};

#endif // MILA_LEXER_H
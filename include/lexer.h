#include <fstream>
#include <string>
#include <cctype>
#include <stdexcept>
#include <set>

#define NUM_TYPE long long

class Lexer {
public:
    enum TokenType {
        IDENTIFIER,
        NUMERIC_LITERAL,
        PUNCTUATION,
        END
    };

    explicit Lexer(const std::string& filename);
    TokenType getToken();
    std::string id() const;
    NUM_TYPE num() const;
    char punc() const;

    bool operator==(char c);

private:
    std::ifstream input_file_;
    std::string current_token_;
    char current_char_{};
    void skipWs();
    Lexer *subfile{nullptr};


    const std::set<char> punctuation = {'(', ')', '{', '}', '[', ']', ',', ';', '<', '>', '=', '.'};
};
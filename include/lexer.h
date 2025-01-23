#include <fstream>
#include <string>
#include <cctype>
#include <stdexcept>
#include <set>

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
    long long num() const;
    char punc() const;

private:
    std::ifstream input_file_;
    std::string current_token_;
    char current_char_{};
    Lexer *subfile{nullptr};

    void skipWs();

    const std::set<char> punctuation = {'(', ')', '{', '}', '[', ']', ',', ';', '<', '>', '=', '.'};
};
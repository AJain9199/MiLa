#include <lexer.h>
#include <error.h>

using namespace std;

Lexer::Lexer(const std::string &filename) : input_file_(filename) {
    if (!input_file_.is_open()) {
        throw std::runtime_error("Cannot open file");
    }
}

Lexer::TokenType Lexer::getToken() {
    if (subfile != nullptr) {
        enum TokenType e;
        if ((e = subfile->getToken()) != END) {
            return token = e;
        } else {
            delete subfile;
            subfile = nullptr;
        }
    }

    current_token_ = "";
    current_char_ = input_file_.get();

    skipWs();

    if (!input_file_.good()) {
        return token = END;
    }

    // identifier
    if (isalpha(current_char_) || current_char_ == '_') {
        while (isalnum(current_char_) || current_char_ == '_') {
            current_token_ += current_char_;
            current_char_ = input_file_.get();
        }
        input_file_.unget();

        if (current_token_ == "include") {
            skipWs();

            if (current_char_ != '"'){
                err("Expected file location after include directive", LEX);
            }

            string filename;
            current_char_ = input_file_.get();
            while (current_char_ != '"') {
                filename += current_char_;
                current_char_ = input_file_.get();
            }
            current_char_ = input_file_.get();

            subfile = new Lexer(filename);
            return getToken();
        }

        if (keywords.find(current_token_) != keywords.end()) {
            return token = KEYWORD;
        }
        return token = IDENTIFIER;
    }

    // numeric literal
    if (std::isdigit(current_char_)) {
        while (std::isalnum(current_char_)) {
            current_token_ += current_char_;
            current_char_ = input_file_.get();
        }
        input_file_.unget();
        return token = NUMERIC_LITERAL;
    }

    if (punctuation.find(current_char_) != punctuation.end()) {
        return token = PUNCTUATION;
    }

    err("Unknown character", 1);
    return token = END;
}

std::string Lexer::id() const {
    return current_token_;
}

NUM_TYPE Lexer::num() const {
    return stoll(current_token_, nullptr, 0);
}

char Lexer::punc() const {
    return current_char_;
}

void Lexer::skipWs() {
    while (isspace(current_char_) && input_file_.good()) {
        current_char_ = input_file_.get();
    }
}

bool Lexer::operator==(char c) {
    return token == PUNCTUATION && current_char_ == c;
}

Lexer::Keyword Lexer::key() const {
    return keywords.at(current_token_);
}

Lexer::TokenType Lexer::current_token() const {
    return token;
}

bool Lexer::operator==(Lexer::Keyword k) {
    return token == KEYWORD && key() == k;
}

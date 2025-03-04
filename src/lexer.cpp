#include <lexer.h>
#include <error.h>
#include <filesystem>

using namespace std;

Lexer::Lexer(const std::string &filename) : input_file_(filename), filename_(filename) {
    if (!input_file_.is_open()) {
        throw std::runtime_error("Cannot open file");
    }
}

Lexer::TokenType Lexer::getToken() {
    if (subfile != nullptr) {
        if (enum TokenType e; (e = subfile->getToken()) != END) {
            return token = e;
        }
        delete subfile;
        subfile = nullptr;
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
                lerr("Expected file location after include directive");
            }

            string filename;
            current_char_ = input_file_.get();
            while (current_char_ != '"') {
                filename += current_char_;
                current_char_ = input_file_.get();
            }
            current_char_ = input_file_.get();

            filesystem::path fpath(filename_);

            subfile = new Lexer((fpath.parent_path() / filename).string());
            return getToken();
        }

        if (keywords.contains(current_token_)) {
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

    if (punctuation.contains(current_char_)) {
        return token = PUNCTUATION;
    }

    lerr("Unknown character");
    return token = END;
}

std::string Lexer::id() const {
    return current_token_str();
}

NUM_TYPE Lexer::num() const {
    return stoll(current_token_str(), nullptr, 0);
}

char Lexer::punc() const {
    if (subfile != nullptr) {
        return subfile->current_char_;
    }
    return current_char_;
}

void Lexer::skipWs() {
    while (isspace(current_char_) && input_file_.good()) {
        if (current_char_ == '\n') {
            line_++;
        }
        current_char_ = input_file_.get();
    }
}

void Lexer::lerr(const std::string &msg) const {
    err(*this, msg, LEX);
}

std::string Lexer::current_token_str() const {
    if (subfile != nullptr) {
        return subfile->current_token_str();
    }
    return current_token_;
}

bool Lexer::operator==(char c) const {
    return current_token() == PUNCTUATION && punc() == c;
}

Lexer::Keyword Lexer::key() const {
    return keywords.at(current_token_str());
}

Lexer::TokenType Lexer::current_token() const {
    if (subfile != nullptr) {
        return subfile->current_token();
    }
    return token;
}

int Lexer::line() const {
    if (subfile != nullptr) {
        return subfile->line();
    }
    return line_;
}

std::string Lexer::filename() const {
    if (subfile != nullptr) {
        return subfile->filename();
    }

    return filename_;
}

bool Lexer::operator==(Lexer::Keyword k) const {
    return current_token() == KEYWORD && key() == k;
}

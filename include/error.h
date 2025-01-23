#ifndef AOS16_ERROR_H
#define AOS16_ERROR_H

#include <string>
#include <iostream>

#define LEX 1
#define PARSE 2

void err(std::string msg, int code) {
    std::cerr << msg;
}

#endif //AOS16_ERROR_H

#ifndef MILA_ERROR_H
#define MILA_ERROR_H

#include <string>
#include <iostream>

#define LEX 1
#define PARSE 2

void err(std::string msg, int code);

#endif //MILA_ERROR_H

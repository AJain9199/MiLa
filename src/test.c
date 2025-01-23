#include <lexer.h>
#include <stdio.h>
#include <stdlib.h>
#include <parser.h>

int main() {
    char *tst = "(MOV)%REGS:0xff% {}";
    long long ival;
    char *id = (char *)malloc(257);
    char sym;

    enum Token tok;

    while ((tok = lex(&tst, &ival, &sym, id)) != END) {
        if (tok == SYM) {
            printf("%c\n", sym);
        } else if (tok == NUM) {
            printf("%lld\n", ival);
        } else if (tok == ID) {
            printf("%s\n", id);
        }
    }

    free(id);

    return 0;
}
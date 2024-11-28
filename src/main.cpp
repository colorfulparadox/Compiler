#include<stdlib.h>

#include"lexer.h"
#include"parser.h"

int main(int argv, char **argc) {
    Lexer lexer("let a = 2; let b = a; let c = b; print(c);");
    lexer.TokenizeInput();

    Parser parser(lexer);
    auto* exe = parser.Parse();
    exe->Run();

    return EXIT_SUCCESS;
}
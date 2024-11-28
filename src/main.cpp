#include<stdlib.h>
#include <fstream>

#include"lexer.h"
#include"parser.h"

int main(int argv, char **argc) {
    
    std::ifstream file("input.txt");
    std::string input((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    Lexer lexer(input);
    lexer.TokenizeInput();

    Parser parser(lexer);
    auto exe = parser.Parse();
    exe.Run();

    return EXIT_SUCCESS;
}
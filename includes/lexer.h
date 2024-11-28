#pragma once
#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>

#include "token.h"

class Lexer {
public:
    Lexer(const std::string txt_input) : pos(0), line_num(1), input(txt_input) {}

    void TokenizeInput();
    
    const std::vector<Token>& GetTokens() const {
        return tokens;
    }

private:
    uint32_t pos; 
    uint32_t line_num;
    std::string input;

    std::vector<Token> tokens;

    void skipWhiteSpace();
    Token getNextToken();
};

#endif
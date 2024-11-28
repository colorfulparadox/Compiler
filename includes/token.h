#pragma once
#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>
#include <string>

enum TokenType {
    END_OF_FILE = 1,

    NUM,
    ID,

    LET,
    EQUAL,
    SEMICOLON,

    PLUS,
    MINUS,
    MULT,
    DIV,

    LPAREN,
    RPAREN,

    OUTPUT,

    ERROR
};

const std::string TOKEN_NAMES[TokenType::ERROR] = {
    "EOF",
    "NUM",
    "ID",

    "LET",
    "EQUAL",
    "SEMICOLON",

    "PLUS",
    "MINUS",
    "MULT",
    "DIV",

    "LPAREN",
    "RPAREN",

    "OUTPUT",

    "ERROR"
};

class Token {
public:
    TokenType type;
    std::string lexeme;
    uint32_t line_num;

    static const std::string& GetStringFromTokenType(TokenType type) {
        return TOKEN_NAMES[type-1];
    }

    void Print() {
        std::cout << "Token {" 
            << "\n\tTokenType: " 
            << TOKEN_NAMES[this->type-1]
            << "\n\tlexeme: "
            << this->lexeme
            << "\n\tline_num: "
            << this->line_num
            << "\n}\n"
            << std::endl;
    }

    const std::string& ToString() const {
        return TOKEN_NAMES[this->type-1];
    } 

    Token() {}
    Token(TokenType type, const std::string lexeme, uint32_t line_no)
        : type(type), lexeme(lexeme), line_num(line_no)
    {}
};


#endif

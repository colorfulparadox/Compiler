#include "lexer.h"
#include "token.h"
#include <cctype>
#include <vector>

const std::vector<char> CONSTANTS = {
    '=',
    '+',
    '-',
    '/',
    '*',
    ';',
    '(',
    ')'
};

const TokenType CONSTANT_TOKENS[8] = {
    EQUAL,
    PLUS,
    MINUS,
    DIV,
    MULT,
    SEMICOLON,
    LPAREN,
    RPAREN
};

const std::vector<std::string> KEYWORDS = {
    "let",
    "print"
};

const TokenType KEYWORD_TOKENS[2] = {
    LET,
    OUTPUT
};

int isKeyWord(const std::string& word) {
    int i = 0;
    for (std::string str : KEYWORDS) {
        if (word == str) {
            return i;
        }
        i += 1;
    }
    return -1;
}

void Lexer::TokenizeInput() {
    Token token = getNextToken();
    //token.Print(); 
    tokens.push_back(token);
    while (token.type != END_OF_FILE) {           
        token = getNextToken();
        tokens.push_back(token);
        //token.Print(); 
    }
}

Token Lexer::getNextToken() {
    skipWhiteSpace();

    if (pos >= input.length()) {
        return Token(TokenType::END_OF_FILE, "", line_num);
    }
    
    std::string remaining_input = input.substr(pos);

    Token token(TokenType::ERROR, remaining_input, line_num);

    char c = remaining_input[0];
    for (int i = 0; i < CONSTANTS.size(); i++) {
        if (c == CONSTANTS[i]) {
            token.type = CONSTANT_TOKENS[i];
            token.lexeme = c;
            pos += 1;
            return token;
        }
    }

    std::string num;
    for (char c : remaining_input) {
        if (!isdigit(c)) {
            break;
        } 
        num += c;
        token.lexeme = num;
        token.type = NUM;
        pos += 1;
    }
    if (token.type == NUM) {
        return token;
    }

    //check for words and keywords
    std::string word;
    for (char c : remaining_input) {
        if (!isalpha(c)) {
            break;
        }

        word += c;
        token.lexeme = word;
        token.type = ID;
        pos += 1;

        int keyword = isKeyWord(word);
        if (keyword != -1) {
            token.type = KEYWORD_TOKENS[keyword];
        }
    }
    if (token.type != ERROR) {
        return token;
    }

    pos += 1;
    token.lexeme = remaining_input;

    return token;
}

void Lexer::skipWhiteSpace() {
    while (pos < input.size() && std::isspace(input[pos])) {
        if (input[pos] == '\n') {
            line_num += 1;
        }
        pos += 1;
    }
}

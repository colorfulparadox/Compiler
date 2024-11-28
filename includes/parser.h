#pragma once

#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "lexer.h"
#include "expr.h"
#include "executer.h"
#include <cstdlib>


class Parser {
public:
    Parser(const Lexer& lexer) : tokens(lexer.GetTokens()), _lexer(lexer) {}

    void ParseScope(Executer* executer );
    Executer* Parse();

private:
    uint32_t pos = 0;
    const std::vector<Token>& tokens;
    [[maybe_unused]] const Lexer& _lexer; //keep it alive

    void expectTokens(std::vector<TokenType> list) {
        for (int i = 0; i < list.size(); i++) {
            this->expectToken(list[i]);
        }
    }

    void expectToken(TokenType expectedToken) {
        const Token* token = &tokens[pos];

        if (token->type == expectedToken) {
            std::cout << "GOT TOKEN: " << token->ToString() << "\n";
            pos += 1;
            return;
        }

        std::cout << "!SYNTAX ERROR! Expected TOKEN '" 
        << Token::GetStringFromTokenType(expectedToken) 
        << "' got TOKEN '" << token->ToString() 
        << "' on line '" << token->line_num << "'\n";

        exit(EXIT_FAILURE);
    }
    const Token& peekToken(uint32_t howFar) const {
        if (howFar == 0) {
            std::cout << "PARSER ERROR: Can not peek ahead by 0!\n";
            exit(EXIT_FAILURE);
        }

        int peek = pos + howFar - 1;
        if (peek >= tokens.size()) {
            return tokens.back();
        }
        return tokens[peek];
    }

    ExprNode* parseExpr(Scope *scope);
};

#endif
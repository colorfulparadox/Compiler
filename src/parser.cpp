#include "parser.h"
#include "token.h"
#include "executer.h"
#include <cstdlib>

void syntaxError(TokenType expected, uint32_t line_num) {
    std::cout << "SYNTAX ERROR "
        //<< Token::GetStringFromTokenType(expected)
        << "on line '" << line_num << "'!\n";
    exit(EXIT_FAILURE);
}

Executer* Parser::Parse() {
    Executer* executer = new Executer(new Scope());
    this->ParseScope(executer);
    return executer;
}

void Parser::ParseScope(Executer* executer) {
    auto& t1 = this->peekToken(1);
    auto& t2 = this->peekToken(2);
    auto& t3 = this->peekToken(3);
    
    if (t1.type == TokenType::END_OF_FILE) {
        std::cout << "Reached EOF\n";
        return;
    }

    if (t1.type == TokenType::LET && t2.type == TokenType::ID && t3.type == TokenType::EQUAL) {
        this->expectTokens({t1.type, t2.type, t3.type});
        ExprNode* result = this->parseExpr(executer->scope);
        this->expectToken(SEMICOLON);

        uint32_t offset = executer->scope->AddSymbol(t2.lexeme);   

        CodeVarInit* code = new CodeVarInit(offset, result);
        executer->PushCode(code);
    }
    else if (t1.type == TokenType::ID && t2.type == TokenType::EQUAL) {
        this->expectTokens({t1.type, t2.type});
        ExprNode* result = this->parseExpr(executer->scope);
        this->expectToken(SEMICOLON);

        CodeVarAssign* code = new CodeVarAssign(executer->scope->GetSymbol(t1.lexeme)->offset, result);
        executer->PushCode(code);
    }
    else if (t1.type == TokenType::OUTPUT && t2.type == TokenType::LPAREN) {
        this->expectTokens({t1.type, t2.type});
        ExprNode* result = this->parseExpr(executer->scope);
        this->expectTokens({TokenType::RPAREN, TokenType::SEMICOLON});

        CodePrint* code = new CodePrint(result);
        executer->PushCode(code);
    }
    else {
        syntaxError(t1.type, t1.line_num);
    }
    this->ParseScope(executer);
}


ExprNode* Parser::parseExpr(Scope *scope) {
    auto& token = this->peekToken(1);
    
    if (token.type == NUM) {
        expectToken(NUM);
        return new ExprNum(token);
    } 
    else {
        expectToken(ID);
        ExprVarAccess* node = new ExprVarAccess(scope->GetSymbol(token.lexeme)->offset);
        return node;
    }
}
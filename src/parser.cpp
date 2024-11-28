#include "parser.h"
#include "expr.h"
#include "token.h"
#include "executer.h"
#include <cstdlib>
#include <stack>

void syntaxError(TokenType expected, uint32_t line_num) {
    std::cout << "SYNTAX ERROR "
        //<< Token::GetStringFromTokenType(expected)
        << "on line '" << line_num << "'!\n";
    exit(EXIT_FAILURE);
}

Executer Parser::Parse() {
    Executer executer(new Scope());
    this->ParseScope(executer);
    return executer;
}

void Parser::ParseScope(Executer& executer) {
    auto& t1 = this->peekToken(1);
    auto& t2 = this->peekToken(2);
    auto& t3 = this->peekToken(3);
    
    if (t1.type == TokenType::END_OF_FILE) {
        //std::cout << "Reached EOF\n";
        return;
    }

    if (t1.type == TokenType::LET && t2.type == TokenType::ID && t3.type == TokenType::EQUAL) {
        this->expectTokens({t1.type, t2.type, t3.type});
        ExprNode* result = this->parseExpr(executer.scope);
        this->expectToken(SEMICOLON);

        uint32_t offset = executer.scope->AddSymbol(t2.lexeme);   

        CodeVarInit* code = new CodeVarInit(offset, result);
        executer.PushCode(code);
    }
    else if (t1.type == TokenType::ID && t2.type == TokenType::EQUAL) {
        this->expectTokens({t1.type, t2.type});
        ExprNode* result = this->parseExpr(executer.scope);
        this->expectToken(SEMICOLON);

        CodeVarAssign* code = new CodeVarAssign(executer.scope->GetSymbol(t1.lexeme)->offset, result);
        executer.PushCode(code);
    }
    else if (t1.type == TokenType::OUTPUT && t2.type == TokenType::LPAREN) {
        this->expectTokens({t1.type, t2.type});
        ExprNode* result = this->parseExpr(executer.scope);
        this->expectTokens({TokenType::SEMICOLON});

        CodePrint* code = new CodePrint(result);
        executer.PushCode(code);
    }
    else {
        syntaxError(t1.type, t1.line_num);
    }
    this->ParseScope(executer);
}

uint32_t opPrecedence(TokenType op) {
    if (op == PLUS || op == MINUS) {
        return 1;
    }
    else if (op == MULT || op == DIV) {
        return 2;
    }
    return 0;
}

void processOperator(std::stack<ExprNode*>& operandStack,  std::stack<ExprOP*>& operatorStack) {
    ExprOP* op = operatorStack.top();
    operatorStack.pop();
    op->right = operandStack.top();
    operandStack.pop();
    op->left= operandStack.top();
    operandStack.pop();
    operandStack.push(op);
}

void expressionSyntaxError(int line_num) {
    std::cout << "Expression Syntax Error on line: '" << line_num << "'\n";
    exit(EXIT_FAILURE);
}

ExprNode* Parser::parseExpr(Scope *scope) {
    auto token = this->peekToken(1);
    
    std::stack<ExprNode*> operandStack;
    std::stack<ExprOP*> operatorStack;

    bool lastTokenOp = true;
    while (token.type != TokenType::SEMICOLON) {
        if (token.type == TokenType::NUM && lastTokenOp) {
            //std::cout << "num\n";
            operandStack.push(new ExprNum(token));
            lastTokenOp = false;
        }
        else if (token.type == TokenType::ID && lastTokenOp) {
           // std::cout << "id\n";
            uint32_t offset = scope->GetSymbol(token.lexeme)->offset;
            operandStack.push(new ExprVarAccess(offset));
            lastTokenOp = false;
        }
        else if ((token.type == TokenType::PLUS || token.type == TokenType::MINUS
        || token.type == TokenType::MULT || token.type == TokenType::DIV) && !lastTokenOp) {
            while(operatorStack.size() != 0 && opPrecedence(operatorStack.top()->type) >= opPrecedence(token.type)) {
                processOperator(operandStack, operatorStack);
            }
            operatorStack.push(new ExprOP(token));
            lastTokenOp = true;
        }
        else if (token.type == TokenType::LPAREN) {
            operatorStack.push(new ExprOP(token));
            lastTokenOp = true;
        }
        else if (token.type == TokenType::RPAREN && this->peekToken(2).type != TokenType::SEMICOLON) {
            while (operatorStack.size() != 0 && operatorStack.top()->type != TokenType::LPAREN) {
                processOperator(operandStack, operatorStack);
            }
            operatorStack.pop();
        }
        else if (lastTokenOp || (!lastTokenOp && token.type == NUM) || (!lastTokenOp && token.type == ID)) {
            expressionSyntaxError(token.line_num);
        }

        expectToken(token.type);
        token = this->peekToken(1);
        //token.Print();
    }
    while (operatorStack.size() != 0) {
        if (operatorStack.top()->type == TokenType::LPAREN) {
            operatorStack.pop();
        }
        processOperator(operandStack, operatorStack);
    }

    return operandStack.top();
}
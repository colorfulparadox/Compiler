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
        this->expectTokens({TokenType::RPAREN, TokenType::SEMICOLON});

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
    //std::cout << "process op type: " << op->GetOp() << "\n";
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


void Parser::expr(std::vector<Token>& list) {
    term(list);
    Token t1 = this->peekToken(1);

    while (t1.type == TokenType::PLUS ||  t1.type == TokenType::MINUS
    || t1.type == TokenType::MULT ||  t1.type == TokenType::DIV) {
        this->expectToken(t1.type);
        list.emplace_back(t1);
        term(list);
        t1 = this->peekToken(1);
    }
    if (t1.type == NUM || t1.type == ID) {
        expressionSyntaxError(t1.line_num);
    }
}

void Parser::term(std::vector<Token>& list) {
    Token t1 = this->peekToken(1);
    if (t1.type == NUM || t1.type == ID) {
        this->expectToken(t1.type);
        list.emplace_back(t1);
    }
    else if (t1.type == LPAREN) {
        list.emplace_back(this->peekToken(1)); // '('
        this->expectToken(LPAREN);
        expr(list);                                 // EXPR
        list.emplace_back(this->peekToken(1)); // ')'
        this->expectToken(RPAREN);
    }
    else {
        expressionSyntaxError(t1.line_num);
    }
}

ExprNode* Parser::parseExpr(Scope *scope) {
    int exprPos = 0;
    std::vector<Token> list;
    list.reserve(6);
    this->expr(list);

    std::stack<ExprNode*> operandStack;
    std::stack<ExprOP*> operatorStack;

    while (exprPos < list.size()) {
        auto token = list.at(exprPos);

        if (token.type == TokenType::NUM) {
            operandStack.push(new ExprNum(token));
        }
        else if (token.type == TokenType::ID) {
            uint32_t offset = scope->GetSymbol(token.lexeme)->offset;
            operandStack.push(new ExprVarAccess(offset));
        }
        else if ((token.type == TokenType::PLUS || token.type == TokenType::MINUS
        || token.type == TokenType::MULT || token.type == TokenType::DIV)) {
            while(operatorStack.size() != 0 && opPrecedence(operatorStack.top()->type) >= opPrecedence(token.type)) {
                processOperator(operandStack, operatorStack);
            }
            operatorStack.push(new ExprOP(token));
        }
        else if (token.type == TokenType::LPAREN) {
            operatorStack.push(new ExprOP(token));
        }
        else if (token.type == TokenType::RPAREN) {
            while (operatorStack.size() != 0 && operatorStack.top()->type != TokenType::LPAREN) {
                processOperator(operandStack, operatorStack);
            }
            operatorStack.pop();
        }

        exprPos += 1;
    }

    //std::cout << ":: END LOOP BEGINS HERE ::\n";

    /*
    auto a = operatorStack;
    auto b = operandStack;
    while (!a.empty()) {
        std::cout << "opStack: " << a.top()->GetOp() << "\n";
        a.pop();
    }
    while (!b.empty()) {
        std::cout << "operandStack: " << b.top()->GetValue() << "\n";
        b.pop();
    }
    */

    while (operatorStack.size() != 0) {
        if (operatorStack.top()->type == TokenType::LPAREN) {
            operatorStack.pop();
        }
        processOperator(operandStack, operatorStack);
    }

    //std::cout << "end of parseExpr\n";
    return operandStack.top();
}
#pragma  once
#ifndef EXPR_H
#define EXPR_H

#include "token.h"

class ExprNode {
public:
    ExprNode* left;
    ExprNode* right;

    virtual int GetValue() = 0;
private:
};


class ExprOP : public ExprNode {
public:
    TokenType type;
    ExprOP(const Token& token) : type(token.type), value(token.lexeme[0]) {}

    void SetLeft(int val) {
        this->left_val = val;
    }

    void SetRight(int val) {
        this->right_val = val;
    }

    virtual int GetValue() {
        switch (this->value) {
            case '+':
                return left_val + right_val;
            case '-':
                return left_val - right_val;
            case '*':
                return left_val * right_val;
            case '/':
                //std::cout << "left val: " << left_val << "right_val: " << right_val << "\n";
                return left_val / right_val;
            default:
                return -1;
        }
    }
private:
    const char value;
    int left_val;
    int right_val;
};


class ExprNum : public ExprNode {
public:
    ExprNum(const Token& token) : value(std::stoi(token.lexeme)) {}
    virtual int GetValue() {
        return this->value;
    }
private:
    const int value;
};

class ExprVarAccess : public ExprNode {
public:
    ExprVarAccess(int offset) : value(offset) {}
    virtual int GetValue() {
        return this->value;
    }
private:
    const int value;
};

#endif
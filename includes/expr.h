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
    ExprOP(const Token& token) : value(token.lexeme[0]) {}

    void SetLeft(int val) {
        this->left = val;
    }

    void SetRight(int val) {
        this->right = val;
    }

    virtual int GetValue() {
        switch (this->value) {
            case '+':
                return left + right;
            case '-':
                return left - right;
            case '*':
                return left * right;
            case '/':
                return left / right;
            default:
                return -1;
        }
    }
private:
    const char value;
    int left;
    int right;
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
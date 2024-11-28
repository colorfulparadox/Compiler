#pragma once
#ifndef EXECUTER_H
#define EXECUTER_H

#include <iostream>
#include <unordered_map>
#include <string>
#include <cstddef>
#include "expr.h"


typedef struct Symbol_T {
    const uint32_t offset;
    const std::string& name;
} Symbol;

class Scope {
public:
    uint32_t AddSymbol(const std::string& name) {
        symbols.insert({
            name, 
            Symbol {
                .offset = offset,
                .name = name
            }
        });
        offset += 1;
        return offset-1;
    }

    const Symbol* GetSymbol(const std::string& name) {
        if (!symbols.contains(name)) {
            return nullptr;
        }
        return &symbols.at(name);
    }

    void AddValueAt(int offset, int value) {
        this->scope[offset] = value;
    }

    int GetValue(uint32_t offset) {
        return scope[offset];
    }

    Scope() : offset(0) {}

private:
    uint32_t offset;
    //Scope* srcScope;
    std::unordered_map<std::string, Symbol> symbols;
    int scope[100];
};

static int RunExpr(ExprNode* node, Scope* scope) {
    int result = 0;

    if (dynamic_cast<ExprOP*>(node)) {
        ExprOP* op = (ExprOP*) node;
        op->SetLeft(RunExpr(node->left, scope));
        op->SetRight(RunExpr(node->right, scope));
        result = op->GetValue();
    }
    else if (dynamic_cast<ExprNum*>(node)) {
        result = node->GetValue();
    }
    else if (dynamic_cast<ExprVarAccess*>(node)){
        uint32_t offset = node->GetValue();
        result = scope->GetValue(offset);
    }
    return result;
}

class CodeNode {
public:
    virtual CodeNode* execute(Scope *scope) = 0;
    CodeNode *next = NULL;
private:
};

class CodeVarInit : public CodeNode { 
public:
    CodeVarInit(uint32_t offset, ExprNode* node) : varOffset(offset), expr(node) {}
    virtual CodeNode* execute(Scope *scope) {
        int result = RunExpr(expr, scope);
        scope->AddValueAt(varOffset, result);
        return this->next;
    }
private:
    uint32_t varOffset;
    ExprNode *expr;
};

class CodeVarAssign : public CodeNode { 
public:
    CodeVarAssign(uint32_t offset, ExprNode* node) : varOffset(offset), expr(node) {}
    virtual CodeNode* execute(Scope *scope) {
        //solve exprNode
        int result = RunExpr(expr, scope);
        scope->AddValueAt(varOffset, result);
        return this->next;
    }
private:
    uint32_t varOffset;
    ExprNode *expr;
};

class CodePrint : public CodeNode {
public:
    CodePrint(ExprNode *node) : expr(node) {}
    virtual CodeNode* execute(Scope *scope) {
        //solve exprNode
        int result = RunExpr(expr, scope);
        std::cout << result << "\n";
        return this->next;
    }
private:
    ExprNode *expr;
};

class Executer {
public: 
    Scope *scope;

    Executer(Scope *scope) : scope(scope) {}

    void PushCode(CodeNode* code) {
        if (head == NULL) {
            head = code;
            return;
        }
        
        if (tail == NULL) {
            head->next = code;
            tail = code;
            return;
        }

        tail->next = code;
        tail = code;
    } 

    void Run() {
        std::cout << "CODE EXE STARTED\n";
        CodeNode *node = head;
        while (node != NULL) {
            node = node->execute(this->scope);
        }
        std::cout << "CODE EXE ENDED\n";
    }

private:
    CodeNode *head = NULL;
    CodeNode *tail = NULL;
};


#endif
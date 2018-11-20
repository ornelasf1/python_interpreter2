
#include <string>
#include <vector>

#ifndef PARSER_H
#define PARSER_H

extern void parse();

class Variable{
    public:
    Variable();
    Variable(std::string);
    Variable(std::string, std::string value);
    void setValue(std::string value);
    std::string name;
    std::string value;
};

class Scope{
    public:
    Scope();
    Scope(std::vector<Variable*>, int);
    Scope* innerScope;
    Scope* outerScope;
    static void traverseToScope(int);
    std::vector<Variable*> scopeVariables;
    //static int scopeLevel;
    int scopeLevel;
};

#endif
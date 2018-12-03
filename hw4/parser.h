
#include <string>
#include <vector>
#include <stack>

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
    std::vector<std::string> value_s;
    bool isFunction;
};

class Function : public Variable{
    public:
    Function();
    std::string printCalls;
    std::vector<Variable*> argsv;
    int argc;

    int funcDefinition_LineNum;
    int funcDefinition_START;
    int funcDefinition_END;

    std::streampos funcDefinStream_START;
    std::streampos funcDefinStream_END;

    //bool doesItEverTerminate;
};

class Scope{
    public:
    Scope();
    Scope(std::vector<Variable*>, int);
    Scope* innerScope;
    Scope* outerScope;
    std::vector<Variable*> scopeVariables;
    Function* fetchFunction(std::string);
    std::string actingFunctionName;
    Scope* getFunctionScope();
    bool functionMode;
    //static int scopeLevel;
    int scopeLevel;
};

#endif

#include <string>

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

#endif
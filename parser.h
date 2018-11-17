
#include <string>

#ifndef PARSER_H
#define PARSER_H

extern void parse();

template<class T>
class Variable{
    Variable(std::string);
    Variable(std::string, T value);
    void setValue(T value);
    std::string name;
    T value;
};

#endif
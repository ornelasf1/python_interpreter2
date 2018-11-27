#include <vector>
#include <string>
#include <fstream>

#ifndef LEX_H
#define LEX_H

enum TOKENS{
    IDENTIFIER,
    INTEGER,
    STRING_LITERAL,
    ADD_OP,
    SUB_OP,
    MULT_OP,
    DIV_OP,
    ASSIGN_OP,
    COND_EQUAL,
    COND_GTEQ,
    COND_GT,
    COND_LTEQ,
    COND_LT,
    COND_NOT_OP,
    COND_AND,
    COND_OR,
    LEFT_PAREN,
    RIGHT_PAREN,
    STRING_QUOTE,
    CHAR_QUOTE,
    COLON,
    COMMA,
    INDENT,
    LINEBREAK,
    END
};

enum CHARCLASS{
    DIGIT,
    LETTER,
    OTHER,
    CHAR_END
};

extern std::ifstream inputFile;
extern std::string lexeme;
extern char nextChar;
extern CHARCLASS charClass;
extern TOKENS nextToken;
extern std::vector<TOKENS> tokens;
extern std::vector<std::string> lexemes;
extern int indentCount;
extern bool endProgram;
extern bool string_literal;
extern bool indent;
extern int numOfIndents;
extern bool codeBlockMode;
extern std::string outputStream;

extern void getChar();
extern void lex();
extern void processSpaces();
extern void lookup(char);
extern void addChar();

#endif



#include <vector>
#include <string>
#include <fstream>

#ifndef LEX_H
#define LEX_H

enum TOKENS{
    IDENTIFIER,
    INTEGER,
    ADD_OP,
    SUB_OP,
    ASSIGN_OP,
    LEFT_PAREN,
    RIGHT_PAREN,
    STRING_QUOTE,
    CHAR_QUOTE,
    COLON,
    INDENT,
    LINEBREAK,
    END
};

enum CHARCLASS{
    DIGIT,
    LETTER,
    OTHER
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

extern void getChar();
extern void lex();
extern void processSpaces();
extern void lookup(char);
extern void addChar();

#endif



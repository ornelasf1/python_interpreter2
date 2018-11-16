#include <vector>
#include <string>
#include <fstream>

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

class Lexical{
    ifstream inputFile;
    std::string lexeme;
    char nextChar;
    CHARCLASS charClass;
    TOKENS token;
    std::vector<TOKENS> tokens;
    std::vector<std::string> lexemes;
    int indentCount;
    void getChar();
    void lex();
    void processSpaces();
    void lookup(char);
    void addChar();
}





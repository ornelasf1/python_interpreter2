#include <fstream>
#include <iostream>
#include "lexical.h"

using namespace std;

Lexical::Lexical(){}

void Lexical::getChar(){
    if(inputFile.get(nextChar)){
        if(isalpha(nextChar)){
            printf("Char: %c\n", nextChar);
            charClass = LETTER;
        }else if(isdigit(nextChar)){
            printf("Dig: %c\n", nextChar);
            charClass = DIGIT;
        }else{
            if(nextChar != '\n') printf("Other: %c\n", nextChar);
            else printf("Other: \\n\n");
            charClass = OTHER;
        }
    }else{
        printf("EOF\n");
        token = END;
    }
}
void Lexical::lex(){
    lexeme = "";
    processSpaces();
    switch (charClass)
    {
    case LETTER:
        addChar();
        getChar();
        while (charClass == LETTER || charClass == DIGIT)
        {
            addChar();
            getChar();
        }
        token = IDENTIFIER;
        break;
    case DIGIT:
        addChar();
        getChar();
        while (charClass == DIGIT)
        {
            addChar();
            getChar();
        }
        token = INTEGER;
        break;
    case OTHER:
        lookup(nextChar);
        getChar();
        break;
    }
    if (lexeme == "\n")
    {
        cout << "Found line break" << endl
             << endl;
    }
    else if (lexeme == "\t")
    {
        cout << "Found indent" << endl
             << endl;
    }
    else
    {
        cout << "Found " << lexeme << endl
             << endl;
    }
    tokens.push_back(token);
    lexemes.push_back(lexeme);
}
void Lexical::processSpaces(){
    int spaces = indentCount;
    while(isspace(nextChar) && nextChar != '\n'){
        cout << spaces << endl;
        if(--spaces == 0){
            charClass = OTHER;
            nextChar = '\t';
            break;
        }
        printf("Skip whitespace\n");
        getChar();
    }
}
void Lexical::lookup(char c){
    switch(c){
        case '=':
            addChar();
            token = ASSIGN_OP;
            break;
        case '(':
            addChar();
            token = LEFT_PAREN;
            break;
        case ')':
            addChar();
            token = RIGHT_PAREN;
            break;
        case ':':
            addChar();
            token = COLON;
            break;
        case '"':
            addChar();
            token = STRING_QUOTE;
            break;
        case '\'':
            addChar();
            token = CHAR_QUOTE;
            break;
        case '\t':
            addChar();
            token = INDENT;
            break;
        case '\n':
            addChar();
            token = LINEBREAK;
            break;
        default:
            token = END;
            break;

    }
}

void Lexical::addChar(){
    if(isspace(nextChar) && nextChar != '\n' && nextChar != '\t') cout << "Add space" << endl;
    lexeme += nextChar;
};
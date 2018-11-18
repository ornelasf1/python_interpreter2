#include <fstream>
#include <iostream>
#include "lexical.h"

using namespace std;

void getChar(){
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
            if(nextChar == '"'){
                string_literal = !string_literal;
                printf("Activate string literal\n");
            }
        }
    }else{
        printf("EOF\n");
        charClass = CHAR_END;
        nextToken = END;
    }
}
void lex(){
    string oldLex = lexeme;
    lexeme = "";
    processSpaces();
    switch (charClass){
    case LETTER:
        addChar();
        getChar();
        if(string_literal) nextToken = STRING_LITERAL;
        else nextToken = IDENTIFIER;
        while (charClass == LETTER || charClass == DIGIT || string_literal){
            addChar();
            getChar();
        }
        
        break;
    case DIGIT:
        addChar();
        getChar();
        while (charClass == DIGIT){
            addChar();
            getChar();
        }
        nextToken = INTEGER;
        break;
    case OTHER:
        lookup(nextChar);
        getChar();
        break;
    default:
        nextToken = END;
    }
    cout << oldLex << " -> " << lexeme << endl;

    if (lexeme == "\n"){
        cout << "Found line break" << endl << endl;
    }else if (lexeme == "\t"){
        cout << "Found indent" << endl << endl;
    }else{
        cout << "Found " << lexeme << endl << endl;
    }
    tokens.push_back(nextToken);
    lexemes.push_back(lexeme);
}

void processSpaces(){
    int spaces = indentCount;
    while(isspace(nextChar) && nextChar != '\n' && !string_literal){
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
void lookup(char c){
    switch(c){
        case '=':
            addChar();
            nextToken = ASSIGN_OP;
            break;
        case '+':
            addChar();
            nextToken = ADD_OP;
            break;
        case '-':
            addChar();
            nextToken = SUB_OP;
            break;
        case '*':
            addChar();
            nextToken = MULT_OP;
            break;
        case '/':
            addChar();
            nextToken = DIV_OP;
            break;
        case '(':
            addChar();
            nextToken = LEFT_PAREN;
            break;
        case ')':
            addChar();
            nextToken = RIGHT_PAREN;
            break;
        case ':':
            addChar();
            nextToken = COLON;
            break;
        case ',':
            addChar();
            nextToken = COMMA;
            break;
        case '"':
            addChar();
            nextToken = STRING_QUOTE;
            break;
        case '\'':
            addChar();
            nextToken = CHAR_QUOTE;
            break;
        case '\t':
            addChar();
            nextToken = INDENT;
            break;
        case '\n':
            addChar();
            nextToken = LINEBREAK;
            break;
        default:
            nextToken = END;
            break;

    }
}

void addChar(){
    if(isspace(nextChar) && nextChar != '\n' && nextChar != '\t') cout << "Add space" << endl;
    lexeme += nextChar;
};

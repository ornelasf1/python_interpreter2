#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
#include <vector>
#include "tokens.h"

using namespace std;


// enum TOKENS{
//     IDENTIFIER,
//     INTEGER,
//     ADD_OP,
//     SUB_OP,
//     ASSIGN_OP,
//     LEFT_PAREN,
//     RIGHT_PAREN,
//     STRING_QUOTE,
//     CHAR_QUOTE,
//     COLON,
//     INDENT,
//     LINEBREAK,
//     END
// };

// enum CHARCLASS{
//     DIGIT,
//     LETTER,
//     OTHER
// };

// ifstream inputFile;
// string lexeme;
// char nextChar;
// CHARCLASS charClass;
// TOKENS token;
// vector<TOKENS> tokens;
// vector<string> lexemes;
// int indentCount = 4;

// void getChar();
// void lex();
// void processSpaces();
// void lookup(char);
// void addChar();

int main(int argc, char** argv){
    ifstream inputfile;
    Lexical lex;

    string filename;
    try{
        if(argc == 2){
            filename = argv[1];
            string ext = filename.substr(filename.find("."), string::npos);
            if(ext != ".py"){
                throw;
            }
            inputFile.open(filename);
        }else{
            throw;
        }
    }catch(...){
        printf("Input: mypython <file.py>");
    }

    if(inputFile.is_open()){
        lex = new Lexical(inputfile);
        getChar();
        do{
            lex();
        }while(token != END);
    }

    inputFile.close();

    printf("PRINTING LEXEMES\n");
    for(int i = 0; i < lexemes.size(); i++){
        if(lexemes[i] == "\t") cout << "indent ";
        else if(lexemes[i] == "\n") cout << "linebreak ";
        else cout << lexemes[i] << " ";
    }

    return 0;
}

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
        }
    }else{
        printf("EOF\n");
        token = END;
    }
}

void processSpaces(){
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

//Adds char to lexeme
void addChar(){
    if(isspace(nextChar) && nextChar != '\n' && nextChar != '\t') cout << "Add space" << endl;
    lexeme += nextChar;
}

void lex(){
    lexeme = "";
    processSpaces();
    switch(charClass){
        case LETTER:
            addChar();
            getChar();
            while(charClass == LETTER || charClass == DIGIT){
                addChar();
                getChar();
            }
            token = IDENTIFIER;
            break;
        case DIGIT:
            addChar();
            getChar();
            while(charClass == DIGIT){
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
    if(lexeme == "\n"){
        cout << "Found line break" << endl << endl;
    }else if(lexeme == "\t"){
        cout << "Found indent" << endl << endl;
    }else{
        cout << "Found " << lexeme << endl << endl;
    }
    tokens.push_back(token);
    lexemes.push_back(lexeme);
}

void lookup(char c){
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
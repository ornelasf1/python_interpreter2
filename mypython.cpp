#include "lexical.h"
#include "parser.h"

#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

std::ifstream inputFile;
std::string lexeme;
char nextChar;
CHARCLASS charClass;
TOKENS nextToken;
std::vector<TOKENS> tokens;
std::vector<std::string> lexemes;
int indentCount;
bool endProgram;
bool string_literal;
void getChar();
void lex();
void processSpaces();
void lookup(char);
void addChar();

void parse();

char* tksnames[] = {
    "IDENTIFIER",
    "INTEGER",
    "STRING_LITERAL",
    "ADD_OP",
    "SUB_OP",
    "MULT_OP",
    "DIV_OP",
    "ASSIGN_OP",
    "LEFT_PAREN",
    "RIGHT_PAREN",
    "STRING_QUOTE",
    "CHAR_QUOTE",
    "COLON",
    "COMMA",
    "INDENT",
    "LINEBREAK",
    "END"
};


int main(int argc, char** argv){
    indentCount = 4;
    endProgram = false;
    string_literal = false;
    string filename;
    try{
        if(argc == 2){
            filename = argv[1];
            string ext = filename.substr(filename.find("."), string::npos);
            if(ext != ".py"){
                throw;
            }
            inputFile.open(filename.c_str());
        }else{
            throw;
        }
    }catch(...){
        printf("Input: mypython <file.py>");
    }


    if(inputFile.is_open()){
        getChar();
        do{
            lex();
            parse();
            if(endProgram) break;
        }while(nextToken != END);
    }

    inputFile.close();

    printf("PRINTING LEXEMES\n");
    for(int i = 0; i < lexemes.size(); i++){
        if(lexemes[i] == "\t") cout << "indent ";
        else if(lexemes[i] == "\n") cout << "linebreak ";
        else cout << lexemes[i] << " ";
    }
    cout << endl;

    printf("PRINTING TOKENS\n");
    for(int i = 0; i < tokens.size(); i++){
        cout << tksnames[tokens[i]] << " ";
    }
    cout << endl;

    return 0;
}
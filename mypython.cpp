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
bool indent;
int numOfIndents;
bool codeBlockMode;
string outputStream;
void getChar();
void lex();
void processSpaces();
void lookup(char);
void addChar();

void parse();

string tksnames[] = {
    "IDENTIFIER",
    "INTEGER",
    "STRING_LITERAL",
    "ADD_OP",
    "SUB_OP",
    "MULT_OP",
    "DIV_OP",
    "ASSIGN_OP",
    "COND_EQUAL",
    "COND_GTEQ",
    "COND_GT",
    "COND_LTEQ",
    "COND_LT",
    "COND_AND",
    "COND_OR",
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
    numOfIndents = 0;
    endProgram = false;
    string_literal = false;
    indent = true;
    codeBlockMode = false;
    outputStream = "";
    string filename;
    try{
        if(argc == 2){
            filename = argv[1];
            string ext = filename.substr(filename.find("."), string::npos);
            if(ext != ".py"){
                throw string("");
            }
            inputFile.open(filename.c_str());
        }else{
            throw string("");
        }
    }catch(string s){
        printf("Input: mypython <file.py>\n");
    }


    if(inputFile.is_open()){
        getChar();
        lex();
        do{
            // if(true){
            //     printf("CODEBLOCK_MODE OFF: lexing in main file\n");
            //     lex();
            // }else{
            //     printf("CODEBLOCK_MODE ON: not lex'ing with token being '%s'\n", lexeme.c_str());
            // }
            printf("Token in MAIN file loop BEFORE parsing is '%s'\n", lexeme.c_str());
            parse();
            printf("Token in MAIN file loop AFTER parsing is '%s'\n", lexeme.c_str());
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

    printf("\nPYTHON PROGRAM OUTPUT\n%s\n", outputStream.c_str());

    return 0;
}
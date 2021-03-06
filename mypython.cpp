#include "lexical.h"
#include "parser.h"

#include <iostream>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
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
int lineNumber;
string outputStream;
void getChar();
void lex();
void processSpaces();
void lookup(char);
void addChar();

void parse();

void log();
ofstream logFile;


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
    "COND_NOT_OP",
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
    lineNumber = 1;
    indentCount = 4;
    numOfIndents = 0;
    endProgram = false;
    string_literal = false;
    indent = true;
    outputStream = "";
    string filename;
    logFile.open("log_mypython.log");
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
    }catch(...){
        printf("Input: mypython <file.py>\n");
    }


    if(inputFile.is_open()){
        getChar();
        lex();
        do{
            log("Token in MAIN file loop BEFORE parsing is '%s'\n", lexeme.c_str());
            parse();
            log("Token in MAIN file loop AFTER parsing is '%s'\n", lexeme.c_str());
            if(endProgram) break;
        }while(nextToken != END);
    }

    inputFile.close();

    log("PRINTING LEXEMES\n");
    for(int i = 0; i < lexemes.size(); i++){
        if(lexemes[i] == "\t") log("indent ");
        else if(lexemes[i] == "\n") log("linebreak ");
        else log("%s ", lexemes[i].c_str());
    }
    log("\n");

    log("PRINTING TOKENS\n");
    for(int i = 0; i < tokens.size(); i++){
        log("%s ", tksnames[tokens[i]].c_str());
        //cout << tksnames[tokens[i]] << " ";
    }
    log("\n");

    //printf("\nPYTHON PROGRAM OUTPUT\n%s\n", outputStream.c_str());
    
    //outputStream contains all the output produced by the python file
    cout << outputStream << endl;

    logFile.close();

    return 0;
}

void log(const char* line, ...){
    va_list argptr;
    va_start(argptr, line);
    char buffer[1024];
    vsnprintf(buffer, 256, line, argptr);
    logFile << buffer;
    memset(buffer, 0, sizeof(buffer));
    va_end(argptr);
}
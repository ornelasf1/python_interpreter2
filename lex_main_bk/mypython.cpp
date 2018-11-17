#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
#include <vector>
#include "lexical.h"

using namespace std;

int main(int argc, char** argv){
    Lexical lex;
    lex.indentCount = 4;

    string filename;
    try{
        if(argc == 2){
            filename = argv[1];
            string ext = filename.substr(filename.find("."), string::npos);
            if(ext != ".py"){
                throw;
            }
            lex.inputFile.open(filename.c_str());
        }else{
            throw;
        }
    }catch(...){
        printf("Input: mypython <file.py>");
    }


    if(lex.inputFile.is_open()){
        lex.getChar();
        do{
            lex.lex();
        }while(lex.token != END);
    }

    lex.inputFile.close();

    printf("PRINTING LEXEMES\n");
    for(int i = 0; i < lex.lexemes.size(); i++){
        if(lex.lexemes[i] == "\t") cout << "indent ";
        else if(lex.lexemes[i] == "\n") cout << "linebreak ";
        else cout << lex.lexemes[i] << " ";
    }
    cout << endl;

    return 0;
}
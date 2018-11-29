#include <fstream>
#include <iostream>
#include "lexical.h"

using namespace std;

void getChar(){
    if(inputFile.get(nextChar)){
        //cout << endl << "Retrieved '" << nextChar << "' from inputfile" << endl;
        log("\nRetrieved '%c' from inputfile\n", nextChar);
        if(string_literal){
            log("SL_Char: %c\n", nextChar);
            charClass = LETTER;
            if(nextChar == '"'){
                log("Toggle string literal OFF\n");
                string_literal = false;
                charClass = OTHER;
            }
        }else if(isalpha(nextChar)){
            log("Char: %c\n", nextChar);
            charClass = LETTER;
        }else if(isdigit(nextChar)){
            log("Dig: %c\n", nextChar);
            charClass = DIGIT;
        }else{
            if(nextChar == '\n') log("Other: \\n\n"); 
            else if(nextChar == ' ') log("Other: space_char\n");
            else log("Other: %c\n", nextChar);
            charClass = OTHER;
            if(nextChar == '"'){
                string_literal = true;
                log("Toggle string literal ON\n");
            }
            // if(string_literal && nextChar == ' '){
            //     log("Make space into letter class\n");
            //     charClass = LETTER;
            // }
        }
    }else{
        log("EOF\n");
        nextChar = '\0';
        charClass = CHAR_END;
        //nextToken = END;
    }
}
void lex(){
    log("Entering lex() with '%s'\n", lexeme.c_str());
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
        log("Looking up '%c'\n", nextChar);
        lookup(nextChar);
        if(nextToken == ASSIGN_OP){
            getChar();
            if(nextChar == '='){
                nextToken = COND_EQUAL;
                addChar();
            }else{
                inputFile.putback(nextChar);
            }
        }else if(nextToken == COND_LT){
            getChar();
            if(nextChar == '='){
                nextToken = COND_LTEQ;
                addChar();
            }else{
                inputFile.putback(nextChar);
            }
        }else if(nextToken == COND_GT){
            getChar();
            if(nextChar == '='){
                nextToken = COND_GTEQ;
                addChar();
            }else{
                inputFile.putback(nextChar);
            }
        }else if(nextToken == COND_NOT_OP){
            getChar();
            if(nextChar == '='){
                nextToken = COND_NOT_OP;
                addChar();
            }else{
                inputFile.putback(nextChar);
            }
        }
        getChar();
        break;
    default:
        log("WE END\n");
        nextToken = END;
        break;
    }
    log("Resulted in TOKEN: %i\n", nextToken);
    log("%s -> %s\n", oldLex.c_str(), lexeme.c_str());

    if (lexeme == "\n"){
        log("Found lexeme linebreak '%s'\n", lexeme.c_str());
        //cout << "Found lexeme linebreak '" << lexeme << "'" << endl << endl;
    }else if (lexeme == "\t"){
        log("Found lexeme indent '%s'\n", lexeme.c_str());
        //cout << "Found lexeme indent '" << lexeme << "'" << endl << endl;
    }else{
        log("Found lexeme '%s'\n", lexeme.c_str());
        //cout << "Found lexeme '" << lexeme << "'" << endl << endl;
    }
    tokens.push_back(nextToken);
    lexemes.push_back(lexeme);
}

void processSpaces(){
    if(nextChar == '\n') log("Enter processSpaces() with 'line break'\n");
    else log("Enter processSpaces() with '%c'\n", nextChar);
    int spaces = indentCount;
    if(nextChar == '\n') {
        log("Reset numOfIndets to 0 and turn ON indent mode\n");
        numOfIndents = 0;
        indent = true;
    }else if(!isspace(nextChar)){
        log("Indent mode turned OFF\n");
        indent = false;
    }
    while(isspace(nextChar) && nextChar != '\n' && !string_literal){
        string msg = (indent)? "indent is true" : "indent is false";
        log("%i and %s\n", spaces, msg.c_str());
        //cout << spaces << " and " << (indent? "indent is true" : "indent is false") << endl;
        if(indent && --spaces == 0){
            charClass = OTHER;
            nextChar = '\t';
            break;
        }
        log("Skip whitespace\n");
        getChar();
    }
}
void lookup(char c){
    switch(c){
        case '=':
            addChar();
            nextToken = ASSIGN_OP;
            break;
        case '>':
            addChar();
            nextToken = COND_GT;
            break;
        case '<':
            addChar();
            nextToken = COND_LT;
            break;
        case '!':
            addChar();
            nextToken = COND_NOT_OP;
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
            lineNumber++;
            break;
        default:
            nextToken = END;
            log("WE END\n");
            break;

    }
}

void addChar(){
    if(isspace(nextChar) && nextChar != '\n' && nextChar != '\t') log("Add space\n");//cout << "Add space" << endl;
    else log("Added '%c' to lexeme\n", nextChar);//cout << "Added '" << nextChar << "' to lexeme" << endl;
    lexeme += nextChar;
};

#include <fstream>
#include <iostream>
#include "lexical.h"

using namespace std;

void getChar(){
    if(inputFile.get(nextChar)){
        cout << endl << "Retrieved '" << nextChar << "' from inputfile" << endl;
        if(string_literal){
            printf("SL_Char: %c\n", nextChar);
            charClass = LETTER;
            if(nextChar == '"'){
                printf("Toggle string literal OFF\n");
                string_literal = false;
                charClass = OTHER;
            }
        }else if(isalpha(nextChar)){
            printf("Char: %c\n", nextChar);
            charClass = LETTER;
        }else if(isdigit(nextChar)){
            printf("Dig: %c\n", nextChar);
            charClass = DIGIT;
        }else{
            if(nextChar == '\n') printf("Other: \\n\n"); 
            else if(nextChar == ' ') printf("Other: space_char\n");
            else printf("Other: %c\n", nextChar);
            charClass = OTHER;
            if(nextChar == '"'){
                string_literal = true;
                printf("Toggle string literal ON\n");
            }
            // if(string_literal && nextChar == ' '){
            //     printf("Make space into letter class\n");
            //     charClass = LETTER;
            // }
        }
    }else{
        printf("EOF\n");
        charClass = CHAR_END;
        nextToken = END;
    }
}
void lex(){
    printf("Entering lex() with '%s'\n", lexeme.c_str());
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
        printf("Looking up '%c'\n", nextChar);
        lookup(nextChar);
        if(nextToken == ASSIGN_OP){
            getChar();
            if(nextChar == '='){
                nextToken = COND_EQUAL;
                addChar();
            }
        }else if(nextToken == COND_LT){
            getChar();
            if(nextChar == '='){
                nextToken = COND_LTEQ;
                addChar();
            }
        }else if(nextToken == COND_GT){
            getChar();
            if(nextChar == '='){
                nextToken = COND_GTEQ;
                addChar();
            }
        }
        getChar();
        break;
    default:
        nextToken = END;
    }
    cout << oldLex << " -> " << lexeme << endl;

    if (lexeme == "\n"){
        cout << "Found lexeme linebreak '" << lexeme << "'" << endl << endl;
    }else if (lexeme == "\t"){
        cout << "Found lexeme indent '" << lexeme << "'" << endl << endl;
    }else{
        cout << "Found lexeme '" << lexeme << "'" << endl << endl;
    }
    tokens.push_back(nextToken);
    lexemes.push_back(lexeme);
}

void processSpaces(){
    if(nextChar == '\n') printf("Enter processSpaces() with 'line break'\n");
    else printf("Enter processSpaces() with '%c'\n", nextChar);
    int spaces = indentCount;
    if(nextChar == '\n') indent = true;
    else if(!isspace(nextChar)) indent = false;
    while(isspace(nextChar) && nextChar != '\n' && !string_literal){
        cout << spaces << " and " << (indent? "indent is true" : "indent is false") << endl;
        if(indent && --spaces == 0){
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
        case '>':
            addChar();
            nextToken = COND_GT;
            break;
        case '<':
            addChar();
            nextToken = COND_LT;
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
    else cout << "Added '" << nextChar << "' to lexeme" << endl;
    lexeme += nextChar;
};

#include <memory>
#include "lexical.h"
#include "parser.h"
#include <string>
#include <vector>

/** 
 *<line> -> print ( " {<identifier>} " )
 */
void printRule();

/**
 *<line> -> def <identifier> ( ) : {<linebreak> <indent> <line>}
 */
void functionRule();

/**
 * <line> -> <identifier> = <identifier>
 *          | <identifier> = " <identifier> "
 *          | <identifier> = <expr>
 * @author: me
 */
void assignmentRule();

const std::string reservedKeywords[] = {"print", "def"};

//std::vector<std::unique_ptr<Variable>> userVariables;

void parse(){
    if(nextToken == IDENTIFIER){
        if(lexeme == "print"){
            printf("lexeme is 'print'\n");
            printRule();
        }else if(lexeme == "def"){
            printf("lexeme is 'def'\n");
            functionRule();
        }else{
            printf("lexeme is '%s'\n", lexeme.c_str());
            assignmentRule();
        }
    }
}

void printRule(){
    printf("Print rule entered\n");
    std::string printString = "";
    printf("Testing lexeme: %s\n", lexeme.c_str());
    try{
        lex();
        if (nextToken == LEFT_PAREN){
            printf("lexeme is '('\n");
            lex();
            if (nextToken == STRING_QUOTE){
                printf("lexeme is '\"'\n");

                lex();
                while(nextToken != STRING_QUOTE){
                    printString += (lexeme + " ");
                    lex();
                }

                if(nextToken == STRING_QUOTE){
                    lex();
                    if(nextToken == RIGHT_PAREN){
                        printf("print works: %s\n", printString.c_str());
                        lex();
                        if(nextToken == LINEBREAK){
                        }else{
                            throw std::string("Syntax Error: Expected \\n after )");
                        }
                    }else{
                        throw std::string("Syntax Error: Expected ) after \"");
                    }
                }else{
                    throw std::string("Syntax Error: Expected \" after string literal");
                }
            }else{
                throw std::string("Syntax Error: Expected \" after (");
            }
        }else{
            throw std::string("Syntax Error: Expected ( after 'print'");
        }
    }catch(std::string s){
        printf("%s\n", s.c_str());
        endProgram = true;
    }
}

void assignmentRule(){
    printf("Assignment rule entered\n");
    std::string variableName = lexeme;
    std::string stringValue = "";
    int exprValue;
    try{
        lex();
        if(nextToken == ASSIGN_OP){
            lex();
            if(nextToken == STRING_QUOTE){
                //String
                lex();
                while(nextToken != STRING_QUOTE){
                    stringValue += (lexeme + " ");
                    lex();
                }
                if(nextToken == LINEBREAK){
                    //Assign string to variable
                }else{
                    throw std::string("Syntax Error: Expected \\n after \"\n");
                }

            }else if(nextToken == IDENTIFIER){
                //Integer or String

            }else{
                //Integer
                //Expression
            }
        }else{
            throw std::string("Syntax Error: Expected = after identifier\n");
        }

        //userVariables.push_back(variable);


    }catch(std::string s){
        printf("%s\n", s.c_str());
    }
}


void functionRule(){
    printf("Function rule entered\n");

}

// template<class T>
// class Variable{
//     Variable(std::string, T value);
//     std::string name;
//     T value;
// }

template <class T>
Variable<T>::Variable(std::string name){
    this.name = name;
}

template <class T>
Variable<T>::Variable(std::string name, T value){
    this.name = name;
    this.value = value;
}

template <class T>
void Variable<T>::setValue(T value){
    this.value = value;
}
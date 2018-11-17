#include <memory>
#include "lexical.h"
#include "parser.h"
#include <string>
#include <vector>

using std::string;

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
 */
void assignmentRule();

/**
 * <expr> -> <term> {(+|-)<term>}
 * <term> -> <factor> {(*|/)<factor>}
 * <factor> -> <variable> | integer | ( <expr> ) | " <identifier> "
 */
string expressionRule();
string termRule();
string factorRule();

string evaluate(string opd1, const char* op, string opd2);
string* variableLookup(string var);
bool is_number(const string&);

const string reservedKeywords[] = {"print", "def"};

std::vector<Variable*> userVariables;

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
    string printString = "";
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
                            throw string("Syntax Error: Expected \\n after )");
                        }
                    }else{
                        throw string("Syntax Error: Expected ) after \"");
                    }
                }else{
                    throw string("Syntax Error: Expected \" after string literal");
                }
            }else{
                throw string("Syntax Error: Expected \" after (");
            }
        }else{
            throw string("Syntax Error: Expected ( after 'print'");
        }
    }catch(string s){
        printf("%s\n", s.c_str());
        endProgram = true;
    }
}

void assignmentRule(){
    printf("Assignment rule entered\n");
    string variableName = lexeme;
    string stringValue = "";
    int exprValue;

    Variable* var = new Variable(lexeme);
    try{
        lex();
        if(nextToken == ASSIGN_OP){
            lex();
            var->value = expressionRule();
            printf("Value is %s\n", var->value.c_str());
        }else{
            throw string("Syntax Error: Expected = after identifier\n");
        }

        userVariables.push_back(var);


    }catch(string s){
        printf("%s\n", s.c_str());
    }
}


void functionRule(){
    printf("Function rule entered\n");

}

string expressionRule(){
    printf("Expression rule entered with %s\n", lexeme.c_str());

    try{
        //lex();
        string operand1 = termRule();
        printf("Expr: Operand 1: %s\n", operand1.c_str());
        string operand2 = "";
        string op = "";
        //lex();
        if(nextToken == ADD_OP || nextToken == SUB_OP){
            op = lexeme;
            lex();
            operand2 = termRule();
            printf("Expr: Operand 2: %s\n", operand2.c_str());
        }else if(nextToken == MULT_OP || nextToken == DIV_OP){
            lex();
            operand2 = termRule();
            printf("Expr: Operand 2: %s\n", operand2.c_str());
        }
        return evaluate(operand1, op.c_str(), operand2);
    }catch(string s){
        printf("%s\n", s.c_str());
    }
}

string termRule(){
    printf("Term rule entered with %s\n", lexeme.c_str());
    string operand1 = factorRule();
    printf("Term: Operand 1: %s\n", operand1.c_str());
    string operand2 = "";
    string op = "";
    //lex();
    if (nextToken == MULT_OP || nextToken == DIV_OP){
        op = lexeme;
        lex();
        operand2 = factorRule();
        printf("Term: Operand 2: %s\n", operand2.c_str());
    }
    if(nextToken == ADD_OP || nextToken == SUB_OP) return operand1;
    printf("Evaluating: %s %s %s\n", operand1.c_str(), op.c_str(), operand2.c_str());
    return evaluate(operand1, op.c_str(), operand2);
}

string factorRule(){
    printf("Factor rule entered with %s\n", lexeme.c_str());
    
    if(nextToken == IDENTIFIER || nextToken == INTEGER){
        string aux = lexeme;
        lex();
        return aux;
    }else if(nextToken == STRING_QUOTE){
        string stringVal = "";
        lex();
        while(nextToken != STRING_QUOTE){
            stringVal += (lexeme + " ");
            lex();
        }

    }else if(nextToken == LEFT_PAREN){
        lex();
        return expressionRule();
        if(nextToken == RIGHT_PAREN){
            lex();
        }else{
            throw string("Syntax Error: No matching )");
        }
    }
}


string evaluate(string opd1, const char* op, string opd2){
    int result;
    int operand1;
    if(is_number(opd1)){
        operand1 = atoi(opd1.c_str());
    }
    int operand2;
    if(is_number(opd2)){
        operand2 = atoi(opd2.c_str());
    }
    switch(*op){
        case '+':
            result = operand1 + operand2;
            break;
        case '-':
            result = operand1 - operand2;
            break;
        case '*':
            result = operand1 * operand2;
            break;
        case '/':
            result = operand1 / operand2;
            break;
        default:
            result = 0;
            break;
    }
    return std::to_string(result);
}

string* variableLookup(string var){
    for(Variable* v : userVariables){
        if(v->name == var){
            return new string(v->value);
        }
    }
    return NULL;
}

bool is_number(const string& s){
    string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

Variable::Variable(){}
Variable::Variable(string varName){
    name = varName;
}

Variable::Variable(string varName, string varValue){
    name = varName;
    value = varValue;
}

void Variable::setValue(string varValue){
    value = varValue;
}
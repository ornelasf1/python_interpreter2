#include <memory>
#include "lexical.h"
#include "parser.h"
#include <string>
#include <vector>
#include <stack>

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
void expressionRule();
void termRule();
void factorRule();
std::vector<string> exprTokens;
string evaluate(std::vector<string>);

//string evaluate(string opd1, const char* op, string opd2);
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
            expressionRule();
            string t = evaluate(exprTokens);
            while(!exprTokens.empty()){
                printf("%s\n", exprTokens[0].c_str());
                exprTokens.erase(exprTokens.begin());
            }
            
            //printf("Value is %s\n", var->value.c_str());
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

void expressionRule(){
    printf("Expression rule entered with %s\n", lexeme.c_str());

    try{
        termRule();
        while(nextToken == ADD_OP || nextToken == SUB_OP){
            exprTokens.push_back(lexeme);
            lex();
            termRule();
        }

    }catch(string s){
        printf("%s\n", s.c_str());
    }
}

void termRule(){
    printf("Term rule entered with %s\n", lexeme.c_str());

    factorRule();
    while(nextToken == MULT_OP || nextToken == DIV_OP){
        exprTokens.push_back(lexeme);
        lex();
        factorRule();
    }
}

void factorRule(){
    printf("Factor rule entered with %s\n", lexeme.c_str());
    
    if(nextToken == IDENTIFIER || nextToken == INTEGER){
        exprTokens.push_back(lexeme);
        lex();
    }else if(nextToken == STRING_QUOTE){
        string stringVal = "";
        lex();
        while(nextToken != STRING_QUOTE){
            stringVal += (lexeme + " ");
            lex();
        }

    }else if(nextToken == LEFT_PAREN){
        exprTokens.push_back(lexeme);
        lex();
        expressionRule();
        if(nextToken == RIGHT_PAREN){
            exprTokens.push_back(lexeme);
            lex();
        }else{
            throw string("Syntax Error: No matching )");
        }
    }
}

string evaluate(std::vector<string> s){
    string postfix = "";
    std::stack<string> pfStack;

    //Lambda function in C++11
    auto op_prec = [](const char* c){
        switch(*c){
            case '+':
                return 1;
            case '-':
                return 1;
            case '*':
                return 2;
            case '/':
                return 2;
            case '(':
                return 3;
            default:
                return 0;
        }
    };

    for(int i = 0; i < s.size(); i++){
        if(op_prec(s[i].c_str()) == 0){
            printf("%s is not an operator\n", s[i].c_str());
            postfix += s[i];
        }else{
            printf("%s is an operator\n", s[i].c_str());
            if(pfStack.empty()){
                pfStack.push(s[i]);
            }else{
                if(s[i] == ")"){
                    while(pfStack.top() != "("){
                        postfix += pfStack.top();
                        pfStack.pop();
                    }
                    pfStack.pop();
                }else if(s[i] == "("){
                    pfStack.push(s[i]);
                }else if(pfStack.top() == "("){
                    pfStack.push(s[i]);
                }else{
                    while(!pfStack.empty() && (op_prec(pfStack.top().c_str()) >= op_prec(s[i].c_str()))){
                        postfix += pfStack.top();
                        pfStack.pop();
                        pfStack.push(s[i]);
                    }
                    pfStack.push(s[i]);
                }
            }
        }
    }
    while(!pfStack.empty()){
        postfix += pfStack.top();
        pfStack.pop();
    }

    printf("Postfix: %s\n", postfix.c_str());
}

// string evaluate(string opd1, const char* op, string opd2){
//     int result;
//     int operand1;
//     if(is_number(opd1)){
//         operand1 = atoi(opd1.c_str());
//     }
//     int operand2;
//     if(is_number(opd2)){
//         operand2 = atoi(opd2.c_str());
//     }
//     switch(*op){
//         case '+':
//             result = operand1 + operand2;
//             break;
//         case '-':
//             result = operand1 - operand2;
//             break;
//         case '*':
//             result = operand1 * operand2;
//             break;
//         case '/':
//             result = operand1 / operand2;
//             break;
//         default:
//             result = 0;
//             break;
//     }
//     return std::to_string(result);
// }

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
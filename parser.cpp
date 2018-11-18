#include <memory>
#include "lexical.h"
#include "parser.h"
#include <string>
#include <vector>
#include <stack>

using std::string;

/** 
 *<line> -> print ( " {<identifier>} " )
            | print ( {<identifier>} )
            | print ( " {<identifier>} " , {<identifier>} )
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
string variableLookup(string var);
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
            do{
                lex();
                if (nextToken == STRING_QUOTE){
                    printf("lexeme is '\"'\n");

                    lex();
                    while (nextToken != STRING_QUOTE){
                        printString += (lexeme + " ");
                        lex();
                    }

                    if (nextToken == STRING_QUOTE){
                        lex();
                        printf("print works str: %s\n", printString.c_str());
                        if (nextToken == RIGHT_PAREN){
                            lex();
                            if (nextToken == LINEBREAK || nextToken == END){
                                return;
                            }
                            else{
                                throw string("Syntax Error: Expected \\n after )");
                            }
                        }else{
                            printf("str: Failed ) and moving to %s\n", lexeme.c_str());
                            //throw string("Syntax Error: Expected ) after \"");
                        }
                    }else{
                        throw string("Syntax Error: Expected \" after string literal");
                    }
                }else if (nextToken == IDENTIFIER){
                    string varValue = variableLookup(lexeme);
                    lex();
                    if (nextToken == RIGHT_PAREN){
                        printf("print works var: %s\n", varValue.c_str());
                        lex();
                        printf("%d\n", nextToken);
                        if (nextToken == LINEBREAK || nextToken == END){
                            return;
                        }else{
                            throw string("Syntax Error: Expected \\n after )");
                        }
                    }else{
                        printf("var: Failed ) and moving to %s\n", lexeme.c_str());
                        //throw string("Syntax Error: Expected ) after variable");
                    }
                }
            }while(nextToken == COMMA);
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
            var->value = evaluate(exprTokens); 
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
    std::stack<string> evaluateStack;

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
            case ')':
                return 3;
            default:
                return 0;
        }
    };

    for(int i = 0; i < s.size(); i++){
        if(op_prec(s[i].c_str()) == 0){
            printf("%s is not an operator\n", s[i].c_str());
            
            //postfix += s[i];
            evaluateStack.push(s[i]);
        }else{
            printf("%s is an operator\n", s[i].c_str());
            if(pfStack.empty()){
                pfStack.push(s[i]);
            }else{
                if(s[i] == ")"){
                    while(pfStack.top() != "("){
                        //postfix += pfStack.top();
                        evaluateStack.push(pfStack.top());
                        pfStack.pop();
                    }
                    pfStack.pop();
                }else if(s[i] == "("){
                    pfStack.push(s[i]);
                }else if(pfStack.top() == "("){
                    pfStack.push(s[i]);
                }else{
                    while(!pfStack.empty() && (op_prec(pfStack.top().c_str()) >= op_prec(s[i].c_str()))){
                        evaluateStack.push(pfStack.top());
                        //postfix += pfStack.top();
                        pfStack.pop();
                    }
                    pfStack.push(s[i]);
                }
            }

        }
    }
    while(!pfStack.empty()){
        //postfix += pfStack.top();
        evaluateStack.push(pfStack.top());
        pfStack.pop();
    }

    //Reverses stack order
    std::stack<string> aux;
    while (!evaluateStack.empty())
    {
        aux.push(evaluateStack.top());
        evaluateStack.pop();
    }
    evaluateStack = aux;

    string operand1;
    string operand2;
    string op;
    string result;
    std::stack<string> results;
    while(!evaluateStack.empty()){
        if(op_prec(evaluateStack.top().c_str()) == 0){
            results.push(evaluateStack.top());
            evaluateStack.pop();
        }else{
            printf("%s is an operator", evaluateStack.top().c_str());

            operand1 = results.top(); results.pop();
            operand2 = results.top(); results.pop();

            if(!is_number(operand1)) operand1 = variableLookup(operand1);
            if(!is_number(operand2)) operand2 = variableLookup(operand2);

            op = evaluateStack.top(); evaluateStack.pop();
            printf(" - evaluating: %s %s %s - ", operand1.c_str(), op.c_str(), operand2.c_str());

            if(op == "+"){
                result = std::to_string(atoi(operand1.c_str()) + atoi(operand2.c_str()));
            }else if(op == "-"){
                result = std::to_string(atoi(operand1.c_str()) - atoi(operand2.c_str()));
            }else if(op == "*"){
                result = std::to_string(atoi(operand1.c_str()) * atoi(operand2.c_str()));
            }else if(op == "/"){
                result = std::to_string(atoi(operand2.c_str()) / atoi(operand1.c_str()));
            }
            results.push(result);
            printf(" result is %s\n", result.c_str());
        }
    }
    printf("Result is %s\n", results.top().c_str());
    return results.top();
}

string variableLookup(string var){
    for(Variable* v : userVariables){
        if(v->name == var){
            return v->value;
        }
    }
    throw string("Variable '%s' is undefined\n", var.c_str());
    return "";
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
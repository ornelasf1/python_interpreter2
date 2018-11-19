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
 * <ifstmt> -> if ( {<boolexpr>} ) : linebreak {indent} <statement> {linebreak <statement>}
 */
void ifstmtRule();

/**
 * <boolexpr> -> <identifier> {(and|or) <identifier>}
 *              | <expr> (and|or|<|<=|>|>=|==) <expr>
 *            
 */
bool booleanExprRule();

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

const string reservedKeywords[] = {"print", "def", "if", "and", "or"};

std::vector<Variable*> userVariables;

void parse(){
    printf("Enter parse\nParsing '%s'\n", lexeme.c_str());
    if(nextToken == IDENTIFIER){
        if(lexeme == "print"){
            printf("lexeme is 'print'\n");
            printRule();
        }else if(lexeme == "def"){
            printf("lexeme is 'def'\n");
            functionRule();
        }else if(lexeme == "if"){
            printf("lexeme is 'if'\n");
            ifstmtRule();
        }else{
            printf("lexeme is '%s'\n", lexeme.c_str());
            assignmentRule();
        }
    }
}

void printRule(){
    printf("Print rule entered\n");
    string printString = "";
    try{
        lex();
        if (nextToken == LEFT_PAREN){
            do{
                lex();
                if (nextToken == STRING_QUOTE){
                    lex();

                    if(nextToken == STRING_LITERAL){
                        printString += lexeme;
                        lex();

                        if (nextToken == STRING_QUOTE){
                            lex();
                            printf("print works str: %s\n", printString.c_str());
                            if (nextToken == RIGHT_PAREN){
                                lex();
                                if (nextToken == LINEBREAK || nextToken == END){
                                    break;
                                }
                                else{
                                    throw string("Expected \\n after )");
                                }
                            }else{
                                printf("str: Failed ) and moving to %s\n", lexeme.c_str());
                                //throw string("Expected ) after \"");
                            }
                        }else{
                            throw string("Expected \" after string literal");
                        }
                    }else{
                        throw string("Expected string literal after \"");
                    }
                }else if (nextToken == IDENTIFIER){
                    string varValue = variableLookup(lexeme);
                    printString += varValue;
                    printf("print works var: %s\n", varValue.c_str());
                    lex();
                    if (nextToken == RIGHT_PAREN){
                        lex();
                        printf("%d\n", nextToken);
                        if (nextToken == LINEBREAK || nextToken == END){
                            break;
                        }else{
                            throw string("Expected \\n after )");
                        }
                    }else{
                        printf("var: Failed ) and moving to %s\n", lexeme.c_str());
                        //throw string("Expected ) after variable");
                    }
                }
            }while(nextToken == COMMA);
            printf("OUTPUT: %s\n", printString.c_str());
        }else{
            throw string("Expected ( after 'print'");
        }
    }catch(string s){
        printf("Syntax Error: %s\n", s.c_str());
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
            throw string("Expected = after identifier\n");
        }

        userVariables.push_back(var);


    }catch(string s){
        printf("Syntax Error: %s\n", s.c_str());
        endProgram = true;
    }
}


void functionRule(){
    printf("Function rule entered\n");

}

void ifstmtRule(){
    printf("If Stmt rule entered\n");
    bool condition;
    try{
        lex();
        if(nextToken == LEFT_PAREN){
            lex();
            condition = booleanExprRule();
            if(condition){
                printf("if resulted in true next lexeme is %s\n", lexeme.c_str());
                lex();
                if(nextToken == COLON){
                    lex();
                    if(nextToken == LINEBREAK){
                        lex();
                        if(nextToken == INDENT){
                            lex();
                            parse();
                        }else{
                            throw string("Expected statement block after 'if' call");
                        }
                    }else{
                        throw string("Expected statement after 'if' call");
                    }
                }else{
                    throw string("Expected ':' after 'if( )'");
                }
            }else{
                printf("if resulted in false next lexeme is %s\n", lexeme.c_str());
            }
        }else{
            throw string("Expected '(' after 'if' keyword");
        }
    }catch(string err){
        printf("Syntax Error: %s\n", err.c_str());
        endProgram = true;
    }
}

bool booleanExprRule(){
    string operand1, operand2;
    expressionRule();
    operand1 = evaluate(exprTokens);
    printf("Bool expr: operand 1 is '%s'\n", operand1.c_str());
    if(nextToken == COND_EQUAL){
        lex();
        expressionRule();
        operand2 = evaluate(exprTokens);
        printf("Bool expr: operand 2 is '%s'\n", operand2.c_str());
        if(nextToken == RIGHT_PAREN){
            return atoi(operand1.c_str()) == atoi(operand2.c_str());
        }
    }else if(nextToken == COND_GTEQ){
        lex();
        expressionRule();
        operand2 = evaluate(exprTokens);
        printf("Bool expr: operand 2 is '%s'\n", operand2.c_str());
        if(nextToken == RIGHT_PAREN){
            return atoi(operand1.c_str()) >= atoi(operand2.c_str());
        }
    }else if(nextToken == COND_GT){
        lex();
        expressionRule();
        operand2 = evaluate(exprTokens);
        printf("Bool expr: operand 2 is '%s'\n", operand2.c_str());
        if(nextToken == RIGHT_PAREN){
            return atoi(operand1.c_str()) > atoi(operand2.c_str());
        }
    }else if(nextToken == COND_LTEQ){
        lex();
        expressionRule();
        operand2 = evaluate(exprTokens);
        if(nextToken == RIGHT_PAREN){
            return atoi(operand1.c_str()) <= atoi(operand2.c_str());
        }
    }else if(nextToken == COND_LT){
        lex();
        expressionRule();
        operand2 = evaluate(exprTokens);
        if(nextToken == RIGHT_PAREN){
            return atoi(operand1.c_str()) < atoi(operand2.c_str());
        }
    }else{
        throw string("Operator not found");
        endProgram = true;
    }
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
        printf("Syntax Error: %s\n", s.c_str());
        endProgram = true;
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
            throw string("No matching )");
            endProgram = true;
        }
    }
}

/**
 * Takes vector of expression tokens and pushes them into
 * a stack 'pfStack' in the order of postfix.
 * Then pushes to 'evaluateStack' to evaluate them. Returns
 * the single element in the stack which will be the result of 
 * the expression.
 */
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
    //Deallocate elements from vector containing expression factors for next use
    exprTokens.clear();

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
            if(!is_number(evaluateStack.top())) results.push(variableLookup(evaluateStack.top()));
            else results.push(evaluateStack.top());
            evaluateStack.pop();
        }else{
            printf("%s is an operator for evaluating", evaluateStack.top().c_str());

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
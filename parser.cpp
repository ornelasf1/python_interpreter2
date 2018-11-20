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
void updateCurrentScope();
bool is_number(const string&);

const string reservedKeywords[] = {"print", "def", "if", "and", "or"};

//std::vector<Variable*> userVariables;
//std::vector< std::vector<Scope*> > scopeOfScopes;
std::vector<Scope*> scopes;
Scope* currentScope = new Scope();

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
    if(nextToken == LINEBREAK){
        printf("LINEBREAK ignored :(\n");
    }
    // if(nextToken == LINEBREAK){
    //     printf("Line break in parse function\n");
    //     lex();
    //     while(nextToken == INDENT){
    //         numOfIndents++;
    //         lex();
    //         printf("Hit INDENT, increase indent count: %i\n", numOfIndents);
    //     }
    // }
    // }else if(nextToken == INDENT){
    //     printf("Create new scope\n");
    //     numOfIndents++;
    //     currentScope->innerScope = new Scope(currentScope->scopeVariables, currentScope->scopeLevel);
    // }
}

void printRule(){
    printf("Print rule entered with scope level %i and with token '%s'\n", currentScope->scopeLevel, lexeme.c_str());

    updateCurrentScope();
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
                            lex();
                            while (nextToken == INDENT){
                                numOfIndents++;
                                printf("Hit INDENT after assignment line, increase indent count: %i\n", numOfIndents);
                                lex();
                            }
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
    printf("Print rule exitting with scope level %i and with token '%s'\n", currentScope->scopeLevel, lexeme.c_str());
}


void assignmentRule(){
    updateCurrentScope();
    printf("Assignment rule entered with scope level %i and with token '%s'\n", currentScope->scopeLevel, lexeme.c_str());
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
            // while(!exprTokens.empty()){
            //     printf("%s\n", exprTokens[0].c_str());
            //     exprTokens.erase(exprTokens.begin());
            // }
            exprTokens.clear();
            if(nextToken == LINEBREAK){
                if(currentScope->scopeLevel != 0){
                    lex();
                    while(nextToken == INDENT){
                        numOfIndents++;
                        printf("Hit INDENT after assignment line, increase indent count: %i\n", numOfIndents);
                        lex();
                    }
                }
            }else{
                throw string("Error after assignment line");
            }

            //printf("Value is %s\n", var->value.c_str());
        }else{
            throw string("Expected = after identifier");
        }
        //userVariables.push_back(var);
        currentScope->scopeVariables.push_back(var);
        printf("Assign '%s' to '%s'\n", var->value.c_str(), var->name.c_str());
        //Exits with LINEBREAK

    }catch(string s){
        printf("Syntax Error: %s\n", s.c_str());
        endProgram = true;
    }
    printf("Assignment rule exitting with scope level %i and with token '%s'\n", currentScope->scopeLevel, lexeme.c_str());
}


void functionRule(){
    printf("Function rule entered\n");

}

void ifstmtRule(){
    printf("If Stmt rule entered with scope level %i and token '%s'\n", currentScope->scopeLevel, lexeme.c_str());
    int indentsToIfStmt;
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
                    indentsToIfStmt = numOfIndents;
                    lex();
                    if(nextToken == LINEBREAK){
                        lex();
                        //TOKENS prevTok;
                        while(nextToken == INDENT){
                            //prevTok = nextToken;
                            numOfIndents++;
                            printf("Hit INDENT in ifstmt, increase indent count: %i\n", numOfIndents);
                            lex();
                        }
                        // if(prevTok == INDENT){
                        if(numOfIndents == indentsToIfStmt + 1){
                            printf("Creating new scope in 'if' call\n");
                            Scope* if_Scope = new Scope(currentScope->scopeVariables, currentScope->scopeLevel);
                            currentScope->innerScope = if_Scope;
                            if_Scope->outerScope = currentScope;
                            do{
                                printf("IFSTMT: Looping with %s and with scope level %i\n", lexeme.c_str(), currentScope->scopeLevel);
                                parse();
                            }while(numOfIndents == indentsToIfStmt + 1);
                            printf("IFSTMT: EXITTING loop with %s and with scope level %i\n", lexeme.c_str(), currentScope->scopeLevel);
                        }else{
                            throw string("Expected statement block after 'if' call");
                        }
                        // if(nextToken == INDENT){
                        //     numOfIndents++;
                        //     printf("Create new scope in 'if' call\n");
                        //     currentScope->innerScope = new Scope(currentScope->scopeVariables, currentScope->scopeLevel);
                        //     lex();
                        //     parse();
                        // }else{
                        //     throw string("Expected statement block after 'if' call");
                        // }
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
    printf("If Stmt rule exitted with scope level %i and token '%s'\n", currentScope->scopeLevel, lexeme.c_str());
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
    printf("Searching for variable %s in scope level %i\n", var.c_str(), currentScope->scopeLevel);
    printf("Variables in scope level %i:\n", currentScope->scopeLevel);
    for(Variable* v : currentScope->scopeVariables){
        printf("Var: %s\n", v->name.c_str());
    }
    printf("\n");
    string latestVarValue = "";;
    for(Variable* v : currentScope->scopeVariables){
        if(v->name == var){
            latestVarValue = v->value;
        }
    }
    if(latestVarValue != "") return latestVarValue;
    else throw string("Variable '%s' is undefined\n", var.c_str());
    return "";
}

void updateCurrentScope(){
    if(numOfIndents > currentScope->scopeLevel){
        printf("%i > %i : Traverse into inner scope\n", numOfIndents, currentScope->scopeLevel);
        while(numOfIndents > currentScope->scopeLevel && currentScope->innerScope != NULL){
            currentScope = currentScope->innerScope;
            printf("Entered inner scope with level %i\n", currentScope->scopeLevel);
        }
    }
    else if(numOfIndents < currentScope->scopeLevel){
        printf("%i < %i : Traverse into outer scope\n", numOfIndents, currentScope->scopeLevel);
        while(numOfIndents < currentScope->scopeLevel && currentScope->outerScope != NULL){
            currentScope = currentScope->outerScope;
            printf("Entered outer scope with level %i\n", currentScope->scopeLevel);
        }
    }
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

//int Scope::scopeLevel = -1;

Scope::Scope(){
    this->scopeLevel = 0;
    this->innerScope = NULL;
    this->outerScope = NULL;
}

Scope::Scope(std::vector<Variable*> parentVars, int new_scopeL){
    this->scopeLevel = new_scopeL + 1;
    this->scopeVariables = parentVars;
    this->innerScope = NULL;
}

void Scope::traverseToScope(int i){
    // if(i > currentScope->scopeLevel){
    //     while(i > currentScope->scopeLevel && currentScope->innerScope != NULL){
    //         currentScope = currentScope->innerScope;
    //     }
    // }else if(i <= currentScope->scopeLevel){
    //     while(i <= currentScope->scopeLevel && currentScope->innerScope != NULL){
    //         currentScope = currentScope->outerScope;
    //     }
    // }
}
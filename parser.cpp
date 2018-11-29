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
 * <line> -> <identifier> ( )
 */
void functionCallRule(string);

/**
 * <line> -> <identifier> = <identifier>
 *          | <identifier> = " <identifier> "
 *          | <identifier> = <expr>
 */
void assignmentRule(string);

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
void consumeIndents();
void updateCurrentScope();
bool is_number(const string&);
void assignFunctionValue();

const string reservedKeywords[] = {"print", "def", "if", "return", "and", "or"};

//std::vector<Variable*> userVariables;
//std::vector< std::vector<Scope*> > scopeOfScopes;
std::vector<Scope*> scopes;
Scope* currentScope = new Scope();

void printVariables(string name, std::vector<Variable*> vec, Scope* sc){
    log("Printing variables of %s in scope level %i:\n", name.c_str(), sc->scopeLevel);
    string aux;
    for(int i = 0; i < vec.size(); i++){
        aux = vec[i]->isFunction? "Yes" : "No";
        log("  Name: %s | Value: %s | isFunction: %s\n", vec[i]->name.c_str(), vec[i]->value.c_str(), aux.c_str());
    }
    log("End printing variables of %s\n", name.c_str());
}

void parse(){
    log("Enter parse\nParsing '%s'\n", lexeme.c_str());
    if(nextToken == IDENTIFIER){
        //updateCurrentScope();
        if(currentScope->fetchFunction(lexeme) != NULL){

        }if(lexeme == "print"){
            log("Producing Print Rule with lexeme '%s' and scope level %i\n", lexeme.c_str(), currentScope->scopeLevel);
            printRule();
        }else if(lexeme == "def"){
            log("Producing Function Rule with lexeme '%s' and scope level %i\n", lexeme.c_str(), currentScope->scopeLevel);
            functionRule();
        }else if(lexeme == "if"){
            log("Producing IfStmt Rule with lexeme '%s' and scope level %i\n", lexeme.c_str(), currentScope->scopeLevel);
            ifstmtRule();
        }else if(lexeme == "return"){
            log("Setting return value for func '%s' with lexeme '%s' and scope level %i\n", currentScope->actingFunctionName.c_str(), lexeme.c_str(), currentScope->scopeLevel);
            assignFunctionValue();
        }else{
            string id = lexeme;
            lex();
            if(nextToken == ASSIGN_OP){
                log("Producing Assignment Rule with lexeme '%s' and scope level %i\n", lexeme.c_str(), currentScope->scopeLevel);
                assignmentRule(id);
            }else if(nextToken == LEFT_PAREN){
                log("Producing Function Call Rule with lexeme '%s' and scope level %i\n", lexeme.c_str(), currentScope->scopeLevel);
                functionCallRule(id);
            }
        }
    }else if(nextToken == LINEBREAK){
        log("LINEBREAK ignored :(\n");
        lex();
    }else{
        log("Parse doesn't recognize token '%s' and scope level '%i'\n", lexeme.c_str(), currentScope->scopeLevel);
        lex();
    }
}

void printRule(){
    log("Print rule entered with scope level %i and with token '%s'\n", currentScope->scopeLevel, lexeme.c_str());

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
                            log("print works str: %s\n", printString.c_str());
                            if (nextToken == RIGHT_PAREN){
                                lex();
                                if (nextToken == LINEBREAK || nextToken == END){
                                    lex();
                                    consumeIndents();
                                    break;
                                }
                                else{
                                    throw string("Expected \\n after )");
                                }
                            }else{
                                log("str: Failed ) and moving to %s\n", lexeme.c_str());
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
                    log("print works var: %s\n", printString.c_str());
                    lex();
                    if (nextToken == RIGHT_PAREN){
                        lex();
                        log("%d\n", nextToken);
                        if (nextToken == LINEBREAK || nextToken == END){
                            lex();
                            consumeIndents();
                            break;
                        }else{
                            throw string("Expected \\n after )");
                        }
                    }else{
                        log("var: Failed ) and moving to %s\n", lexeme.c_str());
                    }
                }else if(nextToken == INTEGER){
                    printString += lexeme;
                    log("print works integer: %s\n", printString.c_str());
                    lex();
                    if (nextToken == RIGHT_PAREN){
                        lex();
                        log("%d\n", nextToken);
                        if (nextToken == LINEBREAK || nextToken == END){
                            lex();
                            consumeIndents();
                            break;
                        }else{
                            throw string("Expected \\n after )");
                        }
                    }else{
                        log("var: Failed ) and moving to %s\n", lexeme.c_str());
                    }
                }
            }while(nextToken == COMMA);
            log("OUTPUT: %s\n", printString.c_str());
        }else{
            throw string("Expected ( after 'print'");
        }


        Scope* temp = currentScope->getFunctionScope();
        if(temp->functionMode){
            printVariables(temp->actingFunctionName, temp->scopeVariables, temp);
            printVariables(temp->outerScope->actingFunctionName, temp->outerScope->scopeVariables, temp->outerScope);
            log("Print rule assigning string to scope with function name '%s' back to scope level %i\n", temp->actingFunctionName.c_str(), temp->outerScope->scopeLevel);
            temp->outerScope->fetchFunction(temp->actingFunctionName)->printCalls += (printString + "\n");
        }else{
            outputStream += (printString + "\n");
        }
        log("Print rule exitting with scope level %i and with token '%s'\n", currentScope->scopeLevel, lexeme.c_str());
    }catch(string s){
        log("Syntax Error in line %i: %s\n", lineNumber, s.c_str());
        outputStream += ("\nSyntax Error in line " + std::to_string(lineNumber) + ": " + s);
        endProgram = true;
    }
}


void assignmentRule(string varName){
    updateCurrentScope();
    log("Assignment rule entered with scope level %i and with token '%s'\n", currentScope->scopeLevel, lexeme.c_str());
    string variableName = varName;
    string stringValue = "";
    int exprValue;

    Variable* var = new Variable(variableName);
    try{
        lex();
        expressionRule();
        var->value = evaluate(exprTokens);
        exprTokens.clear();
        if(nextToken == LINEBREAK){
            //if(currentScope->scopeLevel != 0){
            lex();
            consumeIndents();
        }else{
            throw string("Error after assignment line");
        }

        //log("Value is %s\n", var->value.c_str());
        //userVariables.push_back(var);
        currentScope->scopeVariables.push_back(var);
        log("Assign '%s' to '%s'\n", var->value.c_str(), var->name.c_str());
        //Exits with LINEBREAK

    }catch(string s){
        log("Syntax Error in line %i: %s\n", lineNumber, s.c_str());
        outputStream += ("\nSyntax Error in line " + std::to_string(lineNumber) + ": " + s);
        endProgram = true;
    }
    log("Assignment rule exitting with scope level %i and with token '%s'\n", currentScope->scopeLevel, lexeme.c_str());
}


void functionRule(){
    updateCurrentScope();
    log("Function rule entered\n");
    int indentsToDefFunc;
    string functionName;
    Function* func = new Function();
    try{
        lex();
        if(nextToken == IDENTIFIER){
            functionName = lexeme;
            func->name = lexeme;
            currentScope->scopeVariables.push_back(func);
            lex();
            if(nextToken == LEFT_PAREN){
                //Parameters implementation
                lex();
                if(nextToken == RIGHT_PAREN){
                    lex();
                    if(nextToken == COLON){
                        indentsToDefFunc = numOfIndents;
                        lex();
                        if(nextToken == LINEBREAK){
                            lex();
                            consumeIndents();
                            if(numOfIndents == indentsToDefFunc + 1){
                                log("Creating new scope in 'def' call\n");
                                Scope* def_Scope = new Scope(currentScope->scopeVariables, currentScope->scopeLevel);
                                currentScope->innerScope = def_Scope;
                                def_Scope->outerScope = currentScope;
                                updateCurrentScope();
                                currentScope->functionMode = true;
                                currentScope->actingFunctionName = functionName;
                                do{
                                    if(nextToken == INDENT) consumeIndents();
                                    log("DEF_FUNC: Looping with %s and with scope level %i\n", lexeme.c_str(), currentScope->scopeLevel);
                                    parse();
                                    log("DEF_FUNC: Back to function loop with '%s' and no. of indents: '%i'\n", lexeme.c_str(), numOfIndents);
                                }while(numOfIndents == indentsToDefFunc + 1 || nextToken == LINEBREAK || nextToken == INDENT);
                                log("DEF_FUNC: EXITTING loop with %s and with scope level %i\n", lexeme.c_str(), currentScope->scopeLevel);
                                currentScope->functionMode = false;
                            }else{
                                throw string("Expected statement block after function decleration");
                            }
                        }else{
                            throw string("Expected line break after ':'");
                        }
                    }else{
                        throw string("Expected ':' after parentheses");
                    }
                }else{
                    throw string("Expected closing parentheses to '('");
                }
            }else{
                throw string("Expected '(' after function name");
            }
        }else{
            throw string("Expected function name after 'def' keyword");
        }
    }catch(string err){
        log("Syntax Error in line %i: %s\n", lineNumber, err.c_str());
        outputStream += ("\nSyntax Error in line " + std::to_string(lineNumber) + ": " + err);
        endProgram = true;
    }
}

void functionCallRule(string funcName){
    log("Function CALL rule entered with scope level %i and token '%s'\n", currentScope->scopeLevel, lexeme.c_str());
    try{
        lex();
        if(nextToken == RIGHT_PAREN){
            log("Function call in scope level %i\n", currentScope->scopeLevel);
            outputStream += currentScope->fetchFunction(funcName)->printCalls;
            lex();
            if (nextToken == LINEBREAK || nextToken == END){
                lex();
                consumeIndents();
                return;
            }else{
                throw string("Expected \\n after )");
            }
        }else{
            throw string("Expected closing parentheses to '('");
        }
    }catch(string err){
        log("Syntax Error in line %i: %s\n", lineNumber, err.c_str());
        outputStream += ("\nSyntax Error in line " + std::to_string(lineNumber) + ": " + err);
        endProgram = true;
    }
    log("Function CALL rule exitted with scope level %i and token '%s'\n", currentScope->scopeLevel, lexeme.c_str());
}

void assignFunctionValue(){
    try{
        updateCurrentScope();
        lex();
        if(nextToken == IDENTIFIER || nextToken == INTEGER){
            string varValue = (nextToken == IDENTIFIER)? variableLookup(lexeme) : lexeme;
            log("Setting value of function '%s' to '%s' in scope level %i\n", currentScope->actingFunctionName.c_str(), varValue.c_str(), currentScope->scopeLevel);
            currentScope->outerScope->fetchFunction(currentScope->actingFunctionName.c_str())->value = varValue;
            lex();
            if (nextToken == LINEBREAK || nextToken == END){
                lex();
                consumeIndents();
                return;
            }else{
                throw string("Expected \\n after the identifier");
            }
        }else{
            throw string("Expected identifier after return keyword");
        }
    }catch(string err){
        log("Syntax Error in line %i: %s\n", lineNumber, err.c_str());
        outputStream += ("\nSyntax Error in line " + std::to_string(lineNumber) + ": " + err);
        endProgram = true;
    }
}

void ifstmtRule(){
    log("If Stmt rule entered with scope level %i and token '%s'\n", currentScope->scopeLevel, lexeme.c_str());
    int indentsToIfStmt;
    bool condition;
    try{
        lex();
        if(nextToken == LEFT_PAREN || nextToken == IDENTIFIER || nextToken == INTEGER){
            bool left_parenthesis = false;
            if(nextToken == LEFT_PAREN){ lex(); left_parenthesis = true; }
            condition = booleanExprRule();
            if(condition){
                log("if resulted in TRUE next lexeme is %s\n", lexeme.c_str());
                if(left_parenthesis) lex();
                if(nextToken == COLON){
                    indentsToIfStmt = numOfIndents;
                    lex();
                    if(nextToken == LINEBREAK){
                        lex();
                        consumeIndents();
                        if(numOfIndents == indentsToIfStmt + 1){
                            log("Creating new scope in 'if' call\n");
                            Scope* if_Scope = new Scope(currentScope->scopeVariables, currentScope->scopeLevel);
                            currentScope->innerScope = if_Scope;
                            if_Scope->outerScope = currentScope;
                            do{
                                if(nextToken == INDENT) consumeIndents();
                                log("IFSTMT: Looping with %s and with scope level %i\n", lexeme.c_str(), currentScope->scopeLevel);
                                parse();
                            }while(numOfIndents == indentsToIfStmt + 1 || nextToken == LINEBREAK || nextToken == INDENT);
                            log("IFSTMT: EXITTING loop with %s and with scope level %i\n", lexeme.c_str(), currentScope->scopeLevel);
                            currentScope->outerScope->scopeVariables = currentScope->scopeVariables;

                            if(numOfIndents == indentsToIfStmt && nextToken == IDENTIFIER && lexeme == "else"){
                                log("TRUE: Skipping else code block\n");
                                do{
                                    log("IFSTMT TRUE SKIP ELSE: Looping with %s and with scope level %i\n", lexeme.c_str(), currentScope->scopeLevel);
                                    while(nextToken != LINEBREAK){
                                        log("IFSTMT TRUE SKIP ELSE: lex is %s : Skipping code block statements\n", lexeme.c_str());
                                        lex();
                                        if(nextToken == END) return;
                                    }
                                    lex();
                                    consumeIndents();
                                }while(numOfIndents == indentsToIfStmt + 1);
                            }
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
                log("if resulted in FALSE next lexeme is %s\n", lexeme.c_str());
                if(left_parenthesis) lex();
                if(nextToken == COLON){
                    indentsToIfStmt = numOfIndents;
                    lex();
                    if(nextToken == LINEBREAK){
                        lex();
                        consumeIndents();
                        if(numOfIndents == indentsToIfStmt + 1){
                            log("FALSE: Not Creating new scope in 'if' call\n");
                            do{
                                log("IFSTMT FALSE: Looping with %s and with scope level %i\n", lexeme.c_str(), currentScope->scopeLevel);
                                while(nextToken != LINEBREAK){
                                    log("IFSTMT FALSE: lex is %s : Skipping code block statements\n", lexeme.c_str());
                                    lex();
                                    if(nextToken == END) return;
                                }
                                lex();
                                consumeIndents();
                            }while(numOfIndents == indentsToIfStmt + 1);

                            if(numOfIndents == indentsToIfStmt && nextToken == IDENTIFIER && lexeme == "else"){
                                lex();
                                if(nextToken == COLON){
                                    lex();
                                    if(nextToken == LINEBREAK){
                                        lex();
                                        consumeIndents();
                                        if(numOfIndents == indentsToIfStmt + 1){
                                            log("Creating new scope in 'else' call\n");
                                            Scope* if_Scope = new Scope(currentScope->scopeVariables, currentScope->scopeLevel);
                                            currentScope->innerScope = if_Scope;
                                            if_Scope->outerScope = currentScope;
                                            do{
                                                if(nextToken == INDENT) consumeIndents();
                                                log("ELSESTMT: Looping with %s and with scope level %i\n", lexeme.c_str(), currentScope->scopeLevel);
                                                parse();
                                                if(nextToken == END) return;
                                            }while(numOfIndents == indentsToIfStmt + 1 || nextToken == LINEBREAK || nextToken == INDENT);
                                            log("ELSESTMT: EXITTING loop with %s and with scope level %i\n", lexeme.c_str(), currentScope->scopeLevel);
                                            currentScope->outerScope->scopeVariables = currentScope->scopeVariables;
                                        }else{
                                            throw string("Expected statement block after 'else' call");
                                        }
                                    }else{
                                        throw string("Expected statement after 'else' call");
                                    }
                                }
                            }
                            log("IFSTMT FALSE: EXITTING loop with %s and with scope level %i\n", lexeme.c_str(), currentScope->scopeLevel);
                        }else{
                            throw string("Expected statement block after 'if' call");
                        }
                    }else{
                        throw string("Expected statement after 'if' call");
                    }
                }else{
                    throw string("Expected ':' after 'if( )'");
                }
            }
        }else{
            throw string("Expected '(' after 'if' keyword");
        }
    }catch(string err){
        log("Syntax Error in line %i: %s\n", lineNumber, err.c_str());
        outputStream += ("\nSyntax Error in line " + std::to_string(lineNumber) + ": " + err);
        endProgram = true;
    }
    log("If Stmt rule exitted with scope level %i and token '%s'\n", currentScope->scopeLevel, lexeme.c_str());
}

bool booleanExprRule(){
    string operand1, operand2;
    expressionRule();
    if(lexeme == "True"){
        lex();
        return true;
    }else if(lexeme == "False"){
        lex();
        return false;
    }
    operand1 = evaluate(exprTokens);
    log("Bool expr: operand 1 is '%s'\n", operand1.c_str());
    if(nextToken == COND_EQUAL || nextToken == COND_NOT_OP){
        int cond_operator = nextToken;
        lex();
        expressionRule();
        operand2 = evaluate(exprTokens);
        log("Bool expr: operand 2 is '%s'\n", operand2.c_str());
        if(nextToken == RIGHT_PAREN || nextToken == COLON){
            log("Compare %s equivalence to %s\n", operand1.c_str(), operand2.c_str());
            if(cond_operator == COND_EQUAL) return atoi(operand1.c_str()) == atoi(operand2.c_str());
            else if(cond_operator == COND_NOT_OP) return atoi(operand1.c_str()) != atoi(operand2.c_str());
        }else{
            throw string("Enclosing characters for condition missing");
        }
    }else if(nextToken == COND_GTEQ){
        lex();
        expressionRule();
        operand2 = evaluate(exprTokens);
        log("Bool expr: operand 2 is '%s'\n", operand2.c_str());
        if(nextToken == RIGHT_PAREN || nextToken == COLON){
            return atoi(operand1.c_str()) >= atoi(operand2.c_str());
        }else{
            throw string("Enclosing characters for condition missing");
        }
    }else if(nextToken == COND_GT){
        lex();
        expressionRule();
        operand2 = evaluate(exprTokens);
        log("Bool expr: operand 2 is '%s'\n", operand2.c_str());
        if(nextToken == RIGHT_PAREN || nextToken == COLON){
            return atoi(operand1.c_str()) > atoi(operand2.c_str());
        }else{
            throw string("Enclosing characters for condition missing");
        }
    }else if(nextToken == COND_LTEQ){
        lex();
        expressionRule();
        operand2 = evaluate(exprTokens);
        if(nextToken == RIGHT_PAREN || nextToken == COLON){
            return atoi(operand1.c_str()) <= atoi(operand2.c_str());
        }else{
            throw string("Enclosing characters for condition missing");
        }
    }else if(nextToken == COND_LT){
        lex();
        expressionRule();
        operand2 = evaluate(exprTokens);
        if(nextToken == RIGHT_PAREN || nextToken == COLON){
            return atoi(operand1.c_str()) < atoi(operand2.c_str());
        }else{
            throw string("Enclosing characters for condition missing");
        }
    }else{
        throw string("Operator '" + lexeme + "' not found");
        endProgram = true;
    }
}

void expressionRule(){
    log("Expression rule entered with '%s'\n", lexeme.c_str());

    try{
        termRule();
        while(nextToken == ADD_OP || nextToken == SUB_OP){
            exprTokens.push_back(lexeme);
            lex();
            termRule();
        }
    }catch(string s){
        log("Syntax Error in line %i: %s\n", lineNumber, s.c_str());
        outputStream += ("\nSyntax Error in line " + std::to_string(lineNumber) + ": " + s);
        endProgram = true;
    }
}

void termRule(){
    log("Term rule entered with '%s'\n", lexeme.c_str());

    factorRule();
    while(nextToken == MULT_OP || nextToken == DIV_OP){
        exprTokens.push_back(lexeme);
        lex();
        factorRule();
    }
}

void factorRule(){
    log("Factor rule entered with '%s'\n", lexeme.c_str());
    
    if(nextToken == IDENTIFIER || nextToken == INTEGER){
        if(lexeme == "True" || lexeme == "False"){
            return;
        }else{
            exprTokens.push_back(lexeme);
            lex();
            if(nextToken == LEFT_PAREN){
                lex();
                if(nextToken == RIGHT_PAREN){
                    lex();
                }
            }
        }
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
            log("%s is not an operator\n", s[i].c_str());
            
            //postfix += s[i];
            evaluateStack.push(s[i]);
        }else{
            log("%s is an operator\n", s[i].c_str());
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
            log("%s is an operator for evaluating", evaluateStack.top().c_str());

            operand1 = results.top(); results.pop();
            operand2 = results.top(); results.pop();

            if(!is_number(operand1)) operand1 = variableLookup(operand1);
            if(!is_number(operand2)) operand2 = variableLookup(operand2);

            op = evaluateStack.top(); evaluateStack.pop();
            log(" - evaluating: %s %s %s - ", operand1.c_str(), op.c_str(), operand2.c_str());

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
            log(" result is %s\n", result.c_str());
        }
    }
    log("Result is %s\n", results.top().c_str());
    return results.top();
}

string variableLookup(string var){
    log("Searching for variable %s in scope level %i\n", var.c_str(), currentScope->scopeLevel);
    log("Variables in scope level %i:\n", currentScope->scopeLevel);
    for(Variable* v : currentScope->scopeVariables){
        log("Var: %s\n", v->name.c_str());
    }
    log("\n");
    string latestVarValue = "";;
    for(Variable* v : currentScope->scopeVariables){
        if(v->name == var){
            latestVarValue = v->value;
        }
    }
    Function* func = currentScope->fetchFunction(var);
    if(func == NULL) throw string("Variable " + var + " is undefined");
    log("Searching for function %s\n", var.c_str());
    if(func->isFunction){
        log("Variable %s IS a function\n", var.c_str());
        outputStream += (func->printCalls);
    }else{
        log("Variable %s IS NOT a function\n", var.c_str());
    }
    if(latestVarValue != "") return latestVarValue;
    else throw string("Variable " + var + " is undefined");
    return "";
}

void consumeIndents(){
    while(nextToken == INDENT){
        numOfIndents++;
        log("Hit INDENT, increase indent count: %i\n", numOfIndents);
        lex();
    }
    //Exits with identifier being the next token.
}

void updateCurrentScope(){
    if(numOfIndents > currentScope->scopeLevel){
        log("%i > %i : Traverse into inner scope\n", numOfIndents, currentScope->scopeLevel);
        while(numOfIndents > currentScope->scopeLevel && currentScope->innerScope != NULL){
            currentScope = currentScope->innerScope;
            log("Entered inner scope with level %i\n", currentScope->scopeLevel);
        }
    }
    else if(numOfIndents < currentScope->scopeLevel){
        log("%i < %i : Traverse into outer scope\n", numOfIndents, currentScope->scopeLevel);
        while(numOfIndents < currentScope->scopeLevel && currentScope->outerScope != NULL){
            currentScope = currentScope->outerScope;
            log("Entered outer scope with level %i\n", currentScope->scopeLevel);
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
    isFunction = false;
}

Variable::Variable(string varName, string varValue){
    name = varName;
    value = varValue;
    isFunction = false;
}

void Variable::setValue(string varValue){
    value = varValue;
}

Function::Function(){
    isFunction = true;
}

Function* Scope::fetchFunction(string funcName){
    for(int i = this->scopeVariables.size() - 1; i >= 0 ; i--){
        if(funcName == this->scopeVariables[i]->name){
            return (Function*)this->scopeVariables[i];
        }
    }
    return NULL;
}

Scope* Scope::getFunctionScope(){
    Scope* trav = currentScope;
    while(trav != NULL && trav->outerScope != NULL){
        string boo = trav->functionMode? "Is in function mode" : "Not in function mode";
        log("Current Scope(%i) %s\n", trav->scopeLevel, boo.c_str());
        if(trav->functionMode){
            log("Return scope with level %i\n", trav->scopeLevel);
            return trav;
        }else{
            trav = trav->outerScope;
            log("Traverse to outer scope with level %i\n", trav->scopeLevel);
        };
    }
    return trav;
};

Scope::Scope(){
    this->scopeLevel = 0;
    this->innerScope = NULL;
    this->outerScope = NULL;
    this->functionMode = false;
    this->actingFunctionName = "";
}

Scope::Scope(std::vector<Variable*> parentVars, int new_scopeL){
    this->scopeLevel = new_scopeL + 1;
    this->scopeVariables = parentVars;
    this->innerScope = NULL;
    this->functionMode = false;
    this->actingFunctionName = "";
}
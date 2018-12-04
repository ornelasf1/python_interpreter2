#include <memory>
#include "lexical.h"
#include "parser.h"
#include <string>
#include <string.h>
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
void functionDefintionRule();
void evaluateFunctionCall(string);

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
void expressionRule(std::vector<string>&);
void termRule(std::vector<string>&);
void factorRule(std::vector<string>&);
string evaluate(std::vector<string>&);

//string evaluate(string opd1, const char* op, string opd2);
string variableLookup(string var);
void consumeIndents();
void updateCurrentScope();
bool is_number(const string&);
void assignFunctionValue();

const string reservedKeywords[] = {"print", "def", "if", "return", "and", "or"};

//std::vector<Variable*> userVariables;
//std::vector< std::vector<Scope*> > scopeOfScopes;
int funcStackDepth = -1;
bool killRecFunction = false;
int ifelseDepth = 0;
bool exprInReturn = false;
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
        updateCurrentScope();
        if(currentScope->fetchFunction(lexeme) != NULL){

        }if(lexeme == "print"){
            log("Producing Print Rule with lexeme '%s' and scope level %i\n", lexeme.c_str(), currentScope->scopeLevel);
            printRule();
        }else if(lexeme == "def"){
            log("Producing Function Rule with lexeme '%s' and scope level %i\n", lexeme.c_str(), currentScope->scopeLevel);
            //functionRule();
            functionDefintionRule();
        }else if(lexeme == "if"){
            log("Producing IfStmt Rule with lexeme '%s' and scope level %i\n", lexeme.c_str(), currentScope->scopeLevel);
            ifstmtRule();
        }else if(lexeme == "return"){
            log("Setting return value for func '%s' with lexeme '%s' and scope level %i\n", currentScope->actingFunctionName.c_str(), lexeme.c_str(), currentScope->scopeLevel);
            assignFunctionValue();
        }else if(lexeme == "else"){
            outputStreamIfElseError += ("\nSyntax Error in line " + std::to_string(lineNumber) + ": else without if.");
            endProgram = true;
            lex();
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

                    // lex();
                    // expressionRule();
                    // var->value = evaluate(exprTokens);
                    // if(nextToken == LINEBREAK){
                    //     //if(currentScope->scopeLevel != 0){
                    //     lex();
                    //     consumeIndents();
                    // }else{
                    //     throw string("Error after assignment line");
                    // }


                    //expressionRule();
                    string varValue = variableLookup(lexeme);
                    //string varValue = evaluate(exprTokens);
                    printString += varValue;
                    log("print works var: %s\n", printString.c_str());
                    log("Return to print call with '%s'\n", lexeme.c_str());
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
                else if(nextToken == INTEGER){
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


        // Scope* temp = currentScope->getFunctionScope();
        // if(temp->functionMode){
        //     printVariables(temp->actingFunctionName, temp->scopeVariables, temp);
        //     printVariables(temp->outerScope->actingFunctionName, temp->outerScope->scopeVariables, temp->outerScope);
        //     log("Print rule assigning string to scope with function name '%s' back to scope level %i\n", temp->actingFunctionName.c_str(), temp->outerScope->scopeLevel);
        //     temp->outerScope->fetchFunction(temp->actingFunctionName)->printCalls += (printString + "\n");
        // }else{
        //     outputStream += (printString + "\n");
        // }

        outputStream += (printString + "\n");

        log("Print rule exitting with scope level %i and with token '%s'\n", currentScope->scopeLevel, lexeme.c_str());
    }catch(string s){
        log("Syntax Error in line %i: %s\n", lineNumber, s.c_str());
        outputStream += ("\nSyntax Error in line " + std::to_string(lineNumber) + ": " + s);
        endProgram = true;
    }
}


void assignmentRule(string varName){
    log("Assignment rule entered with scope level %i and with token '%s'\n", currentScope->scopeLevel, lexeme.c_str());
    string variableName = varName;
    string stringValue = "";
    int exprValue;

    Variable* var = new Variable(variableName);
    currentScope->scopeVariables.push_back(var);
    log("Checking if '%s' is mutated\n", variableName.c_str());
    Scope* local = currentScope->getFunctionScope();
    printVariables("", local->scopeVariables, local);
    if(local->scopeVariables.size() >= 2){
        for(int i = local->scopeVariables.size() - 2; i >= 0; i--){
            if(local->scopeVariables[i]->name == variableName && !local->functionMode){
                outputStreamMutations += (variableName + ", ");
            }
        }
    }
    try{
        lex();
        std::vector<string> exprTokens;
        expressionRule(exprTokens);
        var->value = evaluate(exprTokens);
        if(nextToken == LINEBREAK){
            lex();
            consumeIndents();
        }else{
            throw string("Error after assignment line");
        }

        log("Assign '%s' to '%s'\n", var->value.c_str(), var->name.c_str());
        //Exits with LINEBREAK

    }catch(string s){
        log("Syntax Error in line %i: %s\n", lineNumber, s.c_str());
        outputStream += ("\nSyntax Error in line " + std::to_string(lineNumber) + ": " + s);
        endProgram = true;
    }
    log("Assignment rule exitting with scope level %i and with token '%s'\n", currentScope->scopeLevel, lexeme.c_str());
}

void functionDefintionRule(){
    log("Function definition rule entered with scope level %i and with token '%s'\n", currentScope->scopeLevel, lexeme.c_str());
    int indentsToDef = numOfIndents;
    string functionName;
    Function* func = new Function();
    func->funcDefinStream_START = beginPosForLine;
    func->funcDefinition_LineNum = lineNumber;
    try{
        func->funcDefinition_START = tokens.size() - numOfIndents - 1;
        log("Def definiton: function START token is '%s'\n", lexemes[func->funcDefinition_START].c_str());
        lex();
        if(nextToken == IDENTIFIER){
            functionName = lexeme;
            func->name = lexeme;
            lex();
            if(nextToken == LEFT_PAREN){
                lex();

                while(nextToken != RIGHT_PAREN){
                    Variable* param = new Variable(lexeme);
                    func->argsv.push_back(param);
                    func->argc++;
                    lex();
                    if(nextToken == COMMA) lex();
                }

                if(nextToken == RIGHT_PAREN){
                    lex();
                    if(nextToken == COLON){
                        lex();
                        if(nextToken == LINEBREAK){
                            do{
                                //Skip def statement block until the end of it (the first identifer the same level as 'def')
                                lex();
                                consumeIndents();
                                if(indentsToDef == numOfIndents && nextToken == IDENTIFIER){
                                    break;
                                }
                            }while(true);
                            func->funcDefinition_END = tokens.size() - 2;
                            func->funcDefinStream_END = inputFile.tellg();
                            log("Def definiton: leaving with lexeme '%s'\n", lexeme.c_str());
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

        currentScope->scopeVariables.push_back(func);
    }catch(string s){
        log("Syntax Error in line %i: %s\n", lineNumber, s.c_str());
        outputStream += ("\nSyntax Error in line " + std::to_string(lineNumber) + ": " + s);
        endProgram = true;
    }
}

void functionRule(){
    log("Function rule entered with scope level %i and with token '%s'\n", currentScope->scopeLevel, lexeme.c_str());
    int indentsToDefFunc;
    string functionName;
    //Function* func = new Function();
    Function* func;
    try{
        lex();
        if(nextToken == IDENTIFIER){
            functionName = lexeme;
            //func->name = lexeme;
            func = currentScope->fetchFunction(lexeme);
            lex();
            if(nextToken == LEFT_PAREN){
                lex();

                //Parameters implementation
                while(nextToken != RIGHT_PAREN){
                    // Variable* param = new Variable(lexeme);
                    // func->argsv.push_back(param);
                    // func->argc++;
                    lex();
                    //if(nextToken == COMMA) lex();
                }
                //currentScope->scopeVariables.push_back(func);

                if(nextToken == RIGHT_PAREN){
                    lex();
                    if(nextToken == COLON){
                        indentsToDefFunc = numOfIndents;
                        lex();
                        if(nextToken == LINEBREAK){
                            lex();
                            consumeIndents();
                            while(nextToken == LINEBREAK){
                                lex();
                                consumeIndents();
                            }
                            log("Compare indents '%i' and '%i'\n", numOfIndents, indentsToDefFunc + 1);
                            if(numOfIndents == indentsToDefFunc + 1){
                                log("(%i) Creating new scope in 'def' call\n", currentScope->scopeLevel);
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
                                    if(currentScope->getFunctionScope()->fetchFunction(functionName)->value != ""){
                                        log("Value of '%s' is already set to '%s'\n", functionName.c_str(), currentScope->getFunctionScope()->fetchFunction(functionName)->value.c_str());
                                        return;
                                    }
                                    if(funcStackDepth > funcStackDepthLIMIT || killRecFunction){
                                        log("Kill rec function with rec depth '%i'\n", funcStackDepth);
                                        killRecFunction = true;
                                        log("Killing recurse with lexeme '%s'\n", lexeme.c_str());
                                        return;
                                    }
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
        evaluateFunctionCall(funcName);
        if(killRecFunction){
            printf("Function '%s' does NOT terminate\n", funcName.c_str());
            killRecFunction = false;
            outputStreamRecFuncTerm += "No, ";
        }else{
            printf("Function '%s' DOES terminate\n", funcName.c_str());
            outputStreamRecFuncTerm += "Yes, ";
        }
        // lex();
        // if(nextToken == RIGHT_PAREN){
            log("Function call in scope level %i\n", currentScope->scopeLevel);
            //outputStream += currentScope->fetchFunction(funcName)->printCalls;
            //lex();
            if (nextToken == LINEBREAK || nextToken == END){
                lex();
                consumeIndents();
                return;
            }else{
                throw string("Expected \\n after )");
            }
        // }else{
        //     throw string("Expected closing parentheses to '('");
        // }
    }catch(string err){
        log("Syntax Error in line %i: %s\n", lineNumber, err.c_str());
        outputStream += ("\nSyntax Error in line " + std::to_string(lineNumber) + ": " + err);
        endProgram = true;
    }
    log("Function CALL rule exitted with scope level %i and token '%s'\n", currentScope->scopeLevel, lexeme.c_str());
}

void assignFunctionValue(){
    try{
        lex();
        if(nextToken == IDENTIFIER || nextToken == INTEGER){
            std::vector<string> exprTokens;
            expressionRule(exprTokens);
            if(exprTokens.size() > 1) exprInReturn = true;
            // if(!exprInReturn){
            //     Scope* funcScope = currentScope->getFunctionScope();
            //     log("ENDING EARLY\n");
            //     funcScope->outerScope->fetchFunction(funcScope->actingFunctionName.c_str())->value = "";
            //     exprTokens.clear();
            //     lex();
            //     return;
            // }
            string varValue = evaluate(exprTokens);

            Scope* funcScope = currentScope->getFunctionScope();
            log("Setting value of function '%s' to '%s' in scope level %i\n", funcScope->actingFunctionName.c_str(), varValue.c_str(), funcScope->scopeLevel);
            funcScope->outerScope->fetchFunction(funcScope->actingFunctionName.c_str())->value = varValue;
            printVariables(funcScope->outerScope->actingFunctionName, funcScope->outerScope->scopeVariables, funcScope->outerScope);
            lex();
            // if (nextToken == LINEBREAK || nextToken == END){
            //     lex();
            //     consumeIndents();
            //     return;
            // }else{
            //     throw string("Expected \\n after the identifier");
            // }
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
                //ifelseDepth++;
                log("Incrementing ifelsedepth to %i\n", ifelseDepth);
                log("(%i) if resulted in TRUE next lexeme is '%s'\n", currentScope->scopeLevel, lexeme.c_str());
                if(left_parenthesis) lex();
                if(nextToken == COLON){
                    indentsToIfStmt = numOfIndents;
                    lex();
                    if(nextToken == LINEBREAK){
                        lex();
                        consumeIndents();
                        while(nextToken == LINEBREAK){
                            lex();
                            consumeIndents();
                        }
                        if(numOfIndents == indentsToIfStmt + 1){
                            ifelseDepth++;
                            log("(%i) Creating new scope in 'if' call\n", currentScope->scopeLevel);
                            Scope* if_Scope = new Scope(currentScope->scopeVariables, currentScope->scopeLevel);
                            currentScope->innerScope = if_Scope;
                            if_Scope->outerScope = currentScope;
                            do{
                                if(nextToken == INDENT) consumeIndents();
                                log("IFSTMT: Looping with '%s' and with scope level %i\n", lexeme.c_str(), currentScope->scopeLevel);
                                parse();
                                if(nextToken == END) return;
                            }while(numOfIndents == indentsToIfStmt + 1 || nextToken == LINEBREAK || nextToken == INDENT);
                            log("IFSTMT: EXITTING loop with %s and with scope level %i\n", lexeme.c_str(), currentScope->scopeLevel);
                            currentScope->outerScope->scopeVariables = currentScope->scopeVariables;

                            if(numOfIndents == indentsToIfStmt && nextToken == IDENTIFIER && lexeme == "else"){
                                log("TRUE: Skipping else code block\n");
                                do{
                                    log("IFSTMT TRUE SKIP ELSE: Looping with '%s' and with scope level %i\n", lexeme.c_str(), currentScope->scopeLevel);
                                    while(nextToken != LINEBREAK){
                                        log("IFSTMT TRUE SKIP ELSE: lex is '%s' : Skipping code block statements\n", lexeme.c_str());
                                        lex();
                                        if(nextToken == END) return;
                                    }
                                    lex();
                                    consumeIndents();
                                    if(numOfIndents <= indentsToIfStmt && nextToken == IDENTIFIER){
                                        break;
                                    }
                                }while(true);
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
                        while(nextToken == LINEBREAK){
                            lex();
                            consumeIndents();
                        }
                        if(numOfIndents == indentsToIfStmt + 1){
                            ifelseDepth++;
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
                                if(numOfIndents <= indentsToIfStmt && nextToken == IDENTIFIER){
                                    break;
                                }
                            }while(true);

                            if(numOfIndents == indentsToIfStmt && nextToken == IDENTIFIER && lexeme == "else"){
                                lex();
                                if(nextToken == COLON){
                                    lex();
                                    if(nextToken == LINEBREAK){
                                        lex();
                                        consumeIndents();
                                        while(nextToken == LINEBREAK){
                                            lex();
                                            consumeIndents();
                                        }
                                        if(numOfIndents == indentsToIfStmt + 1){
                                            ifelseDepth++;
                                            log("(%i) Creating new scope in 'else' call\n", currentScope->scopeLevel);
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
        if(numOfIndents == 0 || endProgram){
            int newDepth = ifelseDepth;
            if(ifelseDepth != 0) newDepth = ifelseDepth - 1;
            outputStreamIfElseLvls += (std::to_string(newDepth) + " level, ");
            ifelseDepth = 0;
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
    std::vector<string> exprTokens;
    expressionRule(exprTokens);
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
        expressionRule(exprTokens);
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
        expressionRule(exprTokens);
        operand2 = evaluate(exprTokens);
        log("Bool expr: operand 2 is '%s'\n", operand2.c_str());
        if(nextToken == RIGHT_PAREN || nextToken == COLON){
            return atoi(operand1.c_str()) >= atoi(operand2.c_str());
        }else{
            throw string("Enclosing characters for condition missing");
        }
    }else if(nextToken == COND_GT){
        lex();
        expressionRule(exprTokens);
        operand2 = evaluate(exprTokens);
        log("Bool expr: operand 2 is '%s'\n", operand2.c_str());
        if(nextToken == RIGHT_PAREN || nextToken == COLON){
            return atoi(operand1.c_str()) > atoi(operand2.c_str());
        }else{
            throw string("Enclosing characters for condition missing");
        }
    }else if(nextToken == COND_LTEQ){
        lex();
        expressionRule(exprTokens);
        operand2 = evaluate(exprTokens);
        if(nextToken == RIGHT_PAREN || nextToken == COLON){
            return atoi(operand1.c_str()) <= atoi(operand2.c_str());
        }else{
            throw string("Enclosing characters for condition missing");
        }
    }else if(nextToken == COND_LT){
        lex();
        expressionRule(exprTokens);
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

void expressionRule(std::vector<string>& exprTokens){
    log("Expression rule entered with '%s'\n", lexeme.c_str());

    try{
        termRule(exprTokens);
        while(nextToken == ADD_OP || nextToken == SUB_OP){
            exprTokens.push_back(lexeme);
            lex();
            termRule(exprTokens);
        }
        log("Exitting Expression Rule with lex '%s'\n", lexeme.c_str());
    }catch(string s){
        log("Syntax Error in line %i: %s\n", lineNumber, s.c_str());
        outputStream += ("\nSyntax Error in line " + std::to_string(lineNumber) + ": " + s);
        endProgram = true;
    }
}

void termRule(std::vector<string>& exprTokens){
    log("Term rule entered with '%s'\n", lexeme.c_str());

    factorRule(exprTokens);
    while(nextToken == MULT_OP || nextToken == DIV_OP){
        exprTokens.push_back(lexeme);
        lex();
        factorRule(exprTokens);
    }
}

void factorRule(std::vector<string>& exprTokens){
    log("Factor rule entered with '%s'\n", lexeme.c_str());
    
    if(nextToken == IDENTIFIER || nextToken == INTEGER){
        if(lexeme == "True" || lexeme == "False"){
            return;
        }else{
            // string val = lexeme;
            // Function* temp = currentScope->fetchFunction(currentScope->actingFunctionName);
            // if(temp != NULL){
            //     for(int i = 0; i < temp->argc; i++){
            //         log("Compare '%s' to '%s'\n", lexeme.c_str(), temp->argsv[i]->name.c_str());
            //         if(lexeme == temp->argsv[i]->name){
            //             val = temp->argsv[i]->value;
            //         }
            //     }
            // }
            // log("Push back '%s'\n", val.c_str());
            if(nextToken == INTEGER){
                exprTokens.push_back(lexeme);
                lex();
                return;
            }
            // string exprToken = lexeme;
            exprTokens.push_back(lexeme);
            // if(nextToken == IDENTIFIER)
            //     evaluateFunctionCall(lexeme);
            // else
            //     lex();
            Function* func = currentScope->fetchFunction(lexeme);
            lex();
            if(nextToken == LEFT_PAREN){
                //exprTokens.push_back(exprToken);
                lex();
                int arguments = 0;
                while(nextToken != RIGHT_PAREN){
                    do{
                        log("Argument checking: lex is '%s'\n", lexeme.c_str());
                        if(nextToken == COMMA) lex();
                        log("Argument checking: lex after is '%s'\n", lexeme.c_str());
                        // if(arguments == func->argc - 1){
                        //     //Place stream position to return back to function call after processing function content
                        //     log("Placing stream pos from '%i'\n", inputFile.tellg());
                        //     log("Lexeme: '%s'   Char: '%c'\n", lexeme.c_str(), nextChar);
                        //     origStreamPos = inputFile.tellg();
                        // }
                        std::vector<string> innerExprTokens;
                        expressionRule(innerExprTokens);
                        string value = evaluate(innerExprTokens);
                        //func->argsv[arguments]->value = value;
                        func->argsv[arguments]->value_s.push_back(value);
                        log("Scanning arguments: assign '%s' to '%s'\n", value.c_str(), func->argsv[arguments]->name.c_str());
                        arguments++;
                    }while(nextToken == COMMA);
                }
                if(arguments != func->argc){
                    throw string("Function " + func->name + " requires " + std::to_string(func->argc) + " arguments; was given " + std::to_string(arguments));
                }
                if(nextToken == RIGHT_PAREN){
                    lex();
                }else{
                    throw string("Missing right parentheses in factor rule");
                }
            }
            // else{
            //     //Attempt early evaluation
            //     string earlyVal = variableLookup(exprToken);
            //     exprTokens.push_back(earlyVal);
            // }
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
        expressionRule(exprTokens);
        if(nextToken == RIGHT_PAREN){
            exprTokens.push_back(lexeme);
            lex();
        }else{
            endProgram = true;
            throw string("No matching )");
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
string evaluate(std::vector<string>& s){
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
    s.clear();

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
                result = std::to_string(atoi(operand2.c_str()) - atoi(operand1.c_str()));
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

//Enters with token identifier and exits with token after right paren
void evaluateFunctionCall(string name){
    log("Evaluate function call rule entered with scope level %i and token '%s'\n", currentScope->scopeLevel, lexeme.c_str());
    try{
        Function* func = currentScope->fetchFunction(name);
        if(!func->isFunction){
            return;
        }
        if(nextToken == LEFT_PAREN){
            lex();
            int arguments = 0;
            while(nextToken != RIGHT_PAREN){
                // log("Scanning arguments: assign '%s' to '%s'\n", lexeme.c_str(), func->argsv[arguments]->name.c_str());
                // func->argsv[arguments]->value = lexeme;
                // arguments++;
                // lex();
                // if(nextToken == COMMA) lex();

                do{
                    log("Argument checking: lex is '%s'\n", lexeme.c_str());
                    if(nextToken == COMMA) lex();
                    log("Argument checking: lex after is '%s'\n", lexeme.c_str());
                    std::vector<string> exprTokens;
                    expressionRule(exprTokens);
                    if(exprTokens.size() > 1) exprInReturn = true;
                    // if(!exprInReturn){
                    //     Scope* funcScope = currentScope->getFunctionScope();
                    //     log("ENDING EARLY\n");
                    //     exprTokens.clear();
                    //     lex();
                    //     return;
                    // }
                    string value = evaluate(exprTokens);
                    //func->argsv[arguments]->value = value;
                    func->argsv[arguments]->value_s.push_back(value);
                    log("Scanning arguments: assign '%s' to '%s'\n", value.c_str(), func->argsv[arguments]->name.c_str());
                    arguments++;
                }while(nextToken == COMMA);
            }
            if(arguments != func->argc){
                throw string("Function " + func->name + " requires " + std::to_string(func->argc) + " arguments; was given " + std::to_string(arguments));
            }
        }
        //if(nextToken == RIGHT_PAREN){
            // lex();
            // for(int i = func->funcDefinition_START; i <= func->funcDefinition_END; i++){
            //     log("'%s'\n", lexemes[i].c_str());
            //     for(int j = 0; j < lexemes[i].length(); j++){
            //         if(tokens[i] == INDENT){
            //             for(int k = 0; k < indentCount; k++){
            //                 log("From '%s', putback '%c'\n", lexemes[i].c_str(), ' ');
            //                 inputFile.putback(' ');
            //             }
            //         }else{
            //             log("From '%s', putback '%c'\n", lexemes[i].c_str(), lexemes[i][j]);
            //             inputFile.putback(lexemes[i][j]);
            //             inputFile.putback(' ');
            //         }
            //     }
            // }
        log("Before determining recursiveness Lexeme: '%s'   Char: '%c'\n", lexeme.c_str(), nextChar);
        if(!killRecFunction){
            log("Placing stream pos from '%i'", inputFile.tellg());
            log(" to '%i'\n", func->funcDefinStream_START);
            std::streampos origStreamPos = inputFile.tellg();
            log("Before going Lexeme: '%s'   Char: '%c'\n", lexeme.c_str(), nextChar);
            log("Original pos next character will be '%c'\n", inputFile.peek());
            funcStackDepth++;
            int oldLineNumber = lineNumber;
            lineNumber = func->funcDefinition_LineNum;
            inputFile.seekg(func->funcDefinStream_START);
            getChar();
            lex();
            log("Attempting to time travel -> Lexeme: '%s'   Char: '%c'\n", lexeme.c_str(), nextChar);
            functionRule();
            log("We have finished evaluating '%s'", func->name.c_str());
            log(" placing ORIGINAL stream pos to '%i'\n", origStreamPos);
            inputFile.seekg(origStreamPos);
            inputFile.seekg(-2, std::ios::cur);
            lineNumber = oldLineNumber;
            funcStackDepth--;
            
            log("Came back with Lexeme: '%s'   Char: '%c'\n", lexeme.c_str(), nextChar);
            getChar();
            lex();
            log("Getting back to speed Lexeme: '%s'   Char: '%c'\n", lexeme.c_str(), nextChar);
        }else{
            log("END RECURSIVENESS\n");
        }

        for(int i = 0; i < func->argsv.size(); i++){
            if(!func->argsv[i]->value_s.empty()){
                log("Pop from argsv_stack %i - name '%s' value '%s'\n", i, func->name.c_str(), func->argsv[i]->value_s.back().c_str());
                func->argsv[i]->value_s.pop_back();
            }
        }
        // }else{
        //     throw string("Expected closing parentheses to '('");
        // }
    }catch(string s){
        log("Syntax Error in line %i: %s\n", lineNumber, s.c_str());
        outputStream += ("\nSyntax Error in line " + std::to_string(lineNumber) + ": " + s);
        endProgram = true;
    }
}

string variableLookup(string var){
    log("Entering variableLookup with lexeme being '%s'\n", lexeme.c_str());
    log("Searching for variable %s in scope level %i\n", var.c_str(), currentScope->scopeLevel);
    log("Variables in scope level %i:\n", currentScope->scopeLevel);
    for(Variable* v : currentScope->scopeVariables){
        log("Var: %s\n", v->name.c_str());
    }
    log("\n");
    string latestVarValue = "";

    string funcName = currentScope->getFunctionScope()->actingFunctionName;
    Function* temp = currentScope->getFunctionScope()->fetchFunction(funcName);
    // if(temp != NULL){
    //     for(int i = 0; i < temp->argc; i++){
    //         log("Compare '%s' to '%s'\n", var.c_str(), temp->argsv[i]->name.c_str());
    //         if(var == temp->argsv[i]->name){
    //             log("Return '%s' with value '%s'\n", var.c_str(), temp->argsv[i]->value.c_str());
    //             return temp->argsv[i]->value;
    //         }
    //     }
    // }

    if(temp != NULL){
        for(int i = 0; i < temp->argc; i++){
            log("Compare '%s' to '%s'\n", var.c_str(), temp->argsv[i]->name.c_str());
            if(var == temp->argsv[i]->name){
                log("Return '%s' with value '%s'\n", var.c_str(), temp->argsv[i]->value_s[funcStackDepth].c_str());
                return temp->argsv[i]->value_s[funcStackDepth];
            }
        }
    }

    Function* func = currentScope->fetchFunction(var);
    if(func == NULL) throw string("Variable " + var + " is undefined");
    log("Searching for function %s\n", var.c_str());
    if(func->isFunction){
        log("Variable '%s' IS a function\n", var.c_str());
        // lex();
        // if(nextToken == LEFT_PAREN){
        //     evaluateFunctionCall(var);
        // }else{
        //     throw string("Function call missing left parenthesis");
        // }
        evaluateFunctionCall(var);
        
    }else{
        log("Variable %s IS NOT a function\n", var.c_str());
    }

    printVariables(currentScope->actingFunctionName, currentScope->scopeVariables, currentScope);
    for(int i = currentScope->scopeVariables.size() - 1; i >= 0; i--){
        log("Variables: Compare '%s' and '%s'\n", currentScope->scopeVariables[i]->name.c_str(), var.c_str());
        if(currentScope->scopeVariables[i]->value != "" && currentScope->scopeVariables[i]->name == var){
            log("Variables: Set latestVarValue to '%s'\n", currentScope->scopeVariables[i]->value.c_str());
            latestVarValue = currentScope->scopeVariables[i]->value;
            break;
        }
    }
    if(killRecFunction) return "";    
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

void processEmptyLines(){
    while(nextToken == LINEBREAK){
        log("Hit LINEBREAK, skipping line\n");
        lex();
    }
    //Exits with identifier/indent being the next token.
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
    //doesItEverTerminate = true;
    argc = 0;
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
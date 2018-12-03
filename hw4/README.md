# python_interpreter HW4
cosc2991 1354098 Fabian Ornelas
cosc2992 1293866 Michael Panagos

Makefile will produce mypython program.

- Needs to be run using std=c++11
- Last line of file should have an extra line break to avoid error

Usage: 
./mypython {file.py}


Main file is mypython.cpp
The program will begin in the mypython file by attempting to open the given input .py file.
If the file is valid, then it will begin to run the lex/parser on the inputstream.
Lastly, it will output the values that were indicated in the python file, mostly from 'print' function call.

lexical.cpp
Processes the input file character by character and forms lexemes. It then actively classifies the characters it receives to tokens based on the content of the lexeme. After a lexeme has been formed, it will classify it as a token
and pass it along to the parser contained in parser.cpp

parser.cpp
Picks up on two important values from a lex call, nextToken and lexeme, which will be repeatedly requested in order to determine if the order of tokens obey the Python syntax. If so, it will perform accordingly to the function of the statements.

# BIUAdvancedProgramming1 - FlightSimulator

link :
https://github.com/ronenpresser/BIUAdvancedProg1

## Background on the project
This project is an assignment as a part of a course called Advanced Programming 1 in our Computer Seince degree of second year in Bar Ilan University.

## Part #1
In this first milestone of the project, we'll make a code parser that will remote control a plane in a FlightGear simulator.
The parser will identify commands from a given txt file that contains the code, and execute them.

### Getting Started and Prerequisites
1.
2.
3.
4.
5.

### General explanations on the code parser - how does it work
We will use the Command DP to parse the code in a given txt file and fly the simulator.

There are 2 main parts:

#### The lexer part: Class Lexer 
func lexer(string filePath)
The goal of the lexing part is to make a vector of tokens made of a given txt file with the code that will fly the plane.
Each line in the txt file contains a command that is needed to be executed and dividing each line to those tokens will help us determine which commands we need to execute in the next part.This class has only one function: lexer

#### The parser part: Class Parser 
func parse(vector<string> tokensVector)
The goal of the parsing part is to parse - to take the tokens vector from the lexer, iterate over it and execute the matched commands by a current token.
There are 10 commands (classes) that can be executed, all inherits a the Command class that has only one function - execute.
The execute function returns a int number, that represents the steps that the parser need to skip in the tokens vector, to get to the next token of command.
This function gets 3 parameters:The tokens vector, the current index of the tokens vector in the parsing part and the Parser.

* OpenServerCommand:
* ConnectCommand:
* PrintCommand:
* SleepCommand:
* DefineVarCommand:
* VarAssignmentCommand:
* IfCommand:
* LoopCommand:
* DefiningFuncCommand:
* FuncCommand:





### Possible changes / improvments

* Implementing Singleton DP on The SymbolTable and the InterpreterTool members of the Parser,
  as well the Parser itself.
* Improve the FuncCommand and DefineFuncCommand to support multiple parameters of a function.




# Part #2
In this second milestone of the project, we'll


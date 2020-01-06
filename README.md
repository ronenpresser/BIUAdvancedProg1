# BIUAdvancedProgramming1

link :
https://github.com/ronenpresser/BIUAdvancedProg1

## Background on the project
This project is an assignment as a part of a course called Advanced Programming 1 in our Computer Seince degree of second year in Bar Ilan University, written in c++.

## Part #1 -  FlightSimulator
In this first milestone of the project, we'll make a code parser that will remote control a plane in a FlightGear simulator.
The parser will identify commands from a given txt file that contains the code, and execute them.

### Getting Started and Prerequisites
In general, we need 3 things. The program itself, a txt file containing code that we want to parse and the flightgear flight simulator.

To get started and operate the program, follow the next steps.
1.Download the following files to your comupter and place it under a new folder:
```
Command.cpp, Command.h,InterpretTool.cpp.InterpretTool.h, Expression.cpp, Expression.h, Lexer.cpp, Lexer.h, Parser.cpp, Parser.h, main.cpp
```


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


main.cpp :
The only thing that the main does is lexing the given txt file
and send the token vector to the parse function of the parser
and start parsing:

#### The parser part: Class Parser 
func parse(vector<string> tokensVector)
The goal of the parsing part is to parse - to take the tokens vector from the lexer, iterate over it and execute the matched commands by a current token.
On creation, 2 maps of the parser will be built in a hard coded way:
  1.command_map:
    The keys are names of usable commands and the values are Command objects
  2.indexToSimulatorPathMap:
    The keys are numbers from 0 to 35 and the values are strings that are paths of the simulator - taken from the generic_small.xml
    file that the simulator samples from and sends it 10 times very seconds.
The parser class has 5 main members:
* command_map: 
  A map of strings as keys(names of commands)and Commands as value. will be used on the parsing part, well send a token to get a wanted     Command to execute.
* symbol_table:
  An object of SymbolTable class that contains a map of strings as keys - variables names and Variable pointers as values.
* simPathToVarMap:
  a map of string - simulator paths as keys and Variable pointers as value.
* indexToSimPathMap:
  as written before, will be used to identify the values that we read from the simulator to update the matched Variables values.
* interpret_tool - object of the class InterpretTool. The class is a tool for interpreting arithetic expressions that contain
  variable names, number, arithmetic operators (* + - /) ,boolean operators (< > <= >= != ==) and parenthisess to an Expression
  object recoursivly that can be calculated.
  
There are 10 commands (classes) that can be executed, all inherits the Command class that has only one function - execute.
The execute function returns a int number, that represents the steps that the parser need to skip in the tokens vector, to get to the next token of command.
This function gets 3 parameters:The tokens vector, the current index of the tokens vector in the parsing part and the Parser.

* OpenServerCommand:
  example:
  ```
  openDataServer(5400)
  ```
* ConnectCommand:
  example:
  ```
  connectControlClient("127.0.0.1",5402)
  ```
* PrintCommand:
  example:
  ```
  Print(alt)
  Print(1000)
  Print("lets fly")
  ```
* SleepCommand:
  example:
  ```
  Sleep(1000)
  Sleep(alt)
  ```
* DefineVarCommand:
  example:
  ```
  var magnetos -> sim("/controls/switches/magnetos")
  var h0 = heading
  ```
* VarAssignmentCommand:
  example:
  ```
  h0 = heading
  ```
* IfCommand:
  example:
  ```
   if alt < x {
    rudder = (h0 - (heading))/80
    aileron = -(roll) / 70
    elevator = pitch / 50
    Print(alt)
    Sleep(250)
    Print(x)
  }
  ```
 
* LoopCommand:
  
  ```
  while alt < x {
    rudder = (h0 - (heading))/80
    aileron = -(roll) / 70
    elevator = pitch / 50
    Print(alt)
    Sleep(250)
    Print(x)
    }
  ```
* DefineFuncCommand:
  example:
  ```
    takeoff(var x) {
   Print(x)
   while alt < x {
      rudder = (h0 - (heading))/80
    aileron = -(roll) / 70
    elevator = pitch / 50
      Print(alt)
    Sleep(250)
    Print(x)
     }
  }
  ```
* FuncCommand:
  example
  ```
  takeoff(1000)
  ```

So, first we want to open a server at a given port ,and wait for the simulator to connect as a client. Using the OpenServerCommand,
we initialize a thread of executing the OpenServerCommand, so the server that we open will always run in the background.
The simulator sends 10 times in a second 36 value separated by commas(The simulator used the generic_small.xml file from previous topic), and once the simulator connects, we'll initialize a thread that start reading the values and updating the matched values in the matched indexes bt separating each line of 36 values. 
In the meantime, we initialize a thread of ConnectCommand that connects us as a clinet (by a given IP and a port) to the simulator that will be used as our server and that thread will be always running so we can send a request to set a new value to a simulator path.

In this project the local host is the server and the client.

After those, we can continue iterating over the tokens vector and indetify wanted command by sending the tokensVector.at(index)
to the command map.

Once we get to part of executing the PrintCommand with the string "done" the program will finish.

### Possible changes / improvments

* Implementing Singleton DP on The SymbolTable and the InterpreterTool members of the Parser,
  as well the Parser itself.
* Improve the FuncCommand and DefineFuncCommand to support multiple parameters of a function.




# Part #2
In this second milestone of the project, we'll


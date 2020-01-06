# BIUAdvancedProgramming1
By: Amit Twito, Ronen Presser

link :
https://github.com/ronenpresser/BIUAdvancedProg1

## Background on the project
This project is a 2 parts assignment as a part of a course called Advanced Programming 1 in our Computer Seince degree of second year in Bar Ilan University, written in c++ (c++14 and bellow).

## Part #1 -  FlightSimulator
In this first milestone of the project, we'll make a code parser that will remote control a plane in a FlightGear simulator.
The parser will identify commands from a given txt file that contains the code, and execute them.

### Getting Started and Prerequisites
In general, we need 3 things. The program itself, a txt file containing code that we want to parse and the flightgear flight simulator.

To get started and operate the program, follow the next steps.

1.Download the following files to your computer and place it under one new folder:
```
main.cpp
Command.cpp, Command.h
InterpretTool.cpp, InterpretTool.h 
Expression.cpp, Expression.h 
Lexer.cpp, Lexer.h
Parser.cpp, Parser.h
generic_small.xml
fly, fly1, fly_with_func.txt
```
2.Go to https://www.flightgear.org/ and download the FlightGear flight simulator and install it.
On linux, you can go to the app store and search for "flightgear" and install it from there.

3.Once the installation ends, take the generic_small.xml file, open the installation folder of the flightgear
and search for the folder "data". If it exists, go to the folder "protocol" and put the file in there.
Else, search for the folder "protocol" in the main folder of the flightgear installation and put the file there.
On linux, if you can't put the file in the folder, open terminal and enter the following command:
```sudo cp /SOURCE_FOLDER_PATH/generic_small.xml /DESTINATION_FOLDER_PATH ```
That will copy the file.
The file is going to determine which values the simulator is going to send us when we connect to it as a client ,as we will soon see.
The xml file has 36 pathes that are used as directories in the telnet interface of the simulator, and each of the path is a value of a variable in the simulator.

4.Open the Flightgear simulator and go to settings -> additional settings. You'll need to write (char by char) the following lines there:
```
--telnet=socket,in,10,127.0.0.1,5402,tcp
--generic=socket,out,10,127.0.0.1,5400,tcp,generic_small
```
The first line tells the simulator to open in the background a connectable server with a telnet client,
through loclahost (127.0.0.1) on port 5402.
The second line tells the simulator to connect as a client to the server that we will build, through localhost on port 5400
as tcp client, and to sameple and send to the server 10 times per second values determined by the xml file ```generic_small```.

5.Now we need to compile the code. Open a terminal or a cmd through the new folder we created in the first instuction and type the following line:
```
g++ -std=c++14 *.cpp -pedantic -o a.out -pthread
```
Make sure you have a g++ compiler installed : 
windows:
https://www.tutorialspoint.com/How-to-Install-Cplusplus-Compiler-on-Windows
linux:
Open terminal and run the following:
```sudo apt install g++```
(The program works on c++14 and bellow).

6.Once a file called a.out is created, run the line (from the folder that contains the files):
```./a.out FILE_WITH_CODE.txt ```  as FILE_WITH_CODE.txt needs to be the path of file with the code we want to parse,
for example you can take the ```fly.txt```. If you only write the name of the file, it needs to be in the same folder
that we execute the program from. Or you enter a whole path to the file.
If you want to make your own ```fly.txt``` file, your are welcome to do so. In the next section you'll see examples and
conventions.

7.The program is waiting for a client to connect to a server that we will open, so we need to enter the simulator so it can connect.
Click on the "Fly!" button in the flightgear simulator and watch the plane fly. On the terminal you'll see progress and
printing.


### General explanations on the code parser - how does it work

We will use the Command DP to parse the code in a given txt file and fly the simulator.

There are 2 main parts:

#### The lexer part: Class Lexer 

func lexer(string filePath)
The goal of the lexing part is to make a vector of tokens made of a given txt file with the code that will fly the plane.
Each line in the txt file contains a command that is needed to be executed and dividing each line to those tokens will help us determine which commands we need to execute in the next part.This class has only one function: lexer


```main.cpp``` :
The only thing that the main does is lexing the given txt file(as an argument) and send the token vector to the parse function of the parser and start parsing:

#### The parser part: Class Parser 

func parse(vector<string> tokensVector)
The goal of the parsing part is to parse - to take the tokens vector from the lexer, iterate over it and execute the matched commands by a current token.
On creation, 2 maps of the parser will be built in a hard coded way:
  
  1.command_map:
    The keys are names of usable commands and the values are Command objects
    
  2.indexToSimulatorPathMap:
    The keys are numbers from 0 to 35 and the values are strings that are paths of the simulator - taken from the ```generic_small.xml ``` file that the simulator samples from and sends it 10 times very seconds

The parser class has 5 main members:
* command_map: 
  A map of strings as keys(names of commands)and Commands as value. will be used on the parsing part, well send a token to get a wanted Command to execute.
* symbol_table:
  An object of SymbolTable class that contains a map of strings as keys - variables names and Variable pointers as values.
* simPathToVarMap:
  a map of string - simulator paths as keys and Variable pointers as value.
* indexToSimPathMap:
  as written before, will be used to identify the values that we read from the simulator to update the matched Variables values.
* interpret_tool - object of the class InterpretTool. The class is a tool for interpreting arithetic expressions that contain
  variable names, number, arithmetic operators (* + - /) ,boolean operators (< > <= >= != ==) and parenthisess to an Expression object recoursivly that can be calculated.
  
There are 10 commands (classes) that can be executed, all inherits the Command class that has only one function - execute.
The execute function returns a int number, that represents the steps that the parser need to skip in the tokens vector, to get to the next token of command.
This function gets 3 parameters: The tokens vector, the current index of the tokens vector in the parsing part and the Parser.

**Conventions for the txt file:**

**Important - if the file does not have execution of OpenServerCommand and then a ConnectCommand after it, the program will exit, and those can be exectued only ONCE.In addition, variable assignment command can be with local variables, and without simulator path (with <- or ->).**

-The txt file need to contain 2 lines of execution of OpenServerCommand and a ConnectCommand. First the OpenServerCommand,
  then the ConnectCommand.
-Expressions have to contain equal number of ( ) meaning that each opening ( needs to have a closing one.

-Any expression in an unary operator needs to be wrapped with ( ).

-Any variable that is in an expression needs to be define before the decleration as we will soon see.

-IP needs to be wrapped by "".

-Declerations on string need to be wrapped by "" - on printing for example.

-In the end of the txt file write ```Print("done")```

-The names of the following commands in the txt file are case sensitive

**Commands**

* OpenServerCommand:Opens a server on a given port that runs in the background throught\ the whole program.
  Gets a port as a number or an expression.
  The string token key is ```openDataServer```
  The next first token is the port.
 example:
  ```
  openDataServer(5400)
  openDataServer(5400 + 2)
  ```
* ConnectCommand:Connects us as a client to the simulator as a server at given ip, on a given port.
Ip needs to be wrapped between " ", and the port is a number or math expression.
 The string token key is ```connectControlClient```.
 The next 2 tokens are the ip and the port.

example:
  ```
  connectControlClient("127.0.0.1",5402)
  ```
* PrintCommand:Prints a number, a string, value of an existing variable or an expression with variables and number.
  The string token key is ```Print```
  The next first token is the parameter.
 
 example:
  ```
  Print(alt)
  Print(1000)
  Print("lets fly")
  ```
* SleepCommand:Sleeps for a given number of milliseconds. can be an existing variable or math expressions.
   The string token key is ```Sleep```
   The first next token is the parameter

 example:
  ```
  Sleep(1000)
  Sleep(alt)
  ```
* DefineVarCommand:Defining a new variable.
  The "syntax" is : ``` var varName = OR -> OR -< sim("/simulatorpath/to/value")```.
  varName can start with _ and a letter char and then numbers and letters.
  -> means we update the simulator at a given path, <- means we read from the simulator the matched value by the given path,
  = mean we want to assign a value or a value of expression..
  The string token key is ```var```
  The next 3 tokens are the name, binding direction or a = and an expression or simulator path.
 
 example:
  ```
  var magnetos -> sim("/controls/switches/magnetos")
  var h0 = heading
  ```
* VarAssignmentCommand: Assigning a new value to an existing variable.
  The string token key is a var name that exists in the symboltable.
  the next token is the value or an exprssion.
 
 example:
  ```
  h0 = heading
  ```
* IfCommand:Implements an if conditoin.
  The string token is ```if```.
  The next 2 tokens are :The boolean expression the boolean opertaor can be  == != < > <= >=
  and a openning {.
  After the innercommands comes a closing }.
 
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
  The string token is ```while```.
  The next 2 tokens are :The boolean expression the boolean opertaor can be  == != < > <= >=
  and a openning {.
  After the innercommands comes a closing }.
 
 example:
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
  Both LoopCommand and IfCommand inherits a class (that inherits The Command class) called ConditionParser,
  that has a member of ```vector<Command*>``` as the inner commands. In the execute function, the tokens vector will be iterated and inner commands that are between the {} will be inserted to the vector untill we find a }.
  If and while loops cannot contain new varaibles definings, an inner if and whiles, and a new function defining.
  
* DefineFuncCommand:Defining a new function and inserts it as a new command (instance of FuncCommand)to the
```commands_map```.
    The string token is not an exisitng commnad string key or "var" or "if" or "while" or a var name.
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
* FuncCommand:Executes a costume function. Can get one parameter as a number.
  The string token is the name of the function.
  The next token is the parameter number.
 
 example
  ```
  takeoff(1000)
  ```

So, first we want to open a server at a given port ,and wait for the simulator to connect as a client. Using the      ```OpenServerCommand```, we initialize a thread of executing the ```OpenServerCommand```, so the server that we open will always run in the background.
The simulator sends 10 times in a second 36 value separated by commas(The simulator used the generic_small.xml file from previous topic), and once the simulator connects, we'll initialize a thread that start reading the values and updating the matched values in the matched indexes bt separating each line of 36 values. 
In the meantime, we initialize a thread of ```ConnectCommand``` that connects us as a clinet (by a given IP and a port) to the simulator that will be used as our server and that thread will be always running so we can send a request to set a new value to a simulator path.

In this project the local host is a server and a client.

After those, we can continue iterating over the tokens vector and indetify wanted command by sending the ```tokensVector.at(index)``` to the command map.

Once we get to part of executing the PrintCommand with the string "done" the program will finish.


### Possible changes / improvments
-Implementing Singleton DP on the Parser,SymbolTable and the InterpretTool.
-Instead of sending the tokens vector and the current index in the ```execute``` function, send an iterator of the vector.

# Part #2
In this second milestone of the project, we'll


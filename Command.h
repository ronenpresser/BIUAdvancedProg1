//
// Created by amit on 20/12/2019.
//


#ifndef BIUADVANCEDPROG1__COMMAND_H_
#define BIUADVANCEDPROG1__COMMAND_H_

#include <string>
#include <vector>
#include "InterpretTool.h"
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>

using namespace std;

class Parser;

class Command {
 public:
  virtual int execute(vector<string> &, int , Parser *) =0;
  virtual ~Command() {}
};

class DefineVarCommand : public Command {
 public:
  int execute(vector<string> &tokensVector, int currentIndex, Parser *pars);
  virtual ~DefineVarCommand() {}
};

class PrintCommand : public Command {
 public:
  int execute(vector<string> &tokensVector, int currentIndex, Parser *pars);
  virtual ~PrintCommand() {}
};

class SleepCommand : public Command {
 public:
  int execute(vector<string> &tokensVector, int currentIndex, Parser *pars);
  virtual ~SleepCommand() {}
};

class ConditionParser : public Command {
 private:
  vector<Command *> inner_commands;
 public:
  virtual int execute(vector<string> &tokensVector, int currentIndex, Parser *pars) = 0;

  bool isConditionTrue(string conditionString, Parser &pars);

  void insert_to_inner_commands(Command *c);

  vector<Command *> getInnerCommands();

  void buildCommandsVector(vector<string> &tokensVec, Parser *parser, int index);

  virtual ~ConditionParser() {
    if (!inner_commands.empty())
      inner_commands.clear();
  }
};

class IfCommand : public ConditionParser {
 public:
  int execute(vector<string> &tokensVector, int currentIndex, Parser *pars);
  virtual ~IfCommand() {}

};

class LoopCommand : public ConditionParser {
 public:
  int execute(vector<string> &tokensVector, int currentIndex, Parser *pars);
  virtual ~LoopCommand() {}
};

class VarAssignmentCommand : public Command {
 public:
  int execute(vector<string> &tokensVector, int currentIndex, Parser *pars);
  virtual ~VarAssignmentCommand() {}
};

class OpenServerCommand : public Command {
 public:
  int execute(vector<string> &tokensVec, int currIndex, Parser *parser);
  virtual ~OpenServerCommand() {}
};

class ConnectCommand : public Command {
 public:
  int execute(vector<string> &tokensVec, int currIndex, Parser *parser);
  virtual  ~ConnectCommand() {}

};

#endif //BIUADVANCEDPROG1__COMMAND_H_

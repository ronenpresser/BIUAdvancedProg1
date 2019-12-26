//
// Created by amit on 20/12/2019.
//


#ifndef BIUADVANCEDPROG1__COMMAND_H_
#define BIUADVANCEDPROG1__COMMAND_H_

#include <string>
#include <vector>
#include "InterpretTool.h"

using namespace std;

class Parser;

class Command {
 public:
  virtual int execute(vector<string> &tokensVector, int currentIndex, Parser *pars) {}
  const vector<string> splitByChar(string wholeString, char delimter);
};

class SleepCommand : public Command {
 public:
  int execute(vector<string> &tokensVector, int currentIndex, Parser *pars);
};

class ConditionParser : public Command {
 private:
  vector<Command> inner_commands;
 public:
  virtual int execute(vector<string> &tokensVector, int currentIndex, Parser *pars) {}

  bool isConditionTrue(string conditionString, Parser &pars);

  void insert_to_inner_commands(Command c);

  vector<Command> getInnerCommands();

  void buildCommandsVector(vector<string> &tokensVec, Parser *parser, int index);
};

class IfCommand : public ConditionParser {
 public:
  int execute(vector<string> &tokensVector, int currentIndex, Parser *pars);

};

class LoopCommand : public ConditionParser {
 public:
  int execute(vector<string> &tokensVector, int currentIndex, Parser *pars);
};

class VarAssignmentCommand : public Command {
 public:
  int execute(vector<string> &tokensVector, int currentIndex, Parser *pars);
};

class OpenServerCommand : public Command {
 public:
  int execute(vector<string> &tokensVec, int currIndex, Parser *parser);
};

class ConnectCommand : public Command {
 public:
  int execute(vector<string> &tokensVec, int currIndex, Parser *parser);

};

#endif //BIUADVANCEDPROG1__COMMAND_H_

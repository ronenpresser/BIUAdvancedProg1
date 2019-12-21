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

};

class IfCommand : public ConditionParser {
 public:
  int execute(vector<string> &tokensVector, int currentIndex, Parser *pars);

};

class LoopCommand : public ConditionParser {
 public:
  int execute(vector<string> &tokensVector, int currentIndex, Parser *pars);
};

#endif //BIUADVANCEDPROG1__COMMAND_H_
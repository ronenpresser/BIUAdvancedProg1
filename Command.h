//
// Created by amit on 20/12/2019.
//


#ifndef BIUADVANCEDPROG1__COMMAND_H_
#define BIUADVANCEDPROG1__COMMAND_H_

#include <list>
#include <string>
#include <vector>
#include "InterpretTool.h"

using namespace std;

class Command {
 public:
  virtual int execute(vector<string> &tokensVector, int currentIndex, InterpretTool &interpreter) {}
};

class SleepCommand : public Command {
 public:
  int execute(vector<string> &tokensVector, int currentIndex, InterpretTool &interpreter);
};

class ConditionParser : public Command {
 private:
  list<Command> inner_commands;
 public:
  virtual int execute(vector<string> &tokensVector, int currentIndex, InterpretTool &interpret);
  bool isConditionTrue(string conditionString, InterpretTool &interpret) {
    return (interpret).interpretBoolExpression(conditionString)->calculate();
  }
  void setInnerCommands(list<Command> commandsList) {
    this->inner_commands = commandsList;
  }
  list<Command> getInnerCommands() {
    return inner_commands;
  }

};

class IfCommand : public ConditionParser {
 public:
  int execute(vector<string> &tokensVector, int currentIndex, InterpretTool &interpret);

};

class LoopCommand : public ConditionParser {
 public:
  int execute(vector<string> &tokensVector, int currentIndex, InterpretTool &interpret);
};

#endif //BIUADVANCEDPROG1__COMMAND_H_

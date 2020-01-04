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
#include <unordered_map>

using namespace std;

class Parser;

class Command {
 public:
  virtual int execute(vector<string> &, int, Parser *) = 0;
  virtual int getSteps() = 0;
  virtual ~Command() {}
};

class DefineVarCommand : public Command {
 public:
  static const int STEPS = 4;
  int execute(vector<string> &tokensVector, int currentIndex, Parser *pars);
  int getSteps() {
    return STEPS;
  }
  virtual ~DefineVarCommand() {}
};

class PrintCommand : public Command {
 public:
  static const int STEPS = 2;

  int execute(vector<string> &tokensVector, int currentIndex, Parser *pars);
  int getSteps() {
    return STEPS;
  }
  virtual ~PrintCommand() {}
};

class SleepCommand : public Command {
 public:
  static const int STEPS = 2;
  int execute(vector<string> &tokensVector, int currentIndex, Parser *pars);
  int getSteps() {
    return STEPS;
  }
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
  virtual int getSteps() = 0;

  virtual ~ConditionParser() {
    if (!inner_commands.empty())
      inner_commands.clear();
  }
};

class IfCommand : public ConditionParser {

 public:
  static const int STEPS = 3;

  int execute(vector<string> &tokensVector, int currentIndex, Parser *pars);
  int getSteps() {
    return STEPS;
  }
  virtual ~IfCommand() {}

};

class LoopCommand : public ConditionParser {
 public:
  static const int STEPS = 3;
  int getSteps() {
    return STEPS;
  }
  int execute(vector<string> &tokensVector, int currentIndex, Parser *pars);
  virtual ~LoopCommand() {}
};

class VarAssignmentCommand : public Command {
 public:
  static const int STEPS = 2;
  int getSteps() {
    return STEPS;
  }
  int execute(vector<string> &tokensVector, int currentIndex, Parser *pars);
  virtual ~VarAssignmentCommand() {}
};

class OpenServerCommand : public Command {
 public:
  static const int STEPS = 2;
  int getSteps() {
    return STEPS;
  }
  int execute(vector<string> &tokensVec, int currIndex, Parser *parser);
  virtual ~OpenServerCommand() {}
};

class ConnectCommand : public Command {
 public:
  static const int STEPS = 2;
  int getSteps() {
    return STEPS;
  }
  int execute(vector<string> &tokensVec, int currIndex, Parser *parser);
  virtual  ~ConnectCommand() {}

};

class DefineFuncCommand : public Command {
 public:
  static const int STEPS = 2;
  int getSteps() {
    return STEPS;
  }
  int execute(vector<string> &tokensVec, int currIndex, Parser *parser);

  virtual  ~DefineFuncCommand() {}

};

class FuncCommand : public Command {
 private:
  vector<Command *> inner_commands;
  unordered_map<string, Variable *> parameters;
  unsigned int startingIndex;
 public:

  static const int STEPS = 3;
  int getSteps() {
    return STEPS;
  }
  int execute(vector<string> &tokensVec, int currIndex, Parser *parser);

  virtual  ~FuncCommand() {
    if (!inner_commands.empty()) {
      this->inner_commands.clear();
    }
    if (!parameters.empty()) {
      this->parameters.clear();
    }
  }

  friend class DefineFuncCommand;

};

#endif //BIUADVANCEDPROG1__COMMAND_H_

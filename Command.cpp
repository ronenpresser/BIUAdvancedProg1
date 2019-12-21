//
// Created by amit on 20/12/2019.
//

#include "Command.h"
#include "InterpretTool.h"
#include <list>

int IfCommand::execute(vector<string> &tokensVec, int currIndex, InterpretTool &interpreter) {
  int steps = 0;

  if (isConditionTrue(tokensVec.at(currIndex + 1), interpreter)) {
    list<Command> commands = getInnerCommands();
    for (Command c : commands) {
      steps += c.execute(tokensVec, currIndex, interpreter);
    }
  }

  return steps;
}

int LoopCommand::execute(vector<string> &tokensVec, int currIndex, InterpretTool &interpreter) {
  int steps = 0;

  while (isConditionTrue(tokensVec.at(currIndex + 1), interpreter)) {
    list<Command> commands = getInnerCommands();
    for (Command c : commands) {
      steps += c.execute(tokensVec, currIndex, interpreter);
    }
  }

  return steps;

}
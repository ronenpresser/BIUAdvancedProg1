//
// Created by amit on 20/12/2019.
//

#include "Command.h"
#include "InterpretTool.h"
#include "Parser.h"
#include <list>
#include <thread>
#include <mutex>
#include <algorithm>

static int const STEPS_FOR_ONE_PARAMETER_COMMAND = 2;
static int const STEPS_FOR_TWO_PARAMETERS_COMMAND = 3;
static int const STEPS_FOR_IF_OR_LOOP_COMMAND = 3;

void sleep(int sleepParam) {
  //while (!parser->isSleep()) {}
  mutex m;
  m.lock();
  //int milliSeconds = stoi(parser->getSleepMilliSeconds());
  this_thread::sleep_for(chrono::milliseconds(sleepParam));
  //parser->wake();
  m.unlock();
}
int SleepCommand::execute(vector<string> &tokensVector, int currentIndex, Parser *parser) {

  int sleepParameter(stoi(tokensVector.at(currentIndex + 1)));
  //parser->setSleepMilliSeconds(sleepParameter);
  //parser->sleep();
  thread sleepThread(sleep, sleepParameter);
  sleepThread.join();

  return STEPS_FOR_ONE_PARAMETER_COMMAND;

}

int IfCommand::execute(vector<string> &tokensVec, int currIndex, Parser *parser) {
  int steps = STEPS_FOR_IF_OR_LOOP_COMMAND;

  int index = currIndex + 3;
  buildCommandsVector(tokensVec, parser, index);

  if (isConditionTrue(tokensVec.at(currIndex + 1), *parser)) {
    vector<Command> commands = getInnerCommands();
    for (Command c : commands) {
      steps += c.execute(tokensVec, currIndex, parser);
    }
  }

  return steps + 1;
}

int LoopCommand::execute(vector<string> &tokensVec, int currIndex, Parser *parser) {
  int steps = STEPS_FOR_IF_OR_LOOP_COMMAND;
  int index = currIndex + 3;
  buildCommandsVector(tokensVec, parser, index);
  while (isConditionTrue(tokensVec.at(currIndex + 1), *parser)) {
    vector<Command> commands = getInnerCommands();
    for (Command c : commands) {
      steps += c.execute(tokensVec, currIndex, parser);
    }
  }

  return steps + 1;
}
void ConditionParser::buildCommandsVector(vector<string> &tokensVec, Parser *parser, int index) {
  while (tokensVec.at(index) != "}") {
    string token = tokensVec.at(index);
    string lowerCaseLine = token;
    transform(lowerCaseLine.begin(),
              lowerCaseLine.end(),
              lowerCaseLine.begin(),
              ::tolower);
    if (parser->isExistsInCommandsMap(lowerCaseLine)) {
      this->insert_to_inner_commands(*parser->getCommand(token));
    } else if (parser->isExistsInSymbolTable(token)) {
      //TODO: add a Command for assignment of the var.
    }
//    if (tokensVec.at(index) == "var") {
//      index++;
//      string name = tokensVec.at(index);
//      index++;
//      float value = stod(tokensVec.at(index));
//      index++;
//      bool bindDirec = tokensVec.at(index) == "->" ? false : true;
//      index++;
//      string path = tokensVec.at(index);
//      parser->insert_to_symbol_table(name, value, path, bindDirec);
//    }

  }
}

bool ConditionParser::isConditionTrue(string conditionString, Parser &pars) {
  auto par = pars;
  return par.getInterpreter()->interpretBoolExpression(conditionString)->calculate() == 1 ? true : false;
}

void ConditionParser::insert_to_inner_commands(Command c) {
  this->inner_commands.push_back(c);
}
vector<Command> ConditionParser::getInnerCommands() {
  return inner_commands;
}
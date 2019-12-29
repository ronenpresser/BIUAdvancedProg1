//
// Created by amit on 14/12/2019.
//

#ifndef BIUADVANCEDPROG1__PARSER_H_
#define BIUADVANCEDPROG1__PARSER_H_
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include "InterpretTool.h"

using namespace std;

class SymbolTable {
 private:
  map<string, Variable *> symbol_map;
 public:

  void setValue(string varName, float value);
  void insert(Variable *var);
  void clear();
  bool empty();
  bool count(string key);
  Variable *getVariable(string key);
};

class Command;
class Parser {
 private:
  map<string, Command *> commands_map;
  SymbolTable symbol_table;
  map<string, Variable *> simPathToVarMap;
  map<int, string> indexToSimPathMap;
  InterpretTool *interpret_tool;


 public:
  Parser() {
    buildMaps();
    interpret_tool = new InterpretTool();
  }
  void parse(vector<string> tokensVector);
  void buildMaps();
  InterpretTool *getInterpreter();
  void insert_to_symbol_table(string varName, float val, string path, bool bindingDirection);
  Command *getCommand(string key);
  bool isExistsInCommandsMap(string key);
  bool isExistsInSymbolTable(string key);
  bool isBindingDirectionLeft(string varName);
  float getValueOfSymbol(string varName) {
      return this->symbol_table.getVariable(varName)->getValue();
  }
  string getSimulatorPathByIndex(int index){
      return this->indexToSimPathMap[index];
  }
  string getSimulatorPathByVarName(string varName) {

      return this->symbol_table.getVariable(varName)->getSimulatorPath();
  }

  virtual ~Parser();

  //friend class Command;

  void updateValue(string varName, float newVal);
};

#endif //BIUADVANCEDPROG1__PARSER_H_


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
  map<string, Variable *> simPathToVarMap; //TODO change to multimap
  map<int, string> indexToSimPathMap;
  InterpretTool *interpret_tool;

 public:

  bool canProceedParsing;
  bool shouldStopParsing;
  Parser() {
    buildMaps();
    interpret_tool = new InterpretTool();
    canProceedParsing = false;
    shouldStopParsing = false;
  }
  void parse(vector<string> tokensVector);
  void buildMaps();
  InterpretTool *getInterpreter();
  void insert_to_symbol_table(string varName, float val, string path, bool bindingDirection, bool isVarAssignment);
  void insertParameterToSymbolTable(Variable *variable);
  Command *getCommand(string key);
  bool isExistsInCommandsMap(string key);
  bool isExistsInSymbolTable(string key);
  bool isBindingDirectionLeft(string varName);
  float getValueOfSymbol(string varName);
  string getSimulatorPathByIndex(int index);
  string getSimulatorPathByVarName(string varName);
  bool getIsLocalVar(string varName);
  void updateValue(string varName, float newVal);
  string getNameOfSymbolBySimulatorPath(string path);
  virtual ~Parser();
  void insertFuncCommandToCommandsMap(string funcName, Command *c);

};

#endif //BIUADVANCEDPROG1__PARSER_H_

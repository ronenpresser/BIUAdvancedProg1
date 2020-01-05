
#ifndef BIUADVANCEDPROG1__PARSER_H_
#define BIUADVANCEDPROG1__PARSER_H_
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include "InterpretTool.h"
#include <unordered_map>
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

  friend class Parser;
};

class Command;
class FuncCommand;
class Parser {
 private:
  map<string, Command *> commands_map;
  SymbolTable symbol_table;
  multimap<string, Variable *> simPathToVarMap;
  map<int, string> indexToSimPathMap;
  InterpretTool *interpret_tool;
  void eraseSymbolFromTableByName(string name);
  void eraseParametersOfFunc(unordered_map<string, Variable *> parameters);

 public:

  bool canProceedParsing;
  bool shouldStopParsing;
  void buildMaps();

  Parser() {
    buildMaps();
    interpret_tool = new InterpretTool();
    canProceedParsing = false;
    shouldStopParsing = false;
  }
  void parse(vector<string> tokensVector);
  InterpretTool *getInterpreter();
  void insert_to_symbol_table(string varName, float val, string path, bool bindingDirection, bool isLocalVariable);
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
  vector<std::__cxx11::string> getNamesOfSymbolsBySimulatorPath(string path);
  void insertFuncCommandToCommandsMap(string funcName, Command *c);

  virtual ~Parser();

  friend class FuncCommand;
};

#endif //BIUADVANCEDPROG1__PARSER_H_

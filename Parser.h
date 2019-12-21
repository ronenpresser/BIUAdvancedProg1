//
// Created by amit on 14/12/2019.
//

#ifndef BIUADVANCEDPROG1__PARSER_H_
#define BIUADVANCEDPROG1__PARSER_H_
#include <string>
#include <map>
#include <vector>
#include "InterpretTool.h"

using namespace std;

class SymbolTable {
 private:
  map<string, Variable *> symbol_map;
 public:

  void setValue(string varName, float value);
  void insert(string varName, float val, string path, bool bindingDirec);
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
  map<string, Variable *> sim_var_table;
  map<int, string> index_sim_table;
  InterpretTool *interpret_tool;

 public:
  Parser() {
    buildMaps();
    interpret_tool = new InterpretTool();
  }

  void parse(vector<string> *tokensVector);
  void buildMaps();
  InterpretTool *getInterpreter();
  void insert_to_sim_var_table(string sim, Variable *var);
  void insert_to_symbol_table(string varName, float val, string path, bool bindingDirection);
  Command* getCommand(string key);
  bool isExistsInCommandsMap(string key);
  virtual ~Parser();

  //friend class Command;
  bool isExistsInSymbolTable(string key);
};

#endif //BIUADVANCEDPROG1__PARSER_H_

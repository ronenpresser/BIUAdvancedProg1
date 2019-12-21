//
// Created by amit on 14/12/2019.
//

#ifndef BIUADVANCEDPROG1__PARSER_H_
#define BIUADVANCEDPROG1__PARSER_H_
#include <string>
#include <map>
#include <vector>
#include "Command.h"

using namespace std;

class Symbol {
 private:
  float value;
  string sim_path;
 public:
  void setValue(float value) {
    this->value = value;
  }

};

class SymbolTable {
 private:
  map<string, Variable *> symbol_map;
 public:

  void setValue(string varName, float value) {
    if (this->symbol_map.count(varName)) {
      this->symbol_map[varName]->setValue(value);
    }
  }
  void insert(string varName, float val, string path) {
    Variable *var = new Variable(varName, val, path);
    this->symbol_map.insert(make_pair(varName, var));
    delete var;
  }
  void clear() {
    this->symbol_map.clear();
  }

};

class Parser {
 private:
  map<string, Command> commands_map;
  SymbolTable symbol_table;
  map<string, Variable> sim_var_table;
  map<int, string> index_sim_table;
  InterpretTool *interpret_tool;

 public:
  Parser() {
    buildMaps();
    interpret_tool = new InterpretTool();
  }

  void parse(vector<string> *tokensVector);
  void buildMaps();
  InterpretTool *getInterpreter() {
    return this->interpret_tool;
  }
  virtual ~Parser() {
    commands_map.clear();
    symbol_table.clear();
    delete interpret_tool;
  }
};

#endif //BIUADVANCEDPROG1__PARSER_H_

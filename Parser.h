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
  map<string, Symbol *> symbol_map;
 public:

  void setValue(string varName, float value) {
    if (this->symbol_map.count(varName)) {
      this->symbol_map[varName]->setValue(value);
    }
  }
  void insert(string varName, Symbol *symbol) {
    this->symbol_map.insert(make_pair(varName, symbol));
  }
  void clear() {
    this->symbol_map.clear();
  }

};

class Parser {
 private:
  map<string, Command *> commands_map;
  SymbolTable symbol_table;
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
  ~Parser() {
    commands_map.clear();
    symbol_table.clear();
    delete interpret_tool;
  }
};

#endif //BIUADVANCEDPROG1__PARSER_H_

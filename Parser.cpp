//
// Created by amit on 14/12/2019.
//

#include "Parser.h"

void Parser::parse(vector<string> *tokensVec) {

  int index = 0;

  while (index < tokensVec->size()) {
    if (this->commands_map.count(tokensVec->at(index))) {
      Command *c = this->commands_map[tokensVec->at(index)];
      index += c->execute(*tokensVec, index,this->interpret_tool);
    }
  }
}

void Parser::buildMaps() {

  this->commands_map.insert(make_pair("if", new IfCommand()));
  LoopCommand loop_command;
  this->commands_map.insert(make_pair("while", new LoopCommand));

  this->symbol_table.insert(, "instrumentation/airspeed-indicator/indicated-speed-kt");
  this->symbol_table.insert();
  this->symbol_table.insert();
  this->symbol_table.insert();
  this->symbol_table.insert();
  this->symbol_table.insert();
  this->symbol_table.insert();
  this->symbol_table.insert();
  this->symbol_table.insert();
  this->symbol_table.insert();
  this->symbol_table.insert();
  this->symbol_table.insert();
  this->symbol_table.insert();
  this->symbol_table.insert();

}
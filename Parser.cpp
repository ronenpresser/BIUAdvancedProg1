//
// Created by amit on 14/12/2019.
//

#include <algorithm>
#include "Parser.h"
#include "Command.h"
void Parser::parse(vector<string> *tokensVec) {

  int index = 0;

  while (index < tokensVec->size()) {
    string lowerCaseLine = tokensVec->at(index);
    transform(lowerCaseLine.begin(),
              lowerCaseLine.end(),
              lowerCaseLine.begin(),
              ::tolower);
    if (this->commands_map.count(lowerCaseLine)) {
      Command *c = this->commands_map[lowerCaseLine];
      index += c->execute(*tokensVec, index, this);
    } else if (this->symbol_table.count(tokensVec->at(index))) {

    }
  }
}

void Parser::buildMaps() {

  this->commands_map.insert(make_pair("sleep", new SleepCommand()));
  this->commands_map.insert(make_pair("if", new IfCommand()));
  this->commands_map.insert(make_pair("while", new LoopCommand()));

  this->index_sim_table.insert(make_pair(0, "/instrumentation/airspeed-indicator/indicated-speed-kt"));
  this->index_sim_table.insert(make_pair(1, "/sim/time/warp"));
  this->index_sim_table.insert(make_pair(2, "/controls/switches/magnetos"));
  this->index_sim_table.insert(make_pair(3, "/instrumentation/heading-indicator/offset-deg"));
  this->index_sim_table.insert(make_pair(4, "/instrumentation/altimeter/indicated-altitude-ft"));
  this->index_sim_table.insert(make_pair(5, "/instrumentation/altimeter/pressure-alt-ft"));
  this->index_sim_table.insert(make_pair(6, "/instrumentation/attitude-indicator/indicated-pitch-deg"));
  this->index_sim_table.insert(make_pair(7, "/instrumentation/attitude-indicator/indicated-roll-deg"));
  this->index_sim_table.insert(make_pair(8, "/instrumentation/attitude-indicator/internal-pitch-deg"));
  this->index_sim_table.insert(make_pair(9, "/instrumentation/attitude-indicator/internal-roll-deg"));
  this->index_sim_table.insert(make_pair(10, "/instrumentation/encoder/indicated-altitude-ft"));
  this->index_sim_table.insert(make_pair(11, "/instrumentation/encoder/pressure-alt-ft"));
  this->index_sim_table.insert(make_pair(12, "/instrumentation/gps/indicated-altitude-ft"));
  this->index_sim_table.insert(make_pair(13, "/instrumentation/gps/indicated-ground-speed-kt"));
  this->index_sim_table.insert(make_pair(14, "/instrumentation/gps/indicated-vertical-speed"));
  this->index_sim_table.insert(make_pair(15, "/instrumentation/heading-indicator/indicated-heading-deg"));
  this->index_sim_table.insert(make_pair(16, "/instrumentation/magnetic-compass/indicated-heading-deg"));
  this->index_sim_table.insert(make_pair(17, "/instrumentation/slip-skid-ball/indicated-slip-skid"));
  this->index_sim_table.insert(make_pair(18, "/instrumentation/turn-indicator/indicated-turn-rate"));
  this->index_sim_table.insert(make_pair(19, "/instrumentation/vertical-speed-indicator/indicated-speed-fpm"));
  this->index_sim_table.insert(make_pair(20, "/controls/flight/aileron"));
  this->index_sim_table.insert(make_pair(21, "/controls/flight/elevator"));
  this->index_sim_table.insert(make_pair(22, "/controls/flight/rudder"));
  this->index_sim_table.insert(make_pair(23, "/controls/flight/flaps"));
  this->index_sim_table.insert(make_pair(24, "/controls/engines/engine/throttle"));
  this->index_sim_table.insert(make_pair(25, "/controls/engines/current-engine/throttle"));
  this->index_sim_table.insert(make_pair(26, "/controls/switches/master-avionics"));
  this->index_sim_table.insert(make_pair(27, "/controls/switches/starter"));
  this->index_sim_table.insert(make_pair(28, "/engines/active-engine/auto-start"));
  this->index_sim_table.insert(make_pair(29, "/controls/flight/speedbrake"));
  this->index_sim_table.insert(make_pair(30, "/sim/model/c172p/brake-parking"));
  this->index_sim_table.insert(make_pair(31, "/controls/engines/engine/primer"));
  this->index_sim_table.insert(make_pair(32, "/controls/engines/current-engine/mixture"));
  this->index_sim_table.insert(make_pair(33, "/controls/switches/master-bat"));
  this->index_sim_table.insert(make_pair(34, "/controls/switches/master-alt"));
  this->index_sim_table.insert(make_pair(35, "/engines/engine/rpm"));
}

void SymbolTable::setValue(string varName, float value) {
  if (this->symbol_map.count(varName)) {
    this->symbol_map[varName]->setValue(value);
  }
}
void SymbolTable::insert(string varName, float val, string path, bool bindingDirec) {
  Variable *var = new Variable(varName, val, path, bindingDirec);
  this->symbol_map.insert(make_pair(varName, var));
  delete var;
}
void SymbolTable::clear() {
  this->symbol_map.clear();
}
bool SymbolTable::empty() {
  return this->symbol_map.empty();
}
Variable *SymbolTable::getVariable(string key) {
  return this->symbol_map[key];
}
bool SymbolTable::count(string key) {
  return this->symbol_map.count(key);
}
InterpretTool *Parser::getInterpreter() {
  return interpret_tool;
}

void Parser::insert_to_sim_var_table(string sim, Variable *var) {
  this->sim_var_table.insert(make_pair(sim, var));
}
void Parser::insert_to_symbol_table(string varName, float val, string path, bool bindingDirec) {
  this->symbol_table.insert(varName, val, path, bindingDirec);
}

Command *Parser::getCommand(string key) {
  return commands_map[key];
}
bool Parser::isExistsInCommandsMap(string key) {
  return commands_map.count(key);
}

bool Parser::isExistsInSymbolTable(string key) {
  return symbol_table.count(key);
}
void Parser::sleep() {
  shouldSleep = true;
}
void Parser::wake() {
  shouldSleep = false;
}
bool Parser::isSleep() {
  return shouldSleep;
}
void Parser::setSleepMilliSeconds(int seconds) {
  this->sleepMilliSeconds = seconds;
}
string Parser::getSleepMilliSeconds() {
  return sleepMilliSeconds;
}
Parser::~Parser() {
  if (!commands_map.empty()) {
    commands_map.clear();
  }
  if (!symbol_table.empty()) {
    symbol_table.clear();
  }
  if (!sim_var_table.empty()) {
    sim_var_table.clear();
  }
  if (!index_sim_table.empty()) {
    index_sim_table.clear();
  }
  if (interpret_tool != nullptr)
    delete interpret_tool;
}
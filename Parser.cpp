//
// Created by amit on 14/12/2019.
//

#include <algorithm>
#include <thread>
#include "Parser.h"
#include "Command.h"

void openServerFunc(Command *c, vector<string> tokensVec, int currIndex, Parser *parser) {
  c->execute(tokensVec, currIndex, parser);
}

void connectClientFunc(Command *c, vector<string> tokensVec, int currIndex, Parser *parser) {
  c->execute(tokensVec, currIndex, parser);
}

void Parser::parse(vector<string> tokensVec) {
  unsigned int index = 0;
  //open the server
  string token = tokensVec.at(index);
//  string lowerCaseLine = tokensVec.at(index);
//  transform(lowerCaseLine.begin(),
//            lowerCaseLine.end(),
//            lowerCaseLine.begin(),
//            ::tolower);
  thread openServer(openServerFunc, this->commands_map[token], tokensVec, index, this);
  index += 2;

  //connect
  token = tokensVec.at(index);
//  transform(lowerCaseLine.begin(),
//            lowerCaseLine.end(),
//            lowerCaseLine.begin(),
//            ::tolower);
  thread connectClient(connectClientFunc, this->commands_map[token], tokensVec, index, this);

  index += 3;

  while (!this->canProceedParsing) { this_thread::sleep_for(400ms); }
  cout << "after server and client" << endl;
  while (index < tokensVec.size()) {
    token = tokensVec.at(index);
//    transform(lowerCaseLine.begin(),
//              lowerCaseLine.end(),
//              lowerCaseLine.begin(),
//              ::tolower);
    if (this->commands_map.count(token)) {
      Command *c = this->commands_map[token];
      index += c->execute(tokensVec, index, this);
    } else if (this->symbol_table.count(token)) {
      Command *c = this->commands_map["="];
      index += c->execute(tokensVec, index, this);
    }
  }

  openServer.join();
  connectClient.join();
}

void Parser::buildMaps() {
  this->commands_map.insert(make_pair("openDataServer", new OpenServerCommand()));
  this->commands_map.insert(make_pair("connectControlClient", new ConnectCommand()));
  this->commands_map.insert(make_pair("Sleep", new SleepCommand()));
  this->commands_map.insert(make_pair("Print", new PrintCommand()));
  this->commands_map.insert(make_pair("var", new DefineVarCommand()));
  this->commands_map.insert(make_pair("=", new VarAssignmentCommand()));
  this->commands_map.insert(make_pair("if", new IfCommand()));
  this->commands_map.insert(make_pair("while", new LoopCommand()));

  this->indexToSimPathMap.insert(make_pair(0,"instrumentation/airspeed-indicator/indicated-speed-kt"));
  this->indexToSimPathMap.insert(make_pair(1,"sim/time/warp"));
  this->indexToSimPathMap.insert(make_pair(2,"controls/switches/magnetos"));
  this->indexToSimPathMap.insert(make_pair(3,"instrumentation/heading-indicator/offset-deg"));
  this->indexToSimPathMap.insert(make_pair(4,"instrumentation/altimeter/indicated-altitude-ft"));
  this->indexToSimPathMap.insert(make_pair(5,"instrumentation/altimeter/pressure-alt-ft"));
  this->indexToSimPathMap.insert(make_pair(6,"instrumentation/attitude-indicator/indicated-pitch-deg"));
  this->indexToSimPathMap.insert(make_pair(7,"instrumentation/attitude-indicator/indicated-roll-deg"));
  this->indexToSimPathMap.insert(make_pair(8,"instrumentation/attitude-indicator/internal-pitch-deg"));
  this->indexToSimPathMap.insert(make_pair(9,"instrumentation/attitude-indicator/internal-roll-deg"));
  this->indexToSimPathMap.insert(make_pair(10,"instrumentation/encoder/indicated-altitude-ft"));
  this->indexToSimPathMap.insert(make_pair(11,"instrumentation/encoder/pressure-alt-ft"));
  this->indexToSimPathMap.insert(make_pair(12,"instrumentation/gps/indicated-altitude-ft"));
  this->indexToSimPathMap.insert(make_pair(13,"instrumentation/gps/indicated-ground-speed-kt"));
  this->indexToSimPathMap.insert(make_pair(14,"instrumentation/gps/indicated-vertical-speed"));
  this->indexToSimPathMap.insert(make_pair(15,"instrumentation/heading-indicator/indicated-heading-deg"));
  this->indexToSimPathMap.insert(make_pair(16,"instrumentation/magnetic-compass/indicated-heading-deg"));
  this->indexToSimPathMap.insert(make_pair(17,"instrumentation/slip-skid-ball/indicated-slip-skid"));
  this->indexToSimPathMap.insert(make_pair(18,"instrumentation/turn-indicator/indicated-turn-rate"));
  this->indexToSimPathMap.insert(make_pair(19,"instrumentation/vertical-speed-indicator/indicated-speed-fpm"));
  this->indexToSimPathMap.insert(make_pair(20,"controls/flight/aileron"));
  this->indexToSimPathMap.insert(make_pair(21,"controls/flight/elevator"));
  this->indexToSimPathMap.insert(make_pair(22,"controls/flight/rudder"));
  this->indexToSimPathMap.insert(make_pair(23,"controls/flight/flaps"));
  this->indexToSimPathMap.insert(make_pair(24,"controls/engines/engine/throttle"));
  this->indexToSimPathMap.insert(make_pair(25,"controls/engines/current-engine/throttle"));
  this->indexToSimPathMap.insert(make_pair(26,"controls/switches/master-avionics"));
  this->indexToSimPathMap.insert(make_pair(27,"controls/switches/starter"));
  this->indexToSimPathMap.insert(make_pair(28,"engines/active-engine/auto-start"));
  this->indexToSimPathMap.insert(make_pair(29,"controls/flight/speedbrake"));
  this->indexToSimPathMap.insert(make_pair(30,"sim/model/c172p/brake-parking"));
  this->indexToSimPathMap.insert(make_pair(31,"controls/engines/engine/primer"));
  this->indexToSimPathMap.insert(make_pair(32,"controls/engines/current-engine/mixture"));
  this->indexToSimPathMap.insert(make_pair(33,"controls/switches/master-bat"));
  this->indexToSimPathMap.insert(make_pair(34,"controls/switches/master-alt"));
  this->indexToSimPathMap.insert(make_pair(35,"engines/engine/rpm"));


}

void SymbolTable::setValue(string varName, float value) {
  Variable *var = symbol_map[varName];
  if(var != nullptr)
    var->setValue(value);
}
void SymbolTable::insert(Variable *var) {

  this->symbol_map.insert(make_pair(var->getName(), var));
}
void SymbolTable::clear() {
  this->symbol_map.clear();
}
bool SymbolTable::empty() {
  return this->symbol_map.empty();
}
Variable *SymbolTable::getVariable(string key) {
  Variable *var = this->symbol_map[key];
  return var;
}
bool SymbolTable::count(string key) {
  return this->symbol_map.count(key);
}
InterpretTool *Parser::getInterpreter() {
  return interpret_tool;
}

void Parser::insert_to_symbol_table(string varName, float val, string path, bool bindingDirec, bool isVarAssgin) {
  Variable *var = new Variable(varName, val, path, bindingDirec, isVarAssgin);
  this->symbol_table.insert(var);
  this->simPathToVarMap.insert(make_pair(var->getSimulatorPath(), var));
  this->interpret_tool->setVariables(varName + "=" + to_string(val));
  //delete var;
}

void Parser::updateValue(string varName, float newVal) {
  if (isExistsInSymbolTable(varName)) {
    this->symbol_table.setValue(varName, newVal);
    string sVal = to_string(newVal);
    this->interpret_tool->setVariables(varName + "=" + sVal);
  }
}

string Parser::getNameOfSymbolBySimulatorPath(string path) {
  if (this->simPathToVarMap.count(path))
    return this->simPathToVarMap[path]->getName();
  else
    return "";
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

bool Parser::isBindingDirectionLeft(string varName) {
  return this->symbol_table.getVariable(varName)->getBindingDirection();

}
Parser::~Parser() {
  if (!commands_map.empty()) {
    commands_map.clear();
  }
  if (!symbol_table.empty()) {
    symbol_table.clear();
  }
  if (!simPathToVarMap.empty()) {
    simPathToVarMap.clear();
  }
  if (!indexToSimPathMap.empty()) {
    indexToSimPathMap.clear();
  }
  if (interpret_tool != nullptr)
    delete interpret_tool;
}
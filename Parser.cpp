
#include <algorithm>
#include <thread>
#include "Parser.h"
#include "Command.h"

/**
 * A func for a thread of executing commands.
 *
 * @param c The command.
 * @param tokensVec The tokens vector.
 * @param currIndex Current index.
 * @param parser Parser.
 */
void executeCommandFunc(Command *c, vector<string> tokensVec, int currIndex, Parser *parser) {
  c->execute(tokensVec, currIndex, parser);
}
/**
 * Parsing the given tokens vector fromt he lexer into executing commands.
 *
 * @param tokensVec The vector to pars.
 */
void Parser::parse(vector<string> tokensVec) {
  unsigned int index = 0;
  string token;
  //find the openDataServer command.
  auto it = find(tokensVec.begin(), tokensVec.end(), "openDataServer");
  if (it == tokensVec.end()) {
    cerr << "No openDataServer found" << endl;
    exit(EXIT_FAILURE);
  }
  //find the connectControlClient command.
  auto it2 = find(tokensVec.begin(), tokensVec.end(), "connectControlClient");
  if (it2 == tokensVec.end()) {
    cerr << "No connectControlClient found" << endl;
    exit(EXIT_FAILURE);
  }


  //open the server with a thread
  //Get the index of the token.
  index = std::distance(tokensVec.begin(), it);
  //open the thread
  thread openServer(executeCommandFunc, this->commands_map["openDataServer"], tokensVec, index, this);
  //erase the command and its parameters
  tokensVec.erase(it);
  tokensVec.erase(it);



  //connect as a client with a thread
  //Get the index of the token.
  index = std::distance(tokensVec.begin(), it2);
  //Open the thread
  thread connectClient(executeCommandFunc, this->commands_map["connectControlClient"], tokensVec, index, this);
  //erase the command and its parameters
  tokensVec.erase(it2);
  tokensVec.erase(it2);
  tokensVec.erase(it2);

  index = 0;
  //wait for the ConnectClientCommand to exit the block call of accepting.
  while (!this->canProceedParsing) { this_thread::sleep_for(400ms); }
  //Proceed parsing.
  while (index < tokensVec.size() && !this->shouldStopParsing) {
    token = tokensVec.at(index);
    Command *c = nullptr;
    if (this->commands_map.count(token)) { // is a method command
      c = this->commands_map[token];
    } else if (this->symbol_table.count(token)) { // is a variable assignment
      c = this->commands_map["="];
    } else { // else its executing a new defined function
      c = this->commands_map["function"];
    }
    index += c->execute(tokensVec, index, this);
  }
  //Now wait for the threads to end.
  openServer.join();
  connectClient.join();

}

/**
 * Build maps of the Parser : commands map and the indexToSimulatorPathMap
 */
void Parser::buildMaps() {
  //Insert known commands to the commands_map;
  this->commands_map.insert(make_pair("openDataServer", new OpenServerCommand()));
  this->commands_map.insert(make_pair("connectControlClient", new ConnectCommand()));
  this->commands_map.insert(make_pair("Sleep", new SleepCommand()));
  this->commands_map.insert(make_pair("Print", new PrintCommand()));
  this->commands_map.insert(make_pair("var", new DefineVarCommand()));
  this->commands_map.insert(make_pair("=", new VarAssignmentCommand()));
  this->commands_map.insert(make_pair("if", new IfCommand()));
  this->commands_map.insert(make_pair("while", new LoopCommand()));
  this->commands_map.insert(make_pair("function", new DefineFuncCommand()));

  //Insert in hardcoded way 36 pairs of indexes and paths according to the generic_small.xml file.
  this->indexToSimPathMap.insert(make_pair(0, "instrumentation/airspeed-indicator/indicated-speed-kt"));
  this->indexToSimPathMap.insert(make_pair(1, "sim/time/warp"));
  this->indexToSimPathMap.insert(make_pair(2, "controls/switches/magnetos"));
  this->indexToSimPathMap.insert(make_pair(3, "instrumentation/heading-indicator/offset-deg"));
  this->indexToSimPathMap.insert(make_pair(4, "instrumentation/altimeter/indicated-altitude-ft"));
  this->indexToSimPathMap.insert(make_pair(5, "instrumentation/altimeter/pressure-alt-ft"));
  this->indexToSimPathMap.insert(make_pair(6, "instrumentation/attitude-indicator/indicated-pitch-deg"));
  this->indexToSimPathMap.insert(make_pair(7, "instrumentation/attitude-indicator/indicated-roll-deg"));
  this->indexToSimPathMap.insert(make_pair(8, "instrumentation/attitude-indicator/internal-pitch-deg"));
  this->indexToSimPathMap.insert(make_pair(9, "instrumentation/attitude-indicator/internal-roll-deg"));
  this->indexToSimPathMap.insert(make_pair(10, "instrumentation/encoder/indicated-altitude-ft"));
  this->indexToSimPathMap.insert(make_pair(11, "instrumentation/encoder/pressure-alt-ft"));
  this->indexToSimPathMap.insert(make_pair(12, "instrumentation/gps/indicated-altitude-ft"));
  this->indexToSimPathMap.insert(make_pair(13, "instrumentation/gps/indicated-ground-speed-kt"));
  this->indexToSimPathMap.insert(make_pair(14, "instrumentation/gps/indicated-vertical-speed"));
  this->indexToSimPathMap.insert(make_pair(15, "instrumentation/heading-indicator/indicated-heading-deg"));
  this->indexToSimPathMap.insert(make_pair(16, "instrumentation/magnetic-compass/indicated-heading-deg"));
  this->indexToSimPathMap.insert(make_pair(17, "instrumentation/slip-skid-ball/indicated-slip-skid"));
  this->indexToSimPathMap.insert(make_pair(18, "instrumentation/turn-indicator/indicated-turn-rate"));
  this->indexToSimPathMap.insert(make_pair(19, "instrumentation/vertical-speed-indicator/indicated-speed-fpm"));
  this->indexToSimPathMap.insert(make_pair(20, "controls/flight/aileron"));
  this->indexToSimPathMap.insert(make_pair(21, "controls/flight/elevator"));
  this->indexToSimPathMap.insert(make_pair(22, "controls/flight/rudder"));
  this->indexToSimPathMap.insert(make_pair(23, "controls/flight/flaps"));
  this->indexToSimPathMap.insert(make_pair(24, "controls/engines/engine/throttle"));
  this->indexToSimPathMap.insert(make_pair(25, "controls/engines/current-engine/throttle"));
  this->indexToSimPathMap.insert(make_pair(26, "controls/switches/master-avionics"));
  this->indexToSimPathMap.insert(make_pair(27, "controls/switches/starter"));
  this->indexToSimPathMap.insert(make_pair(28, "engines/active-engine/auto-start"));
  this->indexToSimPathMap.insert(make_pair(29, "controls/flight/speedbrake"));
  this->indexToSimPathMap.insert(make_pair(30, "sim/model/c172p/brake-parking"));
  this->indexToSimPathMap.insert(make_pair(31, "controls/engines/engine/primer"));
  this->indexToSimPathMap.insert(make_pair(32, "controls/engines/current-engine/mixture"));
  this->indexToSimPathMap.insert(make_pair(33, "controls/switches/master-bat"));
  this->indexToSimPathMap.insert(make_pair(34, "controls/switches/master-alt"));
  this->indexToSimPathMap.insert(make_pair(35, "engines/engine/rpm"));

}

/**
 * Sets a new value to a var by a given name.
 * @param varName The wanted var name.
 * @param value The new value.
 */
void SymbolTable::setValue(string varName, float value) {
  Variable *var = symbol_map[varName];
  if (var != nullptr)
    var->setValue(value);
}
/**
 * Inserts a new var to the SymbolTable.
 *
 * @param var The new var to insert.
 */
void SymbolTable::insert(Variable *var) {
  this->symbol_map.insert(make_pair(var->getName(), var));
}

/**
 * Clears the SymbolTable
 *
 */
void SymbolTable::clear() {
  this->symbol_map.clear();
}
/**
 *
 *
 * @return True if the SymbolTable is empty.
 */
bool SymbolTable::empty() {
  return this->symbol_map.empty();
}

/**
 * Gets a variable by a given key.
 * @param key Name of the variable.
 * @return Variable pointer.
 */
Variable *SymbolTable::getVariable(string key) {
  Variable *var = this->symbol_map[key];
  return var;
}
/**
 * Returns if a given key exists in the symbol table.
 * @param key Var name.
 * @return True if a given key exists in the symbol table.
 */
bool SymbolTable::count(string key) {
  return this->symbol_map.count(key);
}

/**
 * Returns the interpreter of the parser.
 * @return Interpreter of the parser.
 */
InterpretTool *Parser::getInterpreter() {//TODO change to singleton.
  return interpret_tool;
}

/**
 * Inserting to the SymbolTable a new Var.
 * @param varName  Var name.
 * @param val Value.
 * @param path Simulator path, if its a local or a parameter it needs to be empty string.
 * @param bindingDirec Direction of the arrow -binding.
 * @param isLocalVariable If its a local var.
 */
void Parser::insert_to_symbol_table(string varName, float val, string path, bool bindingDirec, bool isLocalVariable) {
  Variable *var = new Variable(varName, val, path, bindingDirec, isLocalVariable);
  this->symbol_table.insert(var);
  this->simPathToVarMap.insert(make_pair(var->getSimulatorPath(), var));
  this->interpret_tool->setVariables(varName + "=" + to_string(val));
  //delete var;
}
/**
 *
 * Inserting a new paramter to the symbolTable.
 *
 * @param variable The parameter to insert.
 */
void Parser::insertParameterToSymbolTable(Variable *variable) {
  this->symbol_table.insert(variable);
  //this->simPathToVarMap.insert(make_pair(variable->getSimulatorPath(), variable));
  this->interpret_tool->setVariables(variable->getName() + "=" + to_string(variable->getValue()));
  //delete var;
}

/**
 * Update value of var by a given name.
 *
 *
 * @param varName Var name.
 * @param newVal New value of the var.
 */
void Parser::updateValue(string varName, float newVal) {
  if (isExistsInSymbolTable(varName)) {
    this->symbol_table.setValue(varName, newVal);
    string sVal = to_string(newVal);
    this->interpret_tool->setVariables(varName + "=" + sVal);
  }
}
/**
 * Returns vector of name of vars by given path.
 *
 * @param path Path of the vars.
 * @return Vector of names of vars.
 */
vector<string> Parser::getNamesOfSymbolsBySimulatorPath(string path) {
  vector<string> names;
  if (this->simPathToVarMap.count(path) && path != "") {
    //Iterate over the simpathtovarmap starting from the first pair matched to the path.
    typedef multimap<string, Variable *>::iterator MMAPIterator;
    std::pair<MMAPIterator, MMAPIterator> result = this->simPathToVarMap.equal_range(path);
    for (MMAPIterator pair = result.first; pair != result.second; ++pair) {
      if (pair->first == path)
        //add the name to the vector.
        names.push_back(pair->second->getName());
      else
        break;
    }

    return names;
  } else
    return names;
}
/**
 * returns the command by a given key.
 * @param key Key of the command.
 * @return Command object.
 */
Command *Parser::getCommand(string key) {
  return commands_map[key];
}
/**
 * Returns true if the key exists in the commands map.
 *
 * @param key Key to check.
 * @return True if the key exists in the commands map.
 */
bool Parser::isExistsInCommandsMap(string key) {
  return commands_map.count(key);
}

/**
 * Checks if the key exists in the symbol table.
 *
 * @param key Key to check.
 * @return True if exists.
 */
bool Parser::isExistsInSymbolTable(string key) {
  return symbol_table.count(key);
}
/**
 * Returns true if the binding direction is left of a var.
 *
 * @param varName Var name.
 * @return true if direction is left.
 */
bool Parser::isBindingDirectionLeft(string varName) {
  return this->symbol_table.getVariable(varName)->getBindingDirection();
}
/**
 *
 * Returns a value of symbol.
 *
 * @param varName Symbol name.
 * @return Value of symbol.
 */
float Parser::getValueOfSymbol(string varName) {
  return this->symbol_table.getVariable(varName)->getValue();
}

/**
 * Return simulator path by index.
 *
 * @param index Index of the path.
 * @return The matched path to the index from the indexToSimPathMap.
 */
string Parser::getSimulatorPathByIndex(int index) {
  return this->indexToSimPathMap[index];
}
/**
 * Return simulator path by varName.
 *
 * @param varName Var name matched to the path..
 * @return The matched path to the index from the indexToSimPathMap.
 */
string Parser::getSimulatorPathByVarName(string varName) {

  return this->symbol_table.getVariable(varName)->getSimulatorPath();
}
/**
 *
 * Returns true if the Variable matched to the varName is local.
 *
 * @param varName  Variable name.
 * @return true is the var is local.
 */
bool Parser::getIsLocalVar(string varName) {
  return this->symbol_table.getVariable(varName)->isLocalVariable();
}
/**
 * Inserts a new func command the commands map.
 * @param funcName New function name as its key.
 * @param c The command.
 */
void Parser::insertFuncCommandToCommandsMap(string funcName, Command *c) {
  this->commands_map.insert(make_pair(funcName, c));
}
/**
 * Erases symbol from the symbol table my name.
 *
 * @param name of the symbol to erase.
 */
void Parser::eraseSymbolFromTableByName(string name) {
  //First erase from the symbol table.
  if (this->symbol_table.count(name)) {
    this->symbol_table.symbol_map.erase(name);
  }
//  //Then iterate on the multimap  simPathToVarMAp and erase the wanted pair.
//  typedef multimap<string, Variable *>::iterator MMAPIterator;
//  std::pair<MMAPIterator, MMAPIterator> result = this->simPathToVarMap.equal_range("");
//  for (MMAPIterator pair = result.first; pair != result.second; ++pair) {
//    if ((*pair).second->getName() == name) {
//      this->simPathToVarMap.erase(pair);
//      break;
//    }
//  }
  //Then erase from the var map.
  this->interpret_tool->eraseFromVarMap(name);
}
/**
 *
 * Erasing parameters of function.
 *
 * @param parameters parameters of function.
 */
void Parser::eraseParametersOfFunc(unordered_map<string, Variable *> parameters) {
  for (auto pair : parameters) {
    eraseSymbolFromTableByName(pair.first);
  }
}
/**
 * Destructor of Parser/
 *
 */
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
//
// Created by amit on 20/12/2019.
//

#include "Command.h"
#include "InterpretTool.h"
#include "Parser.h"
#include <list>
#include <thread>
#include <mutex>
#include <algorithm>
#include <sstream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <unordered_map>

int sockID;
int client_socket;
map<string, float> simulatorValues;
bool done;
mutex m;
bool canConnectClient = false;
bool clientDisconnected = false;
//string wholeValues = "";
void updateValuesOfSymbols(Parser &pars) {
  try {
    for (auto pair : simulatorValues) {
      if (pars.getNameOfSymbolBySimulatorPath(pair.first) != "") {
        string name = pars.getNameOfSymbolBySimulatorPath(pair.first);
        if (pars.isBindingDirectionLeft(name)) {
          pars.updateValue(name, pair.second);
        }
      }
    }
  } catch (const char *e) {
    cout << e << endl;
  }
}

const vector<string> splitByChar(string wholeString, char delimeter) {
  vector<string> tokens;
  string token;
  istringstream tokenStream(wholeString);
  while (getline(tokenStream, token, delimeter)) {
    tokens.push_back(token);
  }
  return tokens;
}
bool canBeFloat(string numString) {
  try {
    stof(numString);
    return true;
  } catch (...) {
    return false;
  }
}
void readAndUpdateValuesFunc(Parser *parser) {
  char buffer[1024] = {0};
  while (!done) {
    int vl = read(client_socket, buffer, 1024);
    if (vl != -1) {
      unsigned int indexForIndexToSimPathMap = 0;
      string wholeValues = buffer;
      vector<string> lines = splitByChar(wholeValues, '\n');

      for (unsigned int i = 0; i < lines.size(); i++) {
        vector<string> valueStrings = splitByChar(lines.at(i), ',');
        if (i != lines.size() - 1)
          indexForIndexToSimPathMap = 36 - valueStrings.size();
        else
          indexForIndexToSimPathMap = 0;
        for (string valueString : valueStrings) {
          if (valueString.empty()) continue;
          string path = parser->getSimulatorPathByIndex(indexForIndexToSimPathMap);
          try {
            int length = 0;
            float value = 0;
            if (canBeFloat(valueString)) value = stof(valueString);
            else {
              while (length < valueString.length()
                  && (valueString.at(length) == '.' || isdigit(valueString.at(length)))) {
                length++;
              }
              value = stof(valueString.substr(0, length));
            }
            if (!simulatorValues.count(path)) {
              simulatorValues.insert(make_pair(path, value));
            } else {
              simulatorValues[path] = value;
            }
          } catch (const invalid_argument &e) {
            cout << "invalid argument at stof readandupdate: " + valueString << endl;

          } catch (const char *e) {
            cout << e << endl;
          }
          indexForIndexToSimPathMap++;
        }
      }
      //wholeValues = lines.at(lines.size() - 1) + ',';
      updateValuesOfSymbols(*parser);
    }
  }
}
void acceptFunc(int socket_server, sockaddr_in &address1) {
  m.lock();
  client_socket = accept(socket_server, (struct sockaddr *) &address1, (socklen_t *) &address1);
  cout << "Accepting simulator...." << endl;
  if (client_socket == -1) {
    throw "Error due to accept command";
  }
  cout << "Simulator Connected" << endl;
  m.unlock();
}

int OpenServerCommand::execute(vector<string> &tokensVec, int currIndex, Parser *parser) {
  int sockserver;
  sockserver = socket(AF_INET, SOCK_STREAM, 0);
  if (sockserver == -1) {
    std::cerr << "Cannot create a socket" << std::endl;
  }
  sockaddr_in address1;
  int portNum = parser->getInterpreter()->interpretMathExpression(tokensVec.at(currIndex + 1))->calculate();
  address1.sin_family = AF_INET;
  address1.sin_addr.s_addr = INADDR_ANY;
  address1.sin_port = htons(portNum);
  if (bind(sockserver, (sockaddr *) &address1, sizeof(address1)) == -1) {
    std::cerr << "Cannot bind socket to ip" << std::endl;

  }
  if (listen(sockserver, SOMAXCONN) == -1) {
    std::cerr << "Error during listen command" << std::endl;
  }
  thread acceptT(acceptFunc, sockserver, std::ref(address1));

  acceptT.join();
  //sockclient = socket(AF_INET, SOCK_STREAM, 0);
  canConnectClient = true;

  thread readAndUpdateValues(readAndUpdateValuesFunc, parser);
  readAndUpdateValues.join();
  while (!clientDisconnected) {}
  close(sockserver);

  return 2;
}

void connectFunc(sockaddr_in address) {
  m.lock();
  int is_connect = connect(sockID, (struct sockaddr *) &address, sizeof(address));
  if (is_connect == -1) {
    std::cerr << "Cannot establish connection" << std::endl;
  }
  m.unlock();
}
int ConnectCommand::execute(vector<string> &tokensVec, int currIndex, Parser *parser) {
  while (!canConnectClient) {
    cout << "waiting for simulator to connect to server" << endl;
    this_thread::sleep_for(1s);
  }
  //cout << "Connection established";

  string ip = tokensVec.at(currIndex + 1);
  ip = ip.substr(1, ip.length() - 2);
  int port = parser->getInterpreter()->interpretMathExpression(tokensVec.at(currIndex + 2))->calculate();
  sockID = socket(AF_INET, SOCK_STREAM, 0);
  if (sockID == -1) {
    std::cerr << "Cannot create a socket" << std::endl;
  }
  sockaddr_in address;
  address.sin_family = AF_INET;
  const char *ipc = ip.c_str();
  address.sin_addr.s_addr = inet_addr(ipc);
  address.sin_port = htons(port);
  thread connect(connectFunc, address);
  connect.join();
  this_thread::sleep_for(5s);
  parser->canProceedParsing = true;

  while (!done) {}

  close(sockID);
  clientDisconnected = true;
  return 3;
}
void sleepFunc(int sleepParam) {
  m.lock();
  this_thread::sleep_for(chrono::milliseconds(sleepParam));
  m.unlock();
}
int SleepCommand::execute(vector<string> &tokensVector, int currentIndex, Parser *parser) {
  string sleepParameterToken = tokensVector.at(currentIndex + 1);
  InterpretTool *i = parser->getInterpreter();
  int sleepParameter = i->interpretMathExpression(sleepParameterToken)->calculate();
  //int sleepParameter = stoi(sleepParameterToken);
  m.lock();
  this_thread::sleep_for(chrono::milliseconds(sleepParameter));
  m.unlock();
//  thread sleepThread(sleepFunc, sleepParameter);
//  sleepThread.join();

  return SleepCommand::STEPS;

}

int IfCommand::execute(vector<string> &tokensVec, int currIndex, Parser *parser) {
  int steps = IfCommand::STEPS;
  int index = currIndex + 3;
  buildCommandsVector(tokensVec, parser, index);
  vector<Command *> commands = getInnerCommands();
  if (isConditionTrue(tokensVec.at(currIndex + 1), *parser)) {
    for (Command *c : commands) index += c->execute(tokensVec, index, parser);
  }
  for (Command *c : commands) steps += c->getSteps();
  if (steps + 1 + currIndex >= tokensVec.size()) done = true;
  return steps + 1;
}

int LoopCommand::execute(vector<string> &tokensVec, int currIndex, Parser *parser) {
  int steps = LoopCommand::STEPS;
  int index = currIndex + 3;
  buildCommandsVector(tokensVec, parser, index);
  bool expIsTrue = isConditionTrue(tokensVec.at(currIndex + 1), *parser);
  vector<Command *> commands = getInnerCommands();
  while (expIsTrue) {
    for (Command *c : commands) index += c->execute(tokensVec, index, parser);
    index = currIndex + 3;
    expIsTrue = isConditionTrue(tokensVec.at(currIndex + 1), *parser);
  }
  for (Command *c : commands) steps += c->getSteps();
  if (steps + 1 + currIndex >= tokensVec.size()) done = true;
  return steps + 1;
}

int DefineVarCommand::execute(vector<string> &tokensVec, int currIndex, Parser *parser) {

  float value = 0;
  bool bindingDirection = false;
  bool isAssignment = false;
  string simulatorPath = tokensVec.at(currIndex + 3);

  if (tokensVec.at(currIndex + 2) == "->") {
    bindingDirection = false;
  }
  if (tokensVec.at(currIndex + 2) == "<-") {
    bindingDirection = true;
    value = simulatorValues[simulatorPath];
  }
  if (tokensVec.at(currIndex + 2) == "=") {
    isAssignment = true;
    updateValuesOfSymbols(*parser);
    InterpretTool *i = parser->getInterpreter();
    string valueString = tokensVec.at(currIndex + 3);
    valueString.erase(std::remove(valueString.begin(),
                                  valueString.end(), ' '), valueString.end());
    value = i->interpretMathExpression(valueString)->calculate();
    simulatorPath = "";
  }

  string name = tokensVec.at(currIndex + 1);
  parser->insert_to_symbol_table(name, value, simulatorPath, bindingDirection, isAssignment);
  if (currIndex + PrintCommand::STEPS >= tokensVec.size()) done = true;
  return DefineVarCommand::STEPS;
}

int PrintCommand::execute(vector<string> &tokensVec, int currIndex, Parser *parser) {
  if (parser->isExistsInSymbolTable(tokensVec.at(currIndex + 1))) {
    std::cout << parser->getValueOfSymbol(tokensVec.at(currIndex + 1)) << std::endl;
  } else if (tokensVec.at(currIndex + 1).find("\"") != string::npos) {
    string stringToPrint = tokensVec.at(currIndex + 1);
    stringToPrint = stringToPrint.substr(1, stringToPrint.length() - 2);
    cout << stringToPrint << endl;
  } else {
    string withoutSpaces = tokensVec.at(currIndex + 1);
    withoutSpaces.erase(remove(withoutSpaces.begin(),
                               withoutSpaces.end(), ' '), withoutSpaces.end());
    try {
      InterpretTool *i = parser->getInterpreter();
      Expression *express = i->interpretMathExpression(withoutSpaces);
      float value = express->calculate();
      cout << value << endl;
    } catch (const char *e) {
      throw e;
    }
  }
  if (tokensVec.at(currIndex + 1) == "\"done\"") done = true;
  if (currIndex + PrintCommand::STEPS >= tokensVec.size()) done = true;

  return PrintCommand::STEPS;
}

int VarAssignmentCommand::execute(vector<string> &tokensVec, int currIndex, Parser *pars) {

  try {
    //updateValuesOfSymbols(*pars);
    string varName = tokensVec.at(currIndex);
    bool isLocalVar = pars->getIsLocalVar(varName);
    bool isBindingDirectionLeft = pars->isBindingDirectionLeft(varName);
    InterpretTool *i = pars->getInterpreter();
    string assignString = tokensVec.at(currIndex + 1);
    Expression *e = i->interpretMathExpression(assignString);
    float val = e->calculate();
    if (!isLocalVar && !isBindingDirectionLeft) {
      string simPath = pars->getSimulatorPathByVarName(varName);
      string message = "set " + simPath + " " + to_string(val) + "\r\n";
      ssize_t return_val = write(sockID, message.c_str(), message.length());
      if (return_val == -1) {
        done = true;
        pars->shouldStopParsing = true;
      }
    }
    pars->updateValue(tokensVec.at(currIndex), val);
  } catch (const char *e) {
    string ex = e;
    cout << "at var assignmnet " + ex << endl;
  }
  if (currIndex + VarAssignmentCommand::STEPS >= tokensVec.size()) done = true;

  return VarAssignmentCommand::STEPS;

}
int DefineFuncCommand::execute(vector<string> &tokensVec, int currIndex, Parser *parser) {

  string functionName = tokensVec.at(currIndex);//takeoff
  string parameterName = tokensVec.at(currIndex + 1);//takeoff_x
  FuncCommand *func_command = new FuncCommand();
  Variable *param = new Variable(parameterName, 0, "", false, true);
  func_command->parameters.insert(make_pair(parameterName, param));
  parser->insertParameterToSymbolTable(param);
  int loopIndex = currIndex + 2;
  while (tokensVec.at(loopIndex) != "}") {
    string token = tokensVec.at(loopIndex);
    string lowerCaseLine = token;
    transform(lowerCaseLine.begin(), lowerCaseLine.end(), lowerCaseLine.begin(), ::tolower);
    if (parser->isExistsInCommandsMap(token)) {
      Command *c = parser->getCommand(token);
      func_command->inner_commands.push_back(c);
      if (lowerCaseLine == "print" || lowerCaseLine == "sleep")
        loopIndex += c->getSteps();
      if (lowerCaseLine == "while" || lowerCaseLine == "if") {
        loopIndex += c->getSteps();
        while (tokensVec.at(loopIndex) != "}") {
          string token = tokensVec.at(loopIndex);
          string lowerCaseLine = token;
          transform(lowerCaseLine.begin(),
                    lowerCaseLine.end(),
                    lowerCaseLine.begin(),
                    ::tolower);
          if (parser->isExistsInCommandsMap(token)) {
            Command *c = parser->getCommand(token);
            func_command->inner_commands.push_back(c);
            if (lowerCaseLine == "print" || lowerCaseLine == "sleep")
              loopIndex += c->getSteps();
          } else if (parser->isExistsInSymbolTable(token)) {
            Command *c = parser->getCommand("=");
            func_command->inner_commands.push_back(c);
            loopIndex += 2;
          }
        }
        loopIndex + 1;
      }
    } else if (parser->isExistsInSymbolTable(token)) {
      Command *c = parser->getCommand("=");
      func_command->inner_commands.push_back(c);
      loopIndex += 2;
    }
  }
  func_command->startingIndex = currIndex + 3;
  parser->insertFuncCommandToCommandsMap(functionName, func_command);

  return loopIndex - currIndex + 1;

}
int FuncCommand::execute(vector<string> &tokensVec, int currIndex, Parser *parser) {
  float value = stof(tokensVec.at(currIndex + 1));
  this->parameters.at(0)->setValue(value);
  int index = this->startingIndex;
  for (Command *c : inner_commands) {
    index = index + c->execute(tokensVec, index, parser);
  }
  return 1 + this->parameters.size();
}
void ConditionParser::buildCommandsVector(vector<string> &tokensVec, Parser *parser, int index) {
  int currentIndex = index;
  this->inner_commands.clear();
  while (tokensVec.at(currentIndex) != "}") {
    string token = tokensVec.at(currentIndex);
    string lowerCaseLine = token;
    transform(lowerCaseLine.begin(),
              lowerCaseLine.end(),
              lowerCaseLine.begin(),
              ::tolower);
    if (parser->isExistsInCommandsMap(token)) {
      Command *c = parser->getCommand(token);
      this->insert_to_inner_commands(c);
      if (lowerCaseLine == "print" || lowerCaseLine == "sleep")
        currentIndex += 2;
    } else if (parser->isExistsInSymbolTable(token)) {
      Command *c = parser->getCommand("=");
      this->insert_to_inner_commands(c);
      currentIndex += 2;
    }
  }
}
bool ConditionParser::isConditionTrue(string conditionString, Parser &pars) {
  updateValuesOfSymbols(pars);
  Expression *e = pars.getInterpreter()->interpretBoolExpression(conditionString);
  int result = e->calculate();
  return result == 1 ? true : false;
}

void ConditionParser::insert_to_inner_commands(Command *c) {
  this->inner_commands.push_back(c);
}

vector<Command *> ConditionParser::getInnerCommands() {
  return inner_commands;
}
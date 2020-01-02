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
#include <cstring>
#include <sstream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <unordered_map>

static int const STEPS_FOR_ONE_PARAMETER_COMMAND = 2;
static int const STEPS_FOR_TWO_PARAMETERS_COMMAND = 3;
static int const STEPS_FOR_IF_OR_LOOP_COMMAND = 3;

int sockID;
int client_socket;
map<string, float> simulatorValues;
bool done;
mutex m;
bool canConnectClient = false;
bool clientDisconnected = false;
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
void sleepFunc(int sleepParam) {
  m.lock();
  this_thread::sleep_for(chrono::milliseconds(sleepParam));
  m.unlock();
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

void readAndUpdateValuesFunc(Parser *parser) {
  char buffer[10000] = {0};
  while (!done) {
    int vl = read(client_socket, buffer, 10000);
    if (vl != -1) {
      unsigned int indexForIndexToSimPathMap = 0;
      string wholeValues = buffer;
      vector<string> lines = splitByChar(wholeValues, '\n');
      unsigned int numOfLines = 1;
      if (!lines.empty()) {
        if (lines.size() > 1)
          numOfLines = 2;
        for (unsigned int i = 0; i < numOfLines; i++) {
          vector<string> valueStrings;
          if (i == 0) {
            indexForIndexToSimPathMap = 36 - valueStrings.size();
            valueStrings = splitByChar(lines.at(0), ',');
          } else {
            indexForIndexToSimPathMap = 0;
            valueStrings = splitByChar(lines.at(lines.size() - 1), ',');
          }
          for (string valueString : valueStrings) {
            if (!valueString.empty()) {
              string path = parser->getSimulatorPathByIndex(indexForIndexToSimPathMap);
              try {
                float value = stof(valueString);
                if (!simulatorValues.count(path)) {
                  simulatorValues.insert(make_pair(path, value));
                } else {
                  simulatorValues[path] = value;
                }
              } catch (const invalid_argument &e) {
                cout << "invalid argumnet at stof readandupdate: " + valueString << endl;

              } catch (const char *e) {
                cout << e << endl;
              }
              indexForIndexToSimPathMap++;
            }
          }
        }
      }

      this_thread::sleep_for(1s);
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
  int sockserver;//, sockclient;
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
  thread accept(acceptFunc, sockserver, std::ref(address1));

  accept.join();
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
  //const char *ipc = ip.c_str(); //TODO check why it doesnt work to put a variable
  address.sin_addr.s_addr = inet_addr("127.0.0.1");
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

int SleepCommand::execute(vector<string> &tokensVector, int currentIndex, Parser *parser) {
  //TODO make the main sleep
  string sleepParameterToken = tokensVector.at(currentIndex + 1);
  InterpretTool *i = parser->getInterpreter();
  int sleepParameter = i->interpretMathExpression(sleepParameterToken)->calculate();
  //int sleepParameter = stoi(sleepParameterToken);

  thread sleepThread(sleepFunc, sleepParameter);
  sleepThread.join();

  return STEPS_FOR_ONE_PARAMETER_COMMAND;

}

int IfCommand::execute(vector<string> &tokensVec, int currIndex, Parser *parser) {
  int steps = STEPS_FOR_IF_OR_LOOP_COMMAND;
  int index = currIndex + 3;
  int returnedSteps = 0;
  buildCommandsVector(tokensVec, parser, index);
  if (isConditionTrue(tokensVec.at(currIndex + 1), *parser)) {
    vector<Command *> commands = getInnerCommands();
    for (Command *c : commands) {
      returnedSteps = c->execute(tokensVec, index, parser);
      steps += returnedSteps;
      index += returnedSteps;
    }
  }
  if (steps + 1 + currIndex >= tokensVec.size()) done = true;
  return steps + 1;
}

int LoopCommand::execute(vector<string> &tokensVec, int currIndex, Parser *parser) {
  int steps = STEPS_FOR_IF_OR_LOOP_COMMAND;
  int index = currIndex + 3;
  int returnedSteps = 0;
  bool wasFirstLoop = false;
  buildCommandsVector(tokensVec, parser, index);

  while (isConditionTrue(tokensVec.at(currIndex + 1), *parser)) {
    vector<Command *> commands = getInnerCommands();
    for (Command *c : commands) {
      returnedSteps = c->execute(tokensVec, index, parser);
      if (!wasFirstLoop) steps += returnedSteps;
      index += returnedSteps;
    }
    index = currIndex + 3;
    if (!wasFirstLoop) {
      wasFirstLoop = true;
    }
  }
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
  if (currIndex + 4 >= tokensVec.size()) done = true;
  return 4;
}

int PrintCommand::execute(vector<string> &tokensVec, int currIndex, Parser *parser) {
  updateValuesOfSymbols(*parser);
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
  //if (tokensVec.at(currIndex + 1) == "\"done\"") done = true;
  if (currIndex + 2 >= tokensVec.size()) done = true;

  return 2;
}

int VarAssignmentCommand::execute(vector<string> &tokensVec, int currIndex, Parser *pars) {

  try {
    updateValuesOfSymbols(*pars);
    string varName = tokensVec.at(currIndex);
    bool isLocalVar = pars->getIsLocalVar(varName);
    bool isBindingDirectionLeft = pars->isBindingDirectionLeft(varName);
    InterpretTool *i = pars->getInterpreter();
    Expression *e = i->interpretMathExpression(tokensVec.at(currIndex + 1));
    float val = e->calculate();
    if (!isLocalVar && !isBindingDirectionLeft) {
      string simPath = pars->getSimulatorPathByVarName(varName);
      string message = "set " + simPath + " " + to_string(val) + "\r\n";
      ssize_t return_val = write(sockID, message.c_str(), message.length());
      if (return_val == -1){
        cerr << "problem at writing to simulator : " + message << endl;
        done = true;
      }
    }
    pars->updateValue(tokensVec.at(currIndex), val);
  } catch (const char *e) {
    string ex = e;
    cout << "at var assignmnet " + ex << endl;
  }
  if (currIndex + 2 >= tokensVec.size()) done = true;

  return 2;

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
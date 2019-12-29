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

static int const STEPS_FOR_ONE_PARAMETER_COMMAND = 2;
static int const STEPS_FOR_TWO_PARAMETERS_COMMAND = 3;
static int const STEPS_FOR_IF_OR_LOOP_COMMAND = 3;

int sockID;
map<string, float> simulatorValues;

void sleepFunc(int sleepParam) {
  //while (!parser->isSleep()) {}
  mutex m;
  m.lock();
  //int milliSeconds = stoi(parser->getSleepMilliSeconds());
  this_thread::sleep_for(chrono::milliseconds(sleepParam));
  //parser->wake();
  m.unlock();
}
const vector<string >splitByChar(string wholeString, char delimter) {

  vector<string> tokens;
  string token;
  istringstream tokenStream(wholeString);
  while (getline(tokenStream, token, '\n')) {
    //tokens.push_back(token);
    string lineWithoutNewLine = token;
    istringstream tokenStream1(lineWithoutNewLine);
    string token1;
    while (getline(tokenStream1, token1, '\r')) {
      string lineWithoutSlashR = token;
      istringstream tokenStream2(lineWithoutSlashR);
      string token2;
      while (getline(tokenStream2, token2, ',')) {
        tokens.push_back(token2);
      }
    }
  }

  return tokens;
}


void readAndUpdateValuesFunc(vector<string> &tokensVec, int currIndex, Parser *parser){
  char *buffer;
  // char* buffer="--generic=socket,out,10,127.0.0.1," + tokensVec.at(currIndex +1) + ",tcp,generic_small";
  // int vals= send(client_socket,(char*)buffer,strlen(buffer),0);
  //buffer is a member of OpenServerCommand Class. we also need to do getBuffer() method.
  while(){
    int vl = read(client_socket, buffer, sizeof(buffer));
    vector<string> values = splitByChar(buffer, ',');
    for (unsigned i = 0; i < values.size(); i++) {
      string path = parser->getSimulatorPathByIndex(i);
      float value = stof(values.at(i));

      if(!simulatorValues.count(path)){
        simulatorValues.insert(make_pair(path, value));
      } else {
        simulatorValues[path] = value;
      }
      //parser->insert_to_simulator_values(path, value);

    }
  }
}

int OpenServerCommand::execute(vector<string> &tokensVec, int currIndex, Parser *parser) {
  int client_socket, sockserver, sockclient;
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

  client_socket = accept(sockserver, (struct sockaddr *) &address1, (socklen_t *) &address1);
  if (client_socket == -1) {
    std::cerr << "Error due to accept command" << std::endl;
  }

  sockclient = socket(AF_INET, SOCK_STREAM, 0);
  return 2;
}

int ConnectCommand::execute(vector<string> &tokensVec, int currIndex, Parser *parser) {
  string ip = tokensVec.at(currIndex + 1);
  int port = parser->getInterpreter()->interpretMathExpression(tokensVec.at(currIndex + 2))->calculate();
  sockID = socket(AF_INET, SOCK_STREAM, 0);
  if (sockID == -1) {
    std::cerr << "Cannot create a socket" << std::endl;
  }
  sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = stoi(ip);
  address.sin_port = htons(port);
  if (connect(sockID, (struct sockaddr *) &address, sizeof(address) == -1)) {
    std::cerr << "Cannot establish connection" << std::endl;
  }
  return 3;
}


int SleepCommand::execute(vector<string> &tokensVector, int currentIndex, Parser *parser) {

  int sleepParameter(stoi(tokensVector.at(currentIndex + 1)));
  //parser->setSleepMilliSeconds(sleepParameter);
  //parser->sleep();
  thread sleepThread(sleepFunc, sleepParameter);
  sleepThread.join();

  return STEPS_FOR_ONE_PARAMETER_COMMAND;

}

int IfCommand::execute(vector<string> &tokensVec, int currIndex, Parser *parser) {
  int steps = STEPS_FOR_IF_OR_LOOP_COMMAND;

  int index = currIndex + 3;
  buildCommandsVector(tokensVec, parser, index);

  if (isConditionTrue(tokensVec.at(currIndex + 1), *parser)) {
    vector<Command> commands = getInnerCommands();
    for (Command c : commands) {
      steps += c.execute(tokensVec, currIndex, parser);
    }
  }

  return steps + 1;
}

int LoopCommand::execute(vector<string> &tokensVec, int currIndex, Parser *parser) {
  int steps = STEPS_FOR_IF_OR_LOOP_COMMAND;
  int index = currIndex + 3;
  buildCommandsVector(tokensVec, parser, index);
  while (isConditionTrue(tokensVec.at(currIndex + 1), *parser)) {
    vector<Command> commands = getInnerCommands();
    for (Command c : commands) {
      steps += c.execute(tokensVec, currIndex, parser);
    }
  }

  return steps + 1;
}


int DefineVarCommand::execute(vector<string> &tokensVec, int currIndex, Parser *parser) {

  float value = 0;
  bool bindingDirection;
  string simulatorPath = tokensVec.at(currIndex + 3);

  if (tokensVec.at(currIndex + 2) == "->") {
//        char* buffer = "set" + tokensVec.at(currIndex + 3) +
//                       to_string(parser->getValueOfSymbol(tokensVec.at(currIndex + 1)));
//        send(socketfd, (char *) buffer, strlen(buffer), 0);
    bindingDirection = false;

  }
  if (tokensVec.at(currIndex + 2) == "<-") {
    bindingDirection = true;
    value = simulatorValues[simulatorPath];
  }
  string name = tokensVec.at(currIndex + 1);
  parser->insert_to_symbol_table(name, value, simulatorPath, bindingDirection);
  return 4;
}

int PrintCommand::execute(vector<string> &tokensVec, int currIndex, Parser *parser) {
  if (parser->isExistsInSymbolTable(tokensVec.at(currIndex + 1))) {
    std::cout << parser->getValueOfSymbol(tokensVec.at(currIndex + 1)) << std::endl;
  } else if (tokensVec.at(currIndex + 1).find("\"") != string::npos) {
    cout << tokensVec.at(currIndex + 1) << endl;
  } else {
    string withoutSpaces = tokensVec.at(currIndex + 1);
    withoutSpaces.erase(remove(withoutSpaces.begin(),
                               withoutSpaces.end(), ' '), withoutSpaces.end());
    try {
      float value = parser->getInterpreter()->interpretMathExpression(withoutSpaces)->calculate();
      cout << value << endl;
    } catch (const char *e) {
      throw e;
    }
  }
  return 2;
}

void ConditionParser::buildCommandsVector(vector<string> &tokensVec, Parser *parser, int index) {
  while (tokensVec.at(index) != "}") {
    string token = tokensVec.at(index);
    string lowerCaseLine = token;
    transform(lowerCaseLine.begin(),
              lowerCaseLine.end(),
              lowerCaseLine.begin(),
              ::tolower);
    if (parser->isExistsInCommandsMap(lowerCaseLine)) {
      this->insert_to_inner_commands(*parser->getCommand(token));
    } else if (parser->isExistsInSymbolTable(token)) {
      this->insert_to_inner_commands(*parser->getCommand("="));
    }
//    if (tokensVec.at(index) == "var") {
//      index++;
//      string name = tokensVec.at(index);
//      index++;
//      float value = stod(tokensVec.at(index));
//      index++;
//      bool bindDirec = tokensVec.at(index) == "->" ? false : true;
//      index++;
//      string path = tokensVec.at(index);
//      parser->insert_to_symbol_table(name, value, path, bindDirec);
//    }

  }
}

int VarAssignmentCommand::execute(vector<string> &tokensVector, int currentIndex, class Parser *pars) {

  if (!pars->isBindingDirectionLeft(tokensVector.at(currentIndex))) {
    string simPath = pars->getSimulatorPathByVarName(tokensVector.at(currentIndex));
    string message = "set" + simPath + tokensVector.at(currentIndex + 1) + "\r\n";
    ssize_t return_val;
    return_val = write(sockID, message.c_str(), message.length());

  }

  pars->updateValue(tokensVector.at(currentIndex), stof(tokensVector.at(currentIndex + 2)));

  return 3;

}


bool ConditionParser::isConditionTrue(string conditionString, Parser &pars) {
  auto par = pars;
  return par.getInterpreter()->interpretBoolExpression(conditionString)->calculate() == 1 ? true : false;
}

void ConditionParser::insert_to_inner_commands(Command c) {
  this->inner_commands.push_back(c);
}

vector<Command> ConditionParser::getInnerCommands() {
  return inner_commands;
}
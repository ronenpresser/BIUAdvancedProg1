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
string prefValues = "";

/**
 * Updateins value of symbols.
 * @param pars  Parser
 */
void updateValuesOfSymbols(Parser &pars) {
  try {
    for (auto pair : simulatorValues) {
      string path = pair.first;
      //Its a multimap, can be more than one variable to a one simulator.
      vector<string> names = pars.getNamesOfSymbolsBySimulatorPath(path);
      if (!names.empty()) {
        for (string name:names) {
          if (pars.isBindingDirectionLeft(name)) {
            pars.updateValue(name, pair.second);
          }
        }
      }
    }
  } catch (const char *e) {
    string err = e;
    cout << "at updateing values" + err << endl;
  }
}
/**
 * Split a string by a given char.
 *
 * @param wholeString The string to split.
 * @param delimeter The char to split by.
 * @return vector of strings that were split.
 */
const vector<string> splitByChar(string wholeString, char delimeter) {
  vector<string> tokens;
  string token;
  istringstream tokenStream(wholeString);
  while (getline(tokenStream, token, delimeter)) {
    tokens.push_back(token);
  }
  return tokens;
}

/**
 * Checks if a string can be converted to float.
 *
 * @param numString
 * @return
 */
bool canBeFloat(string numString) {
  try {
    stof(numString);
    return true;
  } catch (...) {
    return false;
  }
}
/**
 * Reads and updates values from the simulator.
 *
 * @param parser
 */
void readAndUpdateValuesFunc(Parser *parser) {
  char buffer[1024] = {0};
  while (!done) {
    //Read from the simulator
    int vl = read(client_socket, buffer, 1024);
    if (vl != -1 && vl != 0) {
      unsigned int indexForIndexToSimPathMap = 0;
      string wholeValues = buffer;
      //First split the buffer by new lines.
      vector<string> lines = splitByChar(wholeValues, '\n');
      //Foreach of the lines:
      for (unsigned int i = 0; i < lines.size(); i++) {
        //Split the line by comma.
        vector<string> valueStrings = splitByChar(lines.at(i), ',');
        if (i != lines.size() - 1)
          //if its not the last line
          indexForIndexToSimPathMap = 36 - valueStrings.size();
        else
          //if its the last line,
          indexForIndexToSimPathMap = 0;
        for (string valueString : valueStrings) {
          if (valueString.empty()) {
            indexForIndexToSimPathMap++;
            continue;
          }
          string path = parser->getSimulatorPathByIndex(indexForIndexToSimPathMap);
          try {
            int length = 0;
            float value = 0;
            if (canBeFloat(valueString)) value = stof(valueString);
//            else {
//              while (length < valueString.length()
//                  && (valueString.at(length) == '.' || isdigit(valueString.at(length)))) {
//                length++;
//              }
//              value = stof(valueString.substr(0, length));
//            }
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
        updateValuesOfSymbols(*parser);
      }
    } else {
      cout << "Stopped parsing" << endl;
      done = true;
      throw runtime_error("Problem reading from the simulator: simulator disconnected");
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
    std::cerr << "Cannot establish connection as a client to the simulator" << std::endl;
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

int SleepCommand::execute(vector<string> &tokensVector, int currentIndex, Parser *parser) {
  //First take the parameter token for the sleep command.
  string sleepParameterToken = tokensVector.at(currentIndex + 1);
  InterpretTool *i = parser->getInterpreter();
  //Calculate it with the interpreter.
  int sleepParameter = i->interpretMathExpression(sleepParameterToken)->calculate();
  //Lock on sleep.
  m.lock();
  this_thread::sleep_for(chrono::milliseconds(sleepParameter));
  m.unlock();
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
    cout << "Stoped parsing" << endl;
    done = true;
    throw runtime_error("Problem at sending a set request to the simulator: simulator disconnected");
  }
  if (currIndex + VarAssignmentCommand::STEPS >= tokensVec.size()) done = true;

  return VarAssignmentCommand::STEPS;

}

int DefineFuncCommand::execute(vector<string> &tokensVec, int currIndex, Parser *parser) {

  //Function name is at the current index in the tokens vec.
  string functionName = tokensVec.at(currIndex);
  FuncCommand *func_command = new FuncCommand();
  //The commands of the function is at current index + 3, after the opennign {.
  int loopIndex = currIndex + 3;
  while (tokensVec.at(loopIndex) != "}") {
    string token = tokensVec.at(loopIndex);
    string lowerCaseLine = token;
    transform(lowerCaseLine.begin(), lowerCaseLine.end(), lowerCaseLine.begin(), ::tolower);
    if (parser->isExistsInCommandsMap(token)) {//if its a command
      Command *c = parser->getCommand(token);
      if (lowerCaseLine == "print" || lowerCaseLine == "sleep") {
        func_command->inner_commands.push_back(c);
        loopIndex += c->getSteps();
      } else if (lowerCaseLine == "while" || lowerCaseLine == "if") { //if its a while or if
        //Convert the command to condition parser.
        ConditionParser *condition_parser = (ConditionParser *) c;
        //Skip to index after the opening {.
        loopIndex += 3;
        condition_parser->buildCommandsVector(tokensVec, parser, loopIndex);
        func_command->inner_commands.push_back(condition_parser);
        auto v = condition_parser->getInnerCommands();
        //Get steps of every command in the while loop or if so we know how much to skip.
        for (auto c : v) loopIndex += c->getSteps();
        ++loopIndex;//for the closing } of the if or while loop.
      }
    } else if (parser->isExistsInSymbolTable(token)) {//Is a variable name.
      Command *c = parser->getCommand("=");
      func_command->inner_commands.push_back(c);
      loopIndex += 2;
    }
  }
  // Save the starting index of the function defining.
  func_command->startingIndex = currIndex + 3;
  //Insert the funccommand.
  parser->insertFuncCommandToCommandsMap(functionName, func_command);

  //The number of steps is from the currenet index to the next token after the closing } of the func defining
  return loopIndex - currIndex + 1;

}

int FuncCommand::execute(vector<string> &tokensVec, int currIndex, Parser *parser) {

  int index = this->startingIndex; //Go to the starting index of where we defined the func.
  //Parameter name is at the startinindex  - 2 in the tokens vec.
  string parameterName = tokensVec.at(index - 2);
  //Build the parameter as a Variable.
  Variable *param = new Variable(parameterName, 0, "", false, true);
  this->parameters.insert(make_pair(parameterName, param));
  parser->insertParameterToSymbolTable(param);
  //Get the parameter in the next index.
  float value = stof(tokensVec.at(currIndex + 1));
  //Update the parameters by the given values.
  for (auto p : this->parameters) {//TODO here can be supported for multiple args
    parser->updateValue(p.first, value);
  }
  //exectue every inner command.
  for (Command *c : inner_commands) {
    index = index + c->execute(tokensVec, index, parser);
  }

  //erase the parameters from the data structures.
  parser->eraseParametersOfFunc(parameters);
  return 1 + this->parameters.size();
}
/**
 *
 * Builds the commands vector of the condition parser.
 *
 * @param tokensVec Tokens vector to iterate on.
 * @param parser Parser to use to get wanted commands.
 * @param index Current index of the while loop or if.
 */
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
    if (parser->isExistsInCommandsMap(token)) { //if its a command.
      Command *c = parser->getCommand(token);
      this->insert_to_inner_commands(c);
      if (lowerCaseLine == "print" || lowerCaseLine == "sleep")
        currentIndex += 2;
    } else if (parser->isExistsInSymbolTable(token)) { //if its a symbol so it a var assignment.
      Command *c = parser->getCommand("=");
      this->insert_to_inner_commands(c);
      currentIndex += 2;
    }
  }
}
/**
 *
 * Checks if the boolean expression of the while loop or if is true.
 *
 * @param conditionString The condition expression string
 * @param pars The parser to use its interpreter.
 * @return True if the condition is true.
 */
bool ConditionParser::isConditionTrue(string conditionString, Parser &pars) {
  updateValuesOfSymbols(pars);
  InterpretTool *i = pars.getInterpreter();
  Expression *e = i->interpretBoolExpression(conditionString);
  int result = e->calculate();
  return result == 1 ? true : false;
}

/**
 * Inserts a given command to the inner command vector of the condition parser parent.
 *
 * @param c The command to insert.
 */
void ConditionParser::insert_to_inner_commands(Command *c) {
  this->inner_commands.push_back(c);
}

/**
 * Returns inner commands vector of the condition parser parent.
 *
 * @return vector of commands*.
 */
vector<Command *> ConditionParser::getInnerCommands() {
  return inner_commands;
}
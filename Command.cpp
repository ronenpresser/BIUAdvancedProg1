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

static int const STEPS_FOR_ONE_PARAMETER_COMMAND = 2;
static int const STEPS_FOR_TWO_PARAMETERS_COMMAND = 3;
static int const STEPS_FOR_IF_OR_LOOP_COMMAND = 3;

void sleep(int sleepParam) {
    //while (!parser->isSleep()) {}
    mutex m;
    m.lock();
    //int milliSeconds = stoi(parser->getSleepMilliSeconds());
    this_thread::sleep_for(chrono::milliseconds(sleepParam));
    //parser->wake();
    m.unlock();
}

int SleepCommand::execute(vector <string> &tokensVector, int currentIndex, Parser *parser) {

    int sleepParameter(stoi(tokensVector.at(currentIndex + 1)));
    //parser->setSleepMilliSeconds(sleepParameter);
    //parser->sleep();
    thread sleepThread(sleep, sleepParameter);
    sleepThread.join();

    return STEPS_FOR_ONE_PARAMETER_COMMAND;

}

int IfCommand::execute(vector <string> &tokensVec, int currIndex, Parser *parser) {
    int steps = STEPS_FOR_IF_OR_LOOP_COMMAND;

    int index = currIndex + 3;
    buildCommandsVector(tokensVec, parser, index);

    if (isConditionTrue(tokensVec.at(currIndex + 1), *parser)) {
        vector <Command> commands = getInnerCommands();
        for (Command c : commands) {
            steps += c.execute(tokensVec, currIndex, parser);
        }
    }

    return steps + 1;
}

int LoopCommand::execute(vector <string> &tokensVec, int currIndex, Parser *parser) {
    int steps = STEPS_FOR_IF_OR_LOOP_COMMAND;
    int index = currIndex + 3;
    buildCommandsVector(tokensVec, parser, index);
    while (isConditionTrue(tokensVec.at(currIndex + 1), *parser)) {
        vector <Command> commands = getInnerCommands();
        for (Command c : commands) {
            steps += c.execute(tokensVec, currIndex, parser);
        }
    }

    return steps + 1;
}

int OpenServerCommand::execute(vector <string> &tokensVec, int currIndex, Parser *parser) {
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd == -1) {
        std::cerr << "Cannot create a socket" << std::endl;
    }
    sockaddr_in address;
    address.sin_family = AF_INET;
    adress.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = hton(stoi(parser->getInterpreter()->interpretMathExpression(tokensVec.at(currIndex + 2)).calculate()));
    if (bind(socketfd), (sockaddr * ) & address, sizeof(address))==-1) {
        std::cerr << "Cannot bind socket to ip" << std::endl;

    }
    if (listen(socketfd, SOMAXCON) == -1) {
        std::cerr << "Error during listen command" << std::endl;
    }
    int client_socket = accept(socketfd, (struct sockaddr *) &address, (socklen_t * ) & address);
    if (client_socket == -1) {
        std::cerr << "Error due to accept command" << std::endl;
    }
    char *buffer;
    for (i = 0; i < 36; i++) {
        if (read(client_socket, (char *) buffer, sizeof(buffer), 0) == -1) {
            std::cerr << "Error reading from client" << std::endl;
        }
        char *arr[36];
        arr[i] = buffer;
    }
    return 2;
}

ConnectCommand:: int execute(vector <string> &tokensVec, int currIndex, Parser *parser) {
    string ip = tokensVec.at(currIndex + 1);
    int port = hton(stoi(parser->getInterpreter()->interpretMathExpression(tokensVec.at(currIndex + 2)).calculate()));
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd == -1) {
        std::cerr << "Cannot create a socket" << std::endl;
    }
    sockaddr_in address;
    address.sin_family = AF_INET;
    adress.sin_addr.s_addr = ip;
    address.sin_port = htons(port);
    if (connect(socketfd, (struct sockaddr *) &address, sizeof(address) == -1) {
        std::cerr << "Cannot establish connection" << std::endl;
    }
    char *buffer;
    ifstream file("fly.txt");
    if (!file.is_open()) {
        cerr << "Error opening file" << std::endl;
    }
    while (file >> buffer) {
        if (send(socketfd, (char *) buffer, sizeof(buffer), 0) == -1) {
            std::cerr << "Error sending message to server" << std::endl;
        }
    }
    return 3;
}

void ConditionParser::buildCommandsVector(vector <string> &tokensVec, Parser *parser, int index) {
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
            //TODO: add a Command for assignment of the var.
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

bool ConditionParser::isConditionTrue(string conditionString, Parser &pars) {
    auto par = pars;
    return par.getInterpreter()->interpretBoolExpression(conditionString)->calculate() == 1 ? true : false;
}

void ConditionParser::insert_to_inner_commands(Command c) {
    this->inner_commands.push_back(c);
}

vector <Command> ConditionParser::getInnerCommands() {
    return inner_commands;
}
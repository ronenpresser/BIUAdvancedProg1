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

int OpenServerCommand::execute(vector<string> &tokensVec, int currIndex, Parser *parser) {
    int client_socket,sockserver,sockclient;
    sockserver = socket(AF_INET, SOCK_STREAM, 0);
    if (sockserver== -1) {
        std::cerr << "Cannot create a socket" << std::endl;
    }
    sockaddr_in address1;
    address1.sin_family = AF_INET;
    address1.sin_addr.s_addr = INADDR_ANY;
    address1.sin_port =
            htons((int) (parser->getInterpreter()->interpretMathExpression(tokensVec.at(currIndex + 2))->calculate()));
    if (bind(sockserver, (sockaddr *) &address1, sizeof(address1))==-1) {
        std::cerr << "Cannot bind socket to ip" << std::endl;

    }
    if (listen(sockserver, SOMAXCONN) == -1) {
        std::cerr << "Error during listen command" << std::endl;
    }

    client_socket = accept(sockserver, (struct sockaddr *) &address1, (socklen_t *) &address1);
    if (client_socket == -1) {
        std::cerr << "Error due to accept command" << std::endl;
    }

    sockclient = socket(AF_INET,SOCK_STREAM,0);
    // char* buffer="--generic=socket,out,10,127.0.0.1," + tokensVec.at(currIndex +1) + ",tcp,generic_small";
    // int vals= send(client_socket,(char*)buffer,strlen(buffer),0);
    //buffer is a member of OpenServerCommand Class. we also need to do getBuffer() method.
    int vl = read(client_socket, parser->getBuffer(), sizeof(getBuffer()));
    for(int i=0;i<strlen(getBuffer());i++) {//i=i+2?
        string str = parser->getIndexToSimPathMap[i];
        Variable v = parser->getSimPathToVarMap[str];
        parser->getSymbolTable.setValue(v.getString(), getBuffer[i]);
    }
    return 2;
}
int ConnectCommand::execute(vector<string> &tokensVec, int currIndex, Parser *parser) {
    int sock;
    string ip = tokensVec.at(currIndex + 1);
    int port = parser->getInterpreter()->interpretMathExpression(tokensVec.at(currIndex + 2))->calculate();
    sock= socket(AF_INET, SOCK_STREAM, 0);
    if (sock== -1) {
        std::cerr << "Cannot create a socket" << std::endl;
    }
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = stoi(ip);
    address.sin_port = htons(port);
    if (connect(sock, (struct sockaddr *) &address, sizeof(address) == -1)) {
        std::cerr << "Cannot establish connection" << std::endl;
    }
    return 3;
}
int DefineVarCommand::execute(vector <string> vector, int currIndex, Parser *parser) {
    if (tokensVec.at(currIndex + 2) == '->') {
        char *buffer = "set" + vector.at(currentIndex + 2) +
                       to_string(parse.getVariable(vector.at(currentIndex + 1).getValue());
        send(socketfd, (char *) buffer, strlen(buffer), 0);
    }
    if (tokensVec.at(currIndex + 2) == '<-') {
        string var = parser.getSimPathToVarMap.get(vector.at(currIndex + 4);
        float val = var.getValue();
        parser.symbol_table.setValue(val);
    }
    return 4;
}

int PrintCommand::execute(vector<string> &tokensVec, int currIndex, Parser *parser) {
    std::cout<<tokensVec.at(currIndex +1)<<std::endl;
    return 2;
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
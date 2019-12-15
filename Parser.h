//
// Created by amit on 14/12/2019.
//

#ifndef BIUADVANCEDPROG1__PARSER_H_
#define BIUADVANCEDPROG1__PARSER_H_
#include <string>
#include <map>
#include "Command.h"

using namespace std;
class Parser {
 private:
  map<string, Command> commands_map;
  map<string,tuple<double,string>> symbol_table;
 public:
  void parse(string tokensArray[]);

};

#endif //BIUADVANCEDPROG1__PARSER_H_

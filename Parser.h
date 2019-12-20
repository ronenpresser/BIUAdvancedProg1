//
// Created by amit on 14/12/2019.
//

#ifndef BIUADVANCEDPROG1__PARSER_H_
#define BIUADVANCEDPROG1__PARSER_H_
#include <string>
#include <map>
#include <vector>

using namespace std;
class Command{


};

class Symbol{


};
class Parser {
 private:
  map<string, Command> commands_map;
  map<string, Symbol> symbol_table;
 public:
  void parse(vector<string> tokensVector);
  void buildMaps(vector<string> tokensVector);
};





#endif //BIUADVANCEDPROG1__PARSER_H_

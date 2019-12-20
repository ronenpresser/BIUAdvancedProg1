//
// Created by amit on 14/12/2019.
//

#include "Parser.h"

void Parser::parse(vector<string> tokensVector) {

  int index = 0;

  while (index < tokensVector.size()) {
    Command c = this->commands_map[tokensVector[index]];
//    if(c != null){
//      index+= c.execute();
//    }
  }
}
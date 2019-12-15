//
// Created by amit on 14/12/2019.
//

#include "Parser.h"


void Parser::parse(string *tokensArray) {

  int index = 0;

  while(index < tokensArray->length()){
    Command c =this->commands_map[tokensArray[index]];
//    if(c != null){
//      index+= c.execute();
//    }
  }
}
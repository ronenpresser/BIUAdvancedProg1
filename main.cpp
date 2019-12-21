

#include "Lexer.h"
#include "Parser.h"
#include <fstream>
int main(int argc, char const *argv[]) {


  Lexer *lexer = new Lexer();

  Parser *parser = new Parser();

  vector<string> *tokensVec = lexer->lexer("../fly.txt");

  //parser->buildMaps(lexer->lexer("../fly.txt"));
  //parser->parse(lexer->lexer(argv[1])); //tokensVec

  //delete tokensVec;
  //delete lexer;
  //delete parser;
  return 0;

}
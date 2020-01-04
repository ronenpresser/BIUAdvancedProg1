

#include "Lexer.h"
#include "Parser.h"

int main(int argc, char const *argv[]) {

  //TODO change the path to argv
  vector<string> tokens = (new Lexer())->lexer("../fly_with_func.txt");
  Parser *parser = new Parser();
  parser->parse(tokens);
  delete parser;
  return 0;
}
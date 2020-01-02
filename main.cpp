

#include "Lexer.h"
#include "Parser.h"
#include <fstream>
#include <thread>
void parse1(Parser *parser, string path) {
  Lexer *lexer = new Lexer();
  parser->parse(lexer->lexer(path));

  delete lexer;
}

int main(int argc, char const *argv[]) {

  //TODO change the path to argv
  vector<string> tokens = (new Lexer())->lexer("../fly1.txt");
  Parser * parser = new Parser();

  parser->parse(tokens);


  delete parser;

  return 0;

}
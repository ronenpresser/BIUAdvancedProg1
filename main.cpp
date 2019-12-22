

#include "Lexer.h"
#include "Parser.h"
#include <fstream>
#include <thread>
#include <mutex>


void parse(Parser *parser, string path) {
  Lexer *lexer = new Lexer();
  parser->parse(lexer->lexer(path));
}

int main(int argc, char const *argv[]) {

  Parser *parser = new Parser();
  thread parserThread(parse, parser, "../fly.txt"); // argv[1]
  Lexer *lexer = new Lexer();
  vector<string> *tokensVec = lexer->lexer("../fly.txt");


  parserThread.join();

  //parser->buildMaps(lexer->lexer("../fly.txt"));
  //parser->parse(lexer->lexer(argv[1])); //tokensVec

  //delete tokensVec;
  //delete lexer;
  delete parser;

  return 0;

}


#include "Lexer.h"
#include "Parser.h"
#include <fstream>
#include <thread>
void parse(Parser *parser, string path) {
  Lexer *lexer = new Lexer();
  parser->parse(lexer->lexer(path));

  delete lexer;
}

int main(int argc, char const *argv[]) {

  Parser *parser = new Parser();
  thread parserThread(parse, parser, "../fly.txt"); // argv[1]
  //Lexer *lexer = new Lexer();
  //vector<string> *tokensVec = lexer->lexer("../fly.txt");


  parserThread.join();

  delete parser;

  return 0;

}
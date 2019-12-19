

#include "Lexer.h"
#include "Parser.h"
int main(int argc, char const *argv[]) {

  Lexer *lexer = new Lexer();

  Parser *parser = new Parser();

  lexer->lexer("../fly.txt");

  parser->buildMaps(lexer->lexer("../fly.txt"));
  parser->parse(lexer->lexer(argv[1]));

  delete lexer;
  delete parser;
  return 0;

}
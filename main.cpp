

#include "Lexer.h"
#include "Parser.h"
int main(int argc, char const *argv[]) {

  Lexer lexer;

  Parser *parser = new Parser();

  lexer.lexer("../fly.txt");
  parser->parse(lexer.lexer(argv[1]));


  delete parser;
  return 0;

}
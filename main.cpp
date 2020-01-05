

#include <iostream>
#include "Lexer.h"
#include "Parser.h"

int main(int argc, char const *argv[]) {

  //TODO change the path to argv
  try {
    vector<string> tokens;
    try {
      tokens = (new Lexer())->lexer(argv[1]);
    } catch (...) {
      throw "Please provide a valid path to the txt file.";
    }
    Parser *parser = new Parser();
    parser->parse(tokens);
    delete parser;
  } catch (const char *e) {
    cout << e << endl;
  }

  return 0;
}
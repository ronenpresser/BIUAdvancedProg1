

#include "Lexer.h"
#include "Parser.h"
#include <fstream>
int main(int argc, char const *argv[]) {

  fstream f("../value.txt");
  fstream of("../output.txt");
  string line;
  int i = 0;
  while (getline(f, line)) {

    if (line.find("node")) {
      string path = line.substr(6, line.length() - 6 - 7);


      of << "this->index_sim_table.insert(make_pair(" << i <<
         ",\"" << path << "\"));";

      i++;
    }

  }

  f.close();
  of.close();
  //Lexer *lexer = new Lexer();

  //Parser *parser = new Parser();

  //vector<string> *tokensVec = lexer->lexer("../fly.txt");

  //parser->buildMaps(lexer->lexer("../fly.txt"));
  //parser->parse(lexer->lexer(argv[1])); //tokensVec

  //delete tokensVec;
  //delete lexer;
  //delete parser;
  return 0;

}


#include "Lexer.h"
#include "Parser.h"
#include <fstream>
#include <thread>
#include <mutex>

//mutex m;
void sleep(int milliSeconds) {
  //m.lock();
  this_thread::sleep_for(chrono::milliseconds(milliSeconds));
  //m.unlock();
}

int main(int argc, char const *argv[]) {

  //TODO change the path to argv
  vector<string> tokens = (new Lexer())->lexer("../fly.txt");
  Parser *parser = new Parser();
  parser->parse(tokens);
  delete parser;
  return 0;
}
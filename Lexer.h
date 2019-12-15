//
// Created by amit on 14/12/2019.
//



#ifndef BIUADVANCEDPROG1__LEXER_H_
#define BIUADVANCEDPROG1__LEXER_H_

#include <string>
#include <vector>

using namespace std;
class Lexer {

 public:
  string *lexer(string filePath);

  void addVarTokensToVector(const string &line, std::vector<string> &tokensVector) const;
  void addMethodTokensToVector(const string &line, vector<string> &tokensVector) const;
  bool isCommandMethod(const string &line) const;
};

#endif //BIUADVANCEDPROG1__LEXER_H_

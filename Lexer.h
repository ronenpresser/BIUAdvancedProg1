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
  vector<string> lexer(string filePath);

  void addVarTokensToVector(const string &line, std::vector<string> &tokensVector) const;
  void addMethodTokensToVector(const string &line, vector<string> &tokensVector) const;
  bool isCommandMethod(const string &line) const;
  bool isFuncDefiningCommand(const string &line) const;
  bool isWhileOrConditionCommand(const string &line) const;
  void toLowerCase(string &lowerCaseLine) const;
  void toWithoutSpaces(string &withoutSpaces) const;
  bool isVarDefineCommand(const string &line) const;
  void addWhileOrIfTokensToVector(vector<string> &linesVector, string &line, vector<string> &tokensVector, unsigned int &i) const;
};

#endif //BIUADVANCEDPROG1__LEXER_H_

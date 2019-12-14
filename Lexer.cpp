//
// Created by amit on 14/12/2019.
//

#include "Lexer.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
using namespace std;

string *Lexer::lexer(string filePath) {

  string line;
  int linesNum = 0;
  vector<string> linesVector;
  vector<string> tokensVector;
  string path = "../fly.txt";
  fstream file(path, ios::in);

  while (getline(file, line)) {
    linesNum++;
    linesVector.push_back(line);
  }

  regex r("^[A-Za-z]+\\(");

  unsigned int i = 0;
  for (i = 0; i < linesNum; i++) {

    line = linesVector.at(i);
    bool isMethod = regex_match(line, r);
    //var lines
    if (line.find("var") != string::npos) {
      addVarTokensToVector(line, tokensVector);
    } else if (isMethod) { //Method lines
      addCommandTokensToVector(line, tokensVector);
    } else if (line.find("while") != string::npos
        || line.find("if") != string::npos) { //while and if lines.
      vector<std::string> tokens;
      string token;
      istringstream tokenStream(line);
      while (getline(tokenStream, token, ' ')) {
        tokens.push_back(token);
      }
      for (string innerToken : tokens) {
        tokensVector.push_back(innerToken);
      }
      int index = i + 1;
      while (index <= linesVector.size() && linesVector.at(index) != "}") {
        string innerLine = linesVector.at(index);
        //trim all spaces.
        innerLine.erase(std::remove(innerLine.begin(),
                                    innerLine.end(), ' '), innerLine.end());
        //trim tabs.
        innerLine.erase(std::remove(innerLine.begin(),
                                    innerLine.end(), '\t'), innerLine.end());

        vector<std::string> innerLinesTokens;
        string innerLineToken;
        istringstream tokenStream(innerLine);
        if (regex_match(line, r)) { // if its a method
          addCommandTokensToVector(innerLine, linesVector);
        } else { //if its a var assignment
          while (getline(tokenStream, innerLineToken, '=')) {
            innerLinesTokens.push_back(token);//TODO to think how to interpret math expressions with variables.
          }
          for(string intok : innerLinesTokens){
            tokensVector.push_back(intok);
          }
        }
        index++;
      }
      push

      i = index;

    }

  }

  string tokens[tokensVector.size()];

  for (unsigned i = 0; i < tokensVector.size(); i++) {

    tokens[i] = tokensVector.at(i);

  }

  file.close();

  return tokens;

}
void Lexer::addCommandTokensToVector(const string &line, vector<string> &tokensVector) const {
  vector<string> tokens;
  string token;
  istringstream tokenStream(line);
  while (getline(tokenStream, token, '(')) {
    tokens.push_back(token);
  }
  tokensVector.push_back(tokens.at(0));
  tokensVector.push_back(tokens.at(1).substr(0, tokens.at(1).length() - 1));
}
void Lexer::addVarTokensToVector(const string &line, vector<string> &tokensVector) const {
  vector<string> tokens;
  string token;
  istringstream tokenStream(line);
  while (getline(tokenStream, token, ' ')) {
    tokens.push_back(token);
  }
  for (string innerToken : tokens) {
    if (innerToken.find("sim") != string::npos) {
      tokensVector.push_back("sim");

      string inParens = innerToken.substr(
          innerToken.find_first_of("\""),
          innerToken.find_first_of(')') - innerToken.find_first_of("\""));
      tokensVector.push_back(inParens);
    } else {
      tokensVector.push_back(innerToken);
    }
  }
}

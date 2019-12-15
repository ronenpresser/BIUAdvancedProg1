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
    //bool isMethod = regex_match(line, r);
    bool isMethod = isCommandMethod(line);
    //var lines
    if (line.find("var") != string::npos) {
      addVarTokensToVector(line, tokensVector);
    } else if (isMethod) { //Method lines
      addMethodTokensToVector(line, tokensVector);
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
        istringstream innerTokenStream(innerLine);
        if (isCommandMethod(innerLine)) { // if its a method
          addMethodTokensToVector(innerLine, tokensVector);
        } else { //if its a var assignment
          while (getline(innerTokenStream, innerLineToken, '=')) {
            tokensVector.push_back(innerLineToken);//TODO to think how to interpret math expressions with variables.
          }
        }
        index++;
      }
      tokensVector.push_back("}");

      i = index;

    } else {
      string withoutSpaces = line;
      withoutSpaces.erase(std::remove(withoutSpaces.begin(),
                                      withoutSpaces.end(), ' '), withoutSpaces.end());
      vector<std::string> lineTokens;
      string lineToken;
      istringstream lineTokenStream(withoutSpaces);
      while (getline(lineTokenStream, lineToken, '=')) {
        tokensVector.push_back(lineToken);
        //lineTokens.push_back();
      }
    }
  }

  string tokens[tokensVector.size()];

  for (unsigned i = 0; i < tokensVector.size(); i++) {

    tokens[i] = tokensVector.at(i);

  }

  file.close();

  return tokens;

}
bool Lexer::isCommandMethod(const string &line) const {
  string lowerCaseLine = line;
  transform(lowerCaseLine.begin(), lowerCaseLine.end(), lowerCaseLine.begin(), ::tolower);
  bool isMethod = lowerCaseLine.find("print") != string::npos
      || lowerCaseLine.find("sleep") != string::npos
      || lowerCaseLine.find("connectcontrolclient") != string::npos
      || lowerCaseLine.find("opendataserver") != string::npos;
  return isMethod;
}
void Lexer::addMethodTokensToVector(const string &line, vector<string> &tokensVector) const {
  vector<string> tokens;
  string token;
  istringstream tokenStream(line);
  while (getline(tokenStream, token, '(')) {
    tokens.push_back(token);
  }
  tokensVector.push_back(tokens.at(0));

  //has more than one parameters
  if (tokens.at(1).find(",") != string::npos) {
    string parameters = tokens.at(1);
    parameters.erase(std::remove(parameters.begin(),
                                 parameters.end(), ')'), parameters.end());
    vector<string> tokens1;
    string token1;
    istringstream tokenStream1(parameters);
    while (getline(tokenStream1, token1, ',')) {
      tokensVector.push_back(token1);
      //tokens.push_back(token1);
    }
  } else {
    tokensVector.push_back(tokens.at(1).substr(0, tokens.at(1).length() - 1));

  }
}
void Lexer::addVarTokensToVector(const string &line, vector<string> &tokensVector) const {
  vector<string> tokens;
  string token;
  istringstream tokenStream(line);
  while (getline(tokenStream, token, ' ')) {
    if (token.find("sim") != string::npos) {
      tokensVector.push_back("sim");

      string inParens = token.substr(
          token.find_first_of("\""),
          token.find_first_of(')') - token.find_first_of("\""));
      tokensVector.push_back(inParens);
    } else {
      tokensVector.push_back(token);
    }
  }
}

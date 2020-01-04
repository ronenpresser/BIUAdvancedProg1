
#include "Lexer.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
using namespace std;
vector<string> funcNames;

vector<string> Lexer::lexer(string filePath) {

  string line;
  unsigned int linesNum = 0;
  vector<string> linesVector;
  vector<string> tokensVector;
  string path = filePath;
  fstream file(path, ios::in);
  if (!file) throw "There was a problem reading the file" + filePath;
  while (getline(file, line)) {
    linesNum++;
    linesVector.push_back(line);
  }

  //regex r("^[A-Za-z]+\\(");

  unsigned int i = 0;
  for (i = 0; i < linesNum; i++) {

    line = linesVector.at(i);
    string withoutSpaces = line;
    toWithoutSpaces(withoutSpaces);
    withoutSpaces.erase(std::remove(withoutSpaces.begin(),
                                    withoutSpaces.end(), '\t'), withoutSpaces.end());
    if (withoutSpaces.empty()) continue;
    //bool isMethod = regex_match(line, r);
    bool isCommand = isCommandMethod(line);
    bool isFuncDefining = isFuncDefiningCommand(line);
    bool isWhileOrIf = isWhileOrConditionCommand(line);
    bool isVarCommand = isVarDefineCommand(line);
    //var lines
    if (isVarCommand) {
      addVarTokensToVector(line, tokensVector);
    } else if (isCommand) { //Method lines
      addMethodTokensToVector(line, tokensVector);
    } else if (isWhileOrIf) {//while or if lines
      addWhileOrIfTokensToVector(linesVector, line, tokensVector, i);
    } else if (isFuncDefining) { //defining new function lines
      vector<std::string> tokens;
      string token;
      istringstream tokenStream(line);
      //separate by (.
      while (getline(tokenStream, token, '(')) {
        tokens.push_back(token);
      }
      string newFuncName = tokens.at(0);
      //erase spaces
      toWithoutSpaces(newFuncName);
      tokensVector.push_back(newFuncName);
      funcNames.push_back(newFuncName);

      //TODO can add here support for multiple params.

      //change the param name to name of the form : funcName_paramName

      string paramName = newFuncName + "_";
      string secondPartOfFuncDefining = tokens.at(1);
      //erase spaces
      toWithoutSpaces(secondPartOfFuncDefining);
      //3 for var.
      paramName += secondPartOfFuncDefining.substr(3, secondPartOfFuncDefining.find_first_of(')') - 3);
      tokensVector.push_back(paramName);
      tokensVector.push_back("{");
      unsigned int index = i + 1;
      //Go over the next lines until you get } meaning that the end of the defining.
      while (index <= linesVector.size() && linesVector.at(index) != "}") {
        string innerLine = linesVector.at(index);
        //trim all spaces.
        innerLine.erase(std::remove(innerLine.begin(),
                                    innerLine.end(), ' '), innerLine.end());
        //trim tabs.
        innerLine.erase(std::remove(innerLine.begin(),
                                    innerLine.end(), '\t'), innerLine.end());

        if (innerLine.empty()) continue;
        vector<std::string> innerLinesTokens;
        string innerLineToken;
        istringstream innerTokenStream(innerLine);
        if (isCommandMethod(innerLine)) { // if its a Command method(print,sleep..)
          addMethodTokensToVector(innerLine, tokensVector);
        } else if (isWhileOrConditionCommand(innerLine)) { // if its a while or if
          addWhileOrIfTokensToVector(linesVector, innerLine, tokensVector, index);
        } else { //if its a var assignment
          toWithoutSpaces(innerLine);
          while (getline(innerTokenStream, innerLineToken, '=')) { // separate by =
            tokensVector.push_back(innerLineToken);
          }
        }
        index++;
      }
      tokensVector.push_back("}");
      i = index;
    } else {
      string withoutSpaces = line;
      toWithoutSpaces(withoutSpaces);
      string lineToken;
      istringstream lineTokenStream(withoutSpaces);
      while (getline(lineTokenStream, lineToken, '=')) {
        tokensVector.push_back(lineToken);
      }
    }
  }

  file.close();

  return tokensVector;

}
void Lexer::addWhileOrIfTokensToVector(vector<string> &linesVector,
                                       string &line,
                                       vector<string> &tokensVector,
                                       unsigned int &i) const { //while and if lines.
  string lowerCaseLine = line;
  this->toLowerCase(lowerCaseLine);
  if (line.substr(0, 2) == "if") {
    tokensVector.push_back("if");
    line = line.substr(2, line.length() - 2 - 1);
  } else if (line.substr(0, 5) == "while") {
    tokensVector.push_back("while");
    line = line.substr(5, line.length() - 5 - 1);
  }
  string noSpaces = line;
  this->toWithoutSpaces(noSpaces);
  tokensVector.push_back(noSpaces);
  tokensVector.push_back("{");
  unsigned int index = i + 1;
  while (index <= linesVector.size() && linesVector.at(index).find("}") == string::npos) {
    string innerLine = linesVector.at(index);

    int startingIndex = 0;
    auto it = innerLine.begin();

    while (innerLine.at(startingIndex) != '_' && !isalpha(innerLine.at(startingIndex))) {
      ++it;
      ++startingIndex;
    }
    //trim tabs from start to first char that is not '_' or letter.
    innerLine.erase(std::remove(innerLine.begin(),
                                it, '\t'), it);
    //trim all spaces from start to first char that is not '_' or letter.
    innerLine.erase(std::remove(innerLine.begin(),
                                it, ' '), it);

    vector<std::string> innerLinesTokens;
    string innerLineToken;
    istringstream innerTokenStream(innerLine);
    if (this->isCommandMethod(innerLine)) { // if its a method
      this->addMethodTokensToVector(innerLine, tokensVector);
    } else { //if its a var assignment
      innerLine.erase(std::remove(innerLine.begin(),
                                  innerLine.end(), ' '), innerLine.end());
      istringstream innerTokenStream(innerLine);
      while (getline(innerTokenStream, innerLineToken, '=')) {
        tokensVector.push_back(innerLineToken);
      }
    }
    index++;
  }
  tokensVector.push_back("}");

  i = index;

}
void Lexer::toWithoutSpaces(string &withoutSpaces) const {
  withoutSpaces.erase(remove(withoutSpaces.begin(),
                             withoutSpaces.end(), ' '), withoutSpaces.end());
}
bool Lexer::isWhileOrConditionCommand(const string &line) const {
  return ((line.find("while") != string::npos
      || line.find("if") != string::npos)
      && (line.find("<") != string::npos
          || line.find(">") != string::npos
          || line.find(">=") != string::npos
          || line.find("<=") != string::npos
          || line.find("==") != string::npos
          || line.find("!=") != string::npos));
}
bool Lexer::isFuncDefiningCommand(const string &line) const {
  return line.find('{') != string::npos && line.find("(") != string::npos
      && line.find(")") != string::npos;
}

bool Lexer::isVarDefineCommand(const string &line) const {
  return line.find("var") != string::npos
      && (line.find("<-") != string::npos || line.find("->") != string::npos || line.find("=") != string::npos);
}
bool Lexer::isCommandMethod(const string &line) const {
  string lowerCase = line;
  toLowerCase(lowerCase);
  bool isMethod = lowerCase.find("print(") != string::npos
      || lowerCase.find("sleep(") != string::npos
      || lowerCase.find("connectcontrolclient(") != string::npos
      || lowerCase.find("opendataserver(") != string::npos;
  bool isNewExecutingFunc;
  for (string s : funcNames) {
    string func = s + "(";
    if (line.find(func) != string::npos && line.find("{") == string::npos) {
      isNewExecutingFunc = true;
      break;
    }
  }
  return isMethod || isNewExecutingFunc;
}
void Lexer::toLowerCase(string &lowerCaseLine) const {
  transform(lowerCaseLine.begin(),
            lowerCaseLine.end(),
            lowerCaseLine.begin(),
            ::tolower);
}
void Lexer::addMethodTokensToVector(const string &line, vector<string> &tokensVector) const {
  //first separate by (
  vector<string> tokens;
  string token;
  istringstream tokenStream(line);
  while (getline(tokenStream, token, '(')) {
    tokens.push_back(token);
  }
  //the first token is the name of the method command.
  tokensVector.push_back(tokens.at(0));

  if (tokens.at(1).find(",") != string::npos) {  //has more than one parameters
    string parameters = tokens.at(1);
    parameters.erase(std::remove(parameters.begin(),
                                 parameters.end(), ')'), parameters.end());
    vector<string> tokens1;
    string token1;
    istringstream tokenStream1(parameters);
    while (getline(tokenStream1, token1, ',')) {
      tokensVector.push_back(token1);
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
      //tokensVector.push_back("sim");

      string inParens = token.substr(
          token.find_first_of("\""),
          token.find_first_of(')') - token.find_first_of("\""));

      if (inParens.at(1) == '/') {
        inParens = inParens.substr(2, inParens.length() - 3);
      }
      tokensVector.push_back(inParens);
    } else {
      tokensVector.push_back(token);
    }
  }
}

//
// Created by amit on 05/11/2019.
//

#include <queue>
#include <stack>
#include <sstream>
#include "ex1.h"
#include <map>
#include <string>
#include <iostream>
#include <regex>
#include <stdlib.h>

using namespace std;

Expression *Interpreter::interpret(string expressionString) {

  if (expressionString == "" || (count(expressionString.begin(), expressionString.end(), '(')
      != count(expressionString.begin(), expressionString.end(), ')')))
    throw "Invalid math expression.";

  queue<tuple<string, bool>> outputQueue;
  //the operator and if it is an unary operator.
  stack<tuple<string, bool>> operatorStack;
  string operatorsString = "-+/*";

  //Create a map or dictionary that holds the precedences of operators.
  //The tuple is built with the precedence of an operator and if its left associative.
  map<string, tuple<int, bool>> operatorsPrecs = map<string, tuple<int, bool>>();
  operatorsPrecs.insert(pair<string, tuple<int, bool>>("+", tuple<int, bool>(1, true)));
  operatorsPrecs.insert(pair<string, tuple<int, bool>>("-", tuple<int, bool>(1, true)));
  operatorsPrecs.insert(pair<string, tuple<int, bool>>("*", tuple<int, bool>(2, true)));
  operatorsPrecs.insert(pair<string, tuple<int, bool>>("/", tuple<int, bool>(2, true)));

  for (unsigned i = 0; i < expressionString.length(); i++) {

    char token = expressionString[i];
    //Convert the char to string so we can push it into the strings queue.
    string a(1, token);
    //Check for vars
    if (token == '_' || islower(token)) {
      if (i != 0) {
        //if there is a - before a variable , its not valid.
        if (expressionString[i - 1] == '-') {
          throw "Invalid math expression.";
        }
      }
      int start = i + 1;
      int end = 1;

      while (isalpha(expressionString[start]) || isdigit(expressionString[start])
          || expressionString[start] == '_') {
        start++;
        end++;
      }
      if (!this->varMap.count(expressionString.substr(i, end))) {
        throw "At least one of the vars in the expression was not set, or its name is wrong.";
      }
      outputQueue.push(tuple<string, bool>(expressionString.substr(i, end), false));
      i = start - 1;

    }
      //check for numbers
    else if (isdigit(token)) {
      int start = i + 1;
      int end = 1;
      while (isdigit(expressionString[start]) || expressionString[start] == '.') {
        start++;
        end++;
      }
      outputQueue.push(tuple<string, bool>(to_string(stod(expressionString.substr(i, end))), false));

      i = start - 1;

    }
      //check for operators
    else if (operatorsString.find(token) != string::npos) {

      //check if there is an operator before and after the current token ,
      // or if its an - and there is a variable after.
      if ((((i + 1 <= expressionString.length() - 1
          && operatorsString.find(expressionString[i + 1]) != string::npos))
          || (token == '-'
              && ((i + 1 <= expressionString.length() - 1 && (isalpha(expressionString[i + 1])))
                  || expressionString[i + 1] == '_')))) {
        throw "illegal math expression";
      }

      while (!operatorStack.empty()
          && ((get<0>(operatorsPrecs[get<0>(operatorStack.top())]) > get<0>(operatorsPrecs[a]))
              && get<1>(operatorsPrecs[get<0>(operatorStack.top())]))
          && get<0>(operatorStack.top()) != "(") {
        outputQueue.push(operatorStack.top());
        operatorStack.pop();
      }
      if (i == 0) {
        if ((token == '-' || token == '+') && expressionString[i + 1] == '(')
          operatorStack.push(tuple<string, bool>(a, true));
        else
          throw "illegal math expression";
      } else if ((token == '-' || token == '+') && isdigit(expressionString[i + 1])
          && expressionString[i - 1] == '(') { // "+3" or "-3"
        int start = i + 1;
        int end = 1;
        while (isdigit(expressionString[start]) || expressionString[start] == '.') {
          start++;
          end++;
        }
        outputQueue.push(tuple<string, bool>(to_string(stod(expressionString.substr(i, end))), false));

        i = start - 1;
      } else {
        if (expressionString[i - 1] == '('
            && (token == '-' || token == '+') && expressionString[i + 1] == '(')
          operatorStack.push(tuple<string, bool>(a, true));
        else
          operatorStack.push(tuple<string, bool>(a, false));
      }
    } else if (token == '(') {
      operatorStack.push(tuple<string, bool>(a, false));
    } else if (token == ')') {
      while (!operatorStack.empty() && get<0>(operatorStack.top()) != "(") {
        outputQueue.push(operatorStack.top());
        operatorStack.pop();
      }
      if (operatorStack.empty()) {
        throw "Invalid math expression.";
      }
      operatorStack.pop();
    } else { //there are not valid chars in the expression.
      throw "Invalid math expression.";

    }
  }

  while (!operatorStack.empty()) {
    outputQueue.push(operatorStack.top());
    operatorStack.pop();
  }

  stack<Expression *> expressionsStack;
  while (!outputQueue.empty()) {
    tuple<string, bool> frontTuple = outputQueue.front();
    bool isUnary = get<1>(frontTuple);
    string front = get<0>(outputQueue.front());
    outputQueue.pop();
    try {
      double n = stod(front);

      expressionsStack.push(new Value(n));

    } catch (const invalid_argument &e) {
      if (varMap.count(front))
        expressionsStack.push(new Variable(front, varMap[front]));
      if (operatorsString.find(front) != string::npos) {

        Expression *right = expressionsStack.top();
        expressionsStack.pop();

        if (isUnary) {
          if (front == "+") {
            expressionsStack.push(new UPlus(right));
          }
          if (front == "-") {
            expressionsStack.push(new UMinus(right));
          }
        } else {

          Expression *left = expressionsStack.top();
          expressionsStack.pop();

          if (front == "+") {
            expressionsStack.push(new Plus(left, right));
          }
          if (front == "-") {
            expressionsStack.push(new Minus(left, right));
          }
          if (front == "*") {
            expressionsStack.push(new Mul(left, right));
          }
          if (front == "/") {
            expressionsStack.push(new Div(left, right));
          }
        }
      }
    }
  }

  operatorsPrecs.clear();
  Expression *e = expressionsStack.top();
  expressionsStack.pop();

  return e;
}

void Interpreter::setVariables(string expressionString) {

  if (expressionString.find('=') == string::npos) {
    throw "illegal variable assignment!";
  }

  string newString;
  if (expressionString[expressionString.length() - 1] == ';')
    newString = expressionString.substr(0, expressionString.length() - 1);
  else
    newString = expressionString;

  //Split the expression by ';'.
  //Got help from https://www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/
  vector<std::string> tokens;
  string token;
  istringstream tokenStream(newString);
  while (getline(tokenStream, token, ';')) {
    tokens.push_back(token);
  }
  //Then, split each of the tokens by '='.

  for (string s : tokens) {

    vector<std::string> innerTokens;
    string innerToken;
    istringstream innerTokenStream(s);
    while (getline(innerTokenStream, innerToken, '=')) {
      innerTokens.push_back(innerToken);
    }
    string pattern1("^_?[a-zA-Z][a-zA-Z0-9]*(_?[a-zA-Z0-9]*)*$");
    string pattern2("^(-?)(0|([1-9][0-9]*))(.[0-9]+)?$");
    regex validVariablePattern(pattern1);
    regex validNumberPattern(pattern2);

    try {
      if (!regex_match(innerTokens.at(0), validVariablePattern)
          || !regex_match(innerTokens.at(1), validNumberPattern)) {
        throw "illegal variable assignment!";
      }
      double value = stod(innerTokens.at(1));
      bool empty = this->varMap.empty();
      pair<string, double> p = make_pair(innerTokens.at(0), value);
      if (!empty) {
        if (this->varMap.count(innerTokens.at(0))) {
          this->varMap[innerTokens.at(0)] = value;
        } else
          this->varMap.insert(p);
      } else {
        this->varMap.insert(p);
      }
    } catch (...) {
      throw "illegal variable assignment!";
    }
  }
}


//Destructors

UnaryOperator::~UnaryOperator() {

  //delete this->expression;
}

UPlus::~UPlus() {

  delete this->expression;
}

UMinus::~UMinus() {

  delete this->expression;
}

BinaryOperator::~BinaryOperator() {

  //delete this->right;
  //delete this->left;
}

Plus::~Plus() {

  delete this->right;
  delete this->left;
}

Minus::~Minus() {

  delete this->right;
  delete this->left;
}

Mul::~Mul() {

  delete this->right;
  delete this->left;
}

Div::~Div() {

  delete this->right;
  delete this->left;
}

//Variable

Variable &Variable::operator++() {

  ++this->value;
  return *this;

}

Variable &Variable::operator--() {
  --this->value;
  return *this;

}

Variable &Variable::operator+=(const double valueToAdd) {
  value += valueToAdd;
  return *this;

}

Variable &Variable::operator-=(const double valueToSub) {
  value -= valueToSub;
  return *this;
}

Variable &Variable::operator++(const int) {
  this->value++;
  return *this;
}

Variable &Variable::operator--(const int) {
  this->value--;
  return *this;
}

//Calculate


double UPlus::calculate() {
  try {
    double expVal = this->expression->calculate();

    return expVal;

  }
  catch (const char *e) {
    throw e;
  }
}

double UMinus::calculate() {
  try {
    double expVal = this->expression->calculate();

    return -expVal;

  }
  catch (const char *e) {
    throw e;
  }
}

double Plus::calculate() {
  try {
    double rightExp = this->right->calculate();
    double leftExp = this->left->calculate();

    return leftExp + rightExp;

  }
  catch (const char *e) {
    throw e;
  }
}

double Minus::calculate() {
  try {
    double rightExp = this->right->calculate();
    double leftExp = this->left->calculate();

    return leftExp - rightExp;

  }
  catch (const char *e) {
    throw e;
  }
}

double Mul::calculate() {
  try {
    double rightExp = this->right->calculate();
    double leftExp = this->left->calculate();

    return leftExp * rightExp;

  }
  catch (const char *e) {
    throw e;
  }

}

double Div::calculate() {
  try {
    double rightExp = this->right->calculate();
    double leftExp = this->left->calculate();
    if (rightExp == 0) {

      throw "Cannot divide by zero";
    }

    return leftExp / rightExp;

  }
  catch (const char *e) {
    throw e;
  }
}
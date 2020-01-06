//
// Created by amit on 05/11/2019.
//

#ifndef UNTITLED1_EX1_H
#define UNTITLED1_EX1_H

#include <string>
#include "Expression.h"
#include <map>
#include <functional>

using namespace std;

class Value : public Expression {
 private:
  double value;

 public:
  Value(double val) : value(val) {}
  Value(const Value *val) {
    this->value = val->value;
  }

  virtual ~Value() {}

  double getValue() const { return this->value; }

  virtual double calculate() {
    double val = getValue();
    return val;
  }

};

class Variable : public Expression {
 private:
  double value;
  string name;
  string simulatorPath;
  bool bindingDirection; // false for right, true for left.
  bool isLocalVar;
 public:
  string getName() {
    return this->name;
  }

  void setValue(double val) {
    this->value = val;
  }

  bool getBindingDirection() {
    return this->bindingDirection;
  }

  bool isLocalVariable() {

    return this->isLocalVar;
  }

  string getSimulatorPath() {
    return simulatorPath;
  }
  float getValue() {
    return this->value;
  }
  virtual double calculate() {
    return value;
  }
  Variable(string nameString, double val, string path, bool bindingDir, bool isLocalVariable) : value(val),
                                                                                                name(nameString),
                                                                                                bindingDirection(
                                                                                                    bindingDir),
                                                                                                isLocalVar(
                                                                                                    isLocalVariable) {
    if (!isLocalVariable) {
      if (path.at(0) == '/') {
        this->simulatorPath = path.substr(1, path.length() - 1);
      } else { simulatorPath = path; }
    }
  }

  Variable(string nameString, double val, string path) : value(val), name(nameString), simulatorPath(path) {}

  Variable(string nameString, double val) : value(val), name(nameString) {}

  virtual ~Variable() {}

  Variable &operator++();

  Variable &operator--();

  Variable &operator+=(const double value);

  Variable &operator-=(const double value);

  Variable &operator++(const int value);

  Variable &operator--(const int value);

};
class InterpretTool {

 private:
  map<string, double> varMap;
 public:
  Expression *interpretMathExpression(string expressionString);

  void setVariables(string expressionString);

  ~InterpretTool() {
    if (!varMap.empty()) {
      varMap.clear();
    }
  }
  Expression *interpretBoolExpression(string expressionString);
  void addParensAroundVars(string &expressionString) const;
  void eraseFromVarMap(string varName) {
    if (this->varMap.count(varName))
      this->varMap.erase(varName);
  }
};

class UnaryOperator : public Expression {
 protected:
  Expression *expression;
 public:
  UnaryOperator(Expression *exp) : expression(exp) {}

  virtual ~UnaryOperator();

};

class UPlus : public UnaryOperator {
 public:
  UPlus(Expression *exp) : UnaryOperator(exp) {}

  virtual ~UPlus();

  double calculate();
};

class UMinus : public UnaryOperator {
 public:
  UMinus(Expression *exp) : UnaryOperator(exp) {}

  virtual ~UMinus();

  double calculate();

};

class BinaryOperator : public Expression {
 protected:
  Expression *left;
  Expression *right;

 public:
  BinaryOperator(Expression *leftExp, Expression *rightExp) : left(leftExp), right(rightExp) {}

  virtual ~BinaryOperator();

};

class Plus : public BinaryOperator {
 public:
  Plus(Expression *leftExp, Expression *rightExp) : BinaryOperator(leftExp, rightExp) {}

  virtual ~Plus();

  double calculate();

};

class Minus : public BinaryOperator {
 public:
  Minus(Expression *leftExp, Expression *rightExp) : BinaryOperator(leftExp, rightExp) {}

  virtual ~Minus();

  double calculate();
};

class Mul : public BinaryOperator {
 public:
  Mul(Expression *leftExp, Expression *rightExp) : BinaryOperator(leftExp, rightExp) {}

  virtual ~Mul();

  double calculate();
};

class Div : public BinaryOperator {

 public:
  Div(Expression *leftExp, Expression *rightExp) : BinaryOperator(leftExp, rightExp) {}

  virtual ~Div();

  double calculate();
};

class BooleanExpression : public BinaryOperator {
 private:
  string boolCon;
  map<string, const function<bool(const double &, const double &)>> boolOperators;
 public:
  BooleanExpression(Expression *leftExp, string boolOperator, Expression *rightExp) : BinaryOperator(leftExp,
                                                                                                     rightExp) {
    boolCon = boolOperator;
    //<,>,<=,>=,==,!=
    //The key is the string of the boolean operator and and value is a boolean value that a lambda function returns.
    auto stringBoolPairL = make_pair(
        "<",
        [](const double &leftE, const double &rightE) {
          return leftE < rightE;
        });
    boolOperators.insert(stringBoolPairL);
    auto stringBoolPairG = make_pair(
        ">", [](const double &leftE, const double &rightE) {
          return leftE > rightE;
        });
    boolOperators.insert(stringBoolPairG);
    auto stringBoolPairLE = make_pair(
        "<=", [](const double &leftE, const double &rightE) {
          return leftE <= rightE;
        });
    boolOperators.insert(stringBoolPairLE);
    auto stringBoolPairGE = make_pair(
        ">=", [](const double &leftE, const double &rightE) {
          return leftE >= rightE;
        });
    boolOperators.insert(stringBoolPairGE);
    auto stringBoolPairE = make_pair(
        "==", [](const double &leftE, const double &rightE) {
          return leftE == rightE;
        });
    boolOperators.insert(stringBoolPairE);
    auto stringBoolPairNE = make_pair(
        "!=", [](const double &leftE, const double &rightE) {
          return leftE != rightE;
        });
    boolOperators.insert(stringBoolPairNE);
  }

  virtual ~BooleanExpression() {}

  double calculate();
};

#endif //UNTITLED1_EX1_H

//
// Created by amit on 05/11/2019.
//

#ifndef UNTITLED1_EX1_H
#define UNTITLED1_EX1_H

#include <string>
#include "Expression.h"
#include <map>

using namespace std;

class ex1 {

};

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
 public:
  Variable(string nameString, double val) {
    this->name = nameString;
    this->value = val;
  }

  virtual ~Variable() {}

  Variable &operator++();

  Variable &operator--();

  Variable &operator+=(const double value);

  Variable &operator-=(const double value);

  Variable &operator++(const int value);

  Variable &operator--(const int value);

  string getName() {
    return this->name;
  }

  virtual double calculate() {
    return value;
  }
};

class Interpreter {

 private:
  map<string, double> varMap;
 public:
  Expression *interpret(string expressionString);

  void setVariables(string expressionString);

  ~Interpreter() {
    if (!varMap.empty()) {
      varMap.clear();
    }
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

#endif //UNTITLED1_EX1_H

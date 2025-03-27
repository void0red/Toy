//
// Created by ikaros on 2025/3/26.
//

#ifndef AST_HPP
#define AST_HPP
#include <memory>
#include <string>
#include <vector>

namespace Toy {

class ExprAST {
public:
  virtual ~ExprAST() = default;
};

class NumberExprAST : public ExprAST {
  double value;

public:
  explicit NumberExprAST(double value) : value(value) {}
};

class VariableExprAST : public ExprAST {
  std::string name;

public:
  explicit VariableExprAST(const std::string &name) : name(name) {}
};

class BinaryExprAST : public ExprAST {
  char opcode;
  std::unique_ptr<ExprAST> lhs, rhs;

public:
  BinaryExprAST(char op, ExprAST *lhs, ExprAST *rhs)
      : opcode(op), lhs(lhs), rhs(rhs) {}
};

// class UnaryExprAST : public ExprAST {
//   char opcode;
//   std::unique_ptr<ExprAST> operand;
//
// public:
//   UnaryExprAST(char op, std::unique_ptr<ExprAST> operand)
//       : opcode(op), operand(std::move(operand)) {}
// };

class CallExprAST : public ExprAST {
  std::string callee;
  std::vector<std::unique_ptr<ExprAST>> arguments;

public:
  CallExprAST(const std::string &callee,
              std::vector<std::unique_ptr<ExprAST>> arguments)
      : callee(callee), arguments(std::move(arguments)) {}
};

class PrototypeAST : public ExprAST {
  std::string name;
  std::vector<std::string> arguments;

public:
  PrototypeAST(const std::string &name, std::vector<std::string> arguments)
      : name(name), arguments(std::move(arguments)) {}
};

class FunctionAST : public ExprAST {
  std::unique_ptr<PrototypeAST> proto;
  std::unique_ptr<ExprAST> body;

public:
  FunctionAST(std::unique_ptr<PrototypeAST> proto,
              std::unique_ptr<ExprAST> body)
      : proto(std::move(proto)), body(std::move(body)) {}
};
} // namespace Toy

#endif // AST_HPP

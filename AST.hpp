//
// Created by ikaros on 2025/3/26.
//

#ifndef AST_HPP
#define AST_HPP
#include "Logger.hpp"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <memory>
#include <string>
#include <vector>

extern std::unique_ptr<llvm::LLVMContext> TheContext;
extern std::unique_ptr<llvm::IRBuilder<>> Builder;
extern std::unique_ptr<llvm::Module> TheModule;
extern std::unordered_map<std::string, llvm::Value *> NamedValues;

namespace Toy {

class ExprAST {
public:
  virtual ~ExprAST() = default;
  virtual std::string to_string() const = 0;
  virtual llvm::Value *codegen() = 0;
};

class NumberExprAST : public ExprAST {
  double value;

public:
  explicit NumberExprAST(double value) : value(value) {}
  std::string to_string() const override;
  llvm::Value *codegen() override;
};

class VariableExprAST : public ExprAST {
  std::string name;

public:
  explicit VariableExprAST(const std::string &name) : name(name) {}
  std::string to_string() const override;
  llvm::Value *codegen() override;
};

class BinaryExprAST : public ExprAST {
  std::unique_ptr<ExprAST> lhs, rhs;

public:
  enum class OpType { ADD, SUB, MUL, DIV, LT, LE, GT, GE, EQ, NE };
  BinaryExprAST(OpType op, ExprAST *lhs, ExprAST *rhs)
      : opcode(op), lhs(lhs), rhs(rhs) {}
  std::string to_string() const override;
  llvm::Value *codegen() override;

private:
  OpType opcode;
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
              std::vector<std::unique_ptr<ExprAST>> &arguments)
      : callee(callee), arguments(std::move(arguments)) {}
  std::string to_string() const override;
  llvm::Value *codegen() override;
};

class PrototypeAST : public ExprAST {
  std::string name;
  std::vector<std::string> arguments;

public:
  PrototypeAST(const std::string &name, std::vector<std::string> arguments)
      : name(name), arguments(std::move(arguments)) {}
  std::string to_string() const override;
  const std::string &getName() const;
  llvm::Function *codegen() override;
};

class FunctionAST : public ExprAST {
  std::unique_ptr<PrototypeAST> proto;
  std::unique_ptr<ExprAST> body;

public:
  FunctionAST(PrototypeAST *proto, ExprAST *body) : proto(proto), body(body) {}
  std::string to_string() const override;
  llvm::Function *codegen() override;
};

class IfElseExprAST : public ExprAST {
  std::unique_ptr<ExprAST> condition;
  std::unique_ptr<ExprAST> then;
  std::unique_ptr<ExprAST> else_;

public:
  IfElseExprAST(ExprAST *condition, ExprAST *then, ExprAST *else_)
      : condition(condition), then(then), else_(else_) {}
  std::string to_string() const override;
  llvm::Value *codegen() override;
};
} // namespace Toy

#endif // AST_HPP

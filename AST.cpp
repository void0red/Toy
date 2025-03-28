//
// Created by ikaros on 2025/3/26.
//

#include "AST.hpp"

#include <format>

namespace Toy {
std::string NumberExprAST::to_string() const { return std::to_string(value); }
std::string VariableExprAST::to_string() const { return this->name; }
std::string BinaryExprAST::to_string() const {
  return std::format("{} {} {}", this->lhs->to_string(), this->opcode,
                     this->rhs->to_string());
}
std::string CallExprAST::to_string() const {
  std::string args;
  for (auto &i : arguments) {
    args += i->to_string() + ",";
  }
  if (!args.empty()) {
    args.pop_back();
  }
  return std::format("{}({})", this->callee, args);
}
std::string PrototypeAST::to_string() const {
  std::string args;
  for (auto &i : arguments) {
    args += i + ",";
  }
  if (!args.empty()) {
    args.pop_back();
  }
  return std::format("PrototypeAST({}({}))", this->name, args);
}
std::string FunctionAST::to_string() const {
  return std::format("FunctionAST({})\n\t{}", this->proto->to_string(),
                     this->body->to_string());
}
std::string IfElseExprAST::to_string() const {
  return std::format("({}) ? ({}) : ({})", this->condition->to_string(),
                     this->then->to_string(), this->else_->to_string());
}
} // namespace Toy
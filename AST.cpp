//
// Created by ikaros on 2025/3/26.
//

#include "AST.hpp"

#include "magic_enum/magic_enum.hpp"
#include <format>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Verifier.h>

namespace Toy {
std::string NumberExprAST::to_string() const { return std::to_string(value); }
std::string VariableExprAST::to_string() const { return this->name; }
std::string BinaryExprAST::to_string() const {
  return std::format("{} {} {}", this->lhs->to_string(),
                     magic_enum::enum_name(this->opcode),
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

llvm::Value *NumberExprAST::codegen() {
  return llvm::ConstantFP::get(*TheContext, llvm::APFloat(value));
}
llvm::Value *VariableExprAST::codegen() {
  auto v = NamedValues[this->name];
  if (!v) {
    LOG_ERROR("Unknown variable name");
  }
  return v;
}
llvm::Value *BinaryExprAST::codegen() {
  auto l = lhs->codegen();
  auto r = rhs->codegen();
  if (!l || !r) {
    return nullptr;
  }
  switch (opcode) {
  case OpType::ADD:
    return Builder->CreateFAdd(l, r);
  case OpType::SUB:
    return Builder->CreateFSub(l, r);
  case OpType::MUL:
    return Builder->CreateFMul(l, r);
  case OpType::DIV:
    return Builder->CreateFDiv(l, r);
  case OpType::LT:
    return Builder->CreateFCmpULT(l, r);
  case OpType::LE:
    return Builder->CreateFCmpULE(l, r);
  case OpType::GT:
    return Builder->CreateFCmpUGT(l, r);
  case OpType::GE:
    return Builder->CreateFCmpUGE(l, r);
  case OpType::EQ:
    return Builder->CreateICmpEQ(l, r);
  case OpType::NE:
    return Builder->CreateICmpNE(l, r);
  default:
    LOG_ERROR("invalid binary operator");
  }
  return nullptr;
}
llvm::Value *CallExprAST::codegen() {
  auto callee = TheModule->getFunction(this->callee);
  if (!callee) {
    LOG_ERROR("Unknown function referenced");
    return nullptr;
  }
  if (callee->arg_size() != arguments.size()) {
    LOG_ERROR("Incorrect arguments passed");
    return nullptr;
  }
  std::vector<llvm::Value *> args;
  for (auto &i : arguments) {
    auto v = i->codegen();
    if (!v) {
      LOG_ERROR("Create arguments failed");
      return nullptr;
    }
    args.push_back(v);
  }
  return Builder->CreateCall(callee, args);
}
llvm::Function *PrototypeAST::codegen() {
  std::vector<llvm::Type *> doubleArgs(arguments.size(),
                                       llvm::Type::getDoubleTy(*TheContext));
  auto funcType = llvm::FunctionType::get(llvm::Type::getDoubleTy(*TheContext),
                                          doubleArgs, false);

  auto func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage,
                                     name, TheModule.get());
  int i = 0;
  for (auto &arg : func->args()) {
    arg.setName(arguments[i++]);
  }
  return func;
}
const std::string &PrototypeAST::getName() const { return name; }
llvm::Function *FunctionAST::codegen() {
  auto func = TheModule->getFunction(proto->getName());
  if (!func) {
    func = proto->codegen();
  }
  if (!func) {
    return nullptr;
  }
  if (!func->empty()) {
    LOG_ERROR("Function cannot be redefined.");
    return nullptr;
  }
  auto bb = llvm::BasicBlock::Create(*TheContext, "entry", func);
  Builder->SetInsertPoint(bb);

  NamedValues.clear();
  for (auto &arg : func->args()) {
    NamedValues[arg.getName().str()] = &arg;
  }

  if (auto ret = this->body->codegen()) {
    Builder->CreateRet(ret);
    llvm::verifyFunction(*func);
    return func;
  }

  func->eraseFromParent();
  return nullptr;
}
llvm::Value *IfElseExprAST::codegen() {
  auto cond = condition->codegen();
  if (!cond)
    return nullptr;
  cond = Builder->CreateFCmpONE(
      cond, llvm::ConstantFP::get(*TheContext, llvm::APFloat(0.0)));

  auto func = Builder->GetInsertBlock()->getParent();
  auto thenBB = llvm::BasicBlock::Create(*TheContext, "then", func);
  auto elseBB = llvm::BasicBlock::Create(*TheContext, "else", func);
  auto mergeBB = llvm::BasicBlock::Create(*TheContext, "merge", func);
  Builder->CreateCondBr(cond, thenBB, elseBB);

  Builder->SetInsertPoint(thenBB);
  auto tv = this->then->codegen();
  if (!tv) {
    return nullptr;
  }
  Builder->CreateBr(mergeBB);
  thenBB = Builder->GetInsertBlock();

  Builder->SetInsertPoint(elseBB);
  auto ev = this->else_->codegen();
  if (!ev) {
    return nullptr;
  }
  Builder->CreateBr(mergeBB);
  elseBB = Builder->GetInsertBlock();

  Builder->SetInsertPoint(mergeBB);
  auto phi = Builder->CreatePHI(llvm::Type::getDoubleTy(*TheContext), 2);
  phi->addIncoming(tv, thenBB);
  phi->addIncoming(ev, elseBB);
  return phi;
}

} // namespace Toy
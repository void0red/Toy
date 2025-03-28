#include "Scanner.hpp"
#include "toy.tab.hpp"
#include <Logger.hpp>
#include <fstream>
#include <iostream>
#include <memory>

#include <llvm/IR/LLVMContext.h>

std::unique_ptr<llvm::LLVMContext> TheContext;
std::unique_ptr<llvm::IRBuilder<>> Builder;
std::unique_ptr<llvm::Module> TheModule;
std::unordered_map<std::string, llvm::Value *> NamedValues;

void LLVMInit(const std::string &module_name) {
  TheContext = std::make_unique<llvm::LLVMContext>();
  TheModule = std::make_unique<llvm::Module>(module_name, *TheContext);
  Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
}

int main(int argc, char *argv[]) {
  Toy::Logger::instance().setLogLevel(Toy::LogLevel::DEBUG);
  if (argc != 2) {
    return -1;
  }
  std::ifstream file(argv[1]);
  if (!file.good() && file.eof()) {
    return -1;
  }
  LLVMInit(argv[1]);
  auto scanner = std::make_unique<Toy::Scanner>(&file);
  auto parser = std::make_unique<Toy::Parser>(*scanner);
  parser->parse();
  TheModule->print(llvm::outs(), nullptr);
  return 0;
}
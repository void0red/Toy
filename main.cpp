#include "Scanner.hpp"
#include "toy.tab.hpp"
#include <fstream>
#include <iostream>

#include <memory>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    return -1;
  }
  std::ifstream file(argv[1]);
  if (!file.good() && file.eof()) {
    return -1;
  }
  auto scanner = std::make_unique<Toy::Scanner>(&file);
  auto parser = std::make_unique<Toy::Parser>(*scanner);
  parser->parse();
  return 0;
}
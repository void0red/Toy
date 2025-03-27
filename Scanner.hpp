//
// Created by ikaros on 2025/3/27.
//

#ifndef SCANNER_HPP
#define SCANNER_HPP

#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "toy.tab.hpp"

namespace Toy {
class Scanner : public yyFlexLexer {
public:
  explicit Scanner(std::istream *in) : yyFlexLexer(in) {}

  using FlexLexer::yylex;
  virtual int yylex(Parser::value_type *yylval, Parser::location_type *loc);

private:
  Parser::semantic_type *yylval{};
  location loc;
};
} // namespace Toy
#undef YY_DECL
#define YY_DECL                                                                \
  int Toy::Scanner::yylex(Toy::Parser::value_type *yylval,                     \
                          Toy::Parser::location_type *loc)
#endif // SCANNER_HPP

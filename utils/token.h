#ifndef TOKEN_H
#define TOKEN_H

enum TokenType {
  TOKENTYPE_IDENTIFIER,
  TOKENTYPE_KEYWORD,
  TOKENTYPE_OPERATOR,
  TOKENTYPE_BOOLEAN_VALUE
};

struct Token {
  enum TokenType tType;
  char token[1024];
};

char *tokenize(char *file, int *tokenCount);

#endif

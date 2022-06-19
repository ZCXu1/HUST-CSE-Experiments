#include "token.h"

struct KeyWord_t
{
  char *key;
  enum TokenKind_t kind;
};

struct KeyWord_t keywords[] = {
  {"GET", TOKEN_GET},
  {"HEAD", TOKEN_HEAD},
  {"HTTP/1.1", TOKEN_HTTP_ONE_ONE},
  {"POST", TOKEN_POST},
  {0, 0},
};

char *TokenKind_toString (enum TokenKind_t k)
{
  switch (k){
  case TOKEN_CRLF:
    return "<CRLF>";
  case TOKEN_EOF:
    return "eof";
  case TOKEN_GET:
    return "GET";
  case TOKEN_HEAD:
    return "HEAD";
  case TOKEN_POST:
    return "POST";
  case TOKEN_HTTP_ONE_ONE:
    return "HTTP/1.1";
  case TOKEN_SPACE:
    return "<space>";
  case TOKEN_STR:
    return "<str>";
  default:
    return "<junk>";
  }
  return "<error>";
}

enum TokenKind_t Token_getKeyWord(char *s)
{
  int i = 0;
  while (keywords[i].key){		
    if (strcmp(s, keywords[i].key)==0)	
      return keywords[i].kind;		
    i++;
  }
  return -1;
}

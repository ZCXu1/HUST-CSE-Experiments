#ifndef TOKEN_H
#define TOKEN_H

enum TokenKind_t{
  TOKEN_CRLF,
  TOKEN_EOF,
  TOKEN_GET,
  TOKEN_HEAD,
  TOKEN_HTTP_ONE_ONE,  
  TOKEN_SPACE,
  TOKEN_POST,
  TOKEN_STR,
};

struct Token_t
{
  enum TokenKind_t kind;
  char *lexeme;
};

#define Token_new(token,k,l)			\
  do{						\
    token.kind=k;				\
    token.lexeme=l;				\
   }while(0)					\

     //#define Token_new(token,k,l)  do{token.kind=k;token.lexeme=l; }while(0)	
			       
char *TokenKind_toString (enum TokenKind_t);
enum TokenKind_t Token_getKeyWord(char *s);


#endif

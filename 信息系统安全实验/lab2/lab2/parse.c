#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"
#include "token.h"

#define DEBUG 1

Body_t Body_new (char *key, char *value)
{
  Body_t p = malloc (sizeof(*p));
  if (0==p)
    outOfMemory();

  strcpy(p->key, key);
  strcpy(p->value,value);
  return p;
}
enum {A_NONE, A_SPACE, A_CRLF} ahead = A_NONE;

struct Token_t token = {-1, 0};

Http_t http400 = 0;
Http_t generate400();

//////////////////////////////////////
// prototypes
void advance (int fd, int sepBySpace);
char *eatToken (enum TokenKind_t, int, int);

char getChar (int fd)
{
  int n;
  char c;
  char *info;

  n = read (fd, &c, 1);
  switch (n){
  case -1:
    info = "fail to read the socket\n";
    write (1, info, strlen (info));
    Http_print (fd, http400);
    close (fd);
    exit (0);
    break;
  case 0: // socket is blocked
    info = "read 0 char\n";
    write (1, info, strlen (info));
    Http_print (fd, http400);
    close (fd);
    exit (0);
    break;
  case 1:
    break;
  default:
    info = "server bug\n";
    write (1, info, strlen (info));
    Http_print (fd, http400);
    close (fd);
    exit (0);
    break;
  }
  return c;
}

int i;
int gfd;
void getToken (int fd, int sepBySpace)
{  
  i = 0;
  char c;
  char s[1024];
  gfd=fd;
  
  unsigned int *framep;
  // Save the ebp value into framep
  asm("movl %%ebp, %0" : "=r"(framep));
  printf("Frame Pointer (inside getToken):      0x%.8x\n", (unsigned int) framep);
    
  switch (ahead){
  case A_NONE:
    c = getChar (gfd);
    break;
  case A_SPACE:
    ahead = A_NONE;
    Token_new(token, TOKEN_SPACE, 0);
    return;
  case A_CRLF:
    ahead = A_NONE;
    Token_new(token, TOKEN_CRLF, 0);
    return;
  default:{
    char *info = "server bug";
    write (1, info, strlen (info));
    Http_print (gfd, http400);
    close (gfd);
    exit (0);
    return;
  }
  }

  while (1){
    switch (c){
    case ' ':
      if (sepBySpace){
	if (i){
	  char *p;
	  int kind;
	  
	  // remember the ' '
	  ahead = A_SPACE;
	  s[i] = '\0';
	  p = malloc (strlen(s)+1);
	  strcpy (p, s);
	  kind = Token_getKeyWord (p);
	  if (kind>=0){
	    
	    Token_new (token, kind, 0);
	    return;
	  }
	  Token_new (token, TOKEN_STR, p);
	  return;
	}
	Token_new(token, TOKEN_SPACE, 0);
	return;
      }
      s[i++] = c;
      break;
    case '\r':{
      char c2;
      
      c2 = getChar (gfd);
      if (c2=='\n'){
	if (i){
	  char *p;
	  int kind;
	  // remember the ' '
	  ahead = A_CRLF;
	  s[i] = '\0';
	  p = malloc (strlen(s)+1);
	  strcpy (p, s);
	  kind = Token_getKeyWord (p);
	  if (kind>=0){
	    Token_new (token, kind, 0);
	    return;
	  }
	  Token_new (token, TOKEN_STR, p);
	  return;
	}
	Token_new(token, TOKEN_CRLF, 0);
	return;
      }
      s[i++] = c;
      s[i++] = c2;
      break;
    }
    default:
      s[i++] = c;
      break;
    }
    c = getChar (gfd);
  }
  return;
}

void parseError(int fd)
{
  //eatAllChars(fd);
  Http_print (fd, http400);
  close (fd);
  exit (0);
}

char *eatToken(enum TokenKind_t kind, int fd
	       , int sepBySpace)
{
  if (token.kind == kind){
    char *s = token.lexeme;
    //advance (fd, sepBySpace);  //modify
    return s;
  }
  else {
    fprintf (stderr, "Error: parse error\n");
    fprintf (stderr, "expects: %s\n"
	     , TokenKind_toString (kind));
    fprintf (stderr, "but got: %s\n"
	     , TokenKind_toString(token.kind));
    parseError (fd);
  }
}

void advance(int fd, int sepBySpace)
{
  getToken(fd, sepBySpace);
}



ReqLine_t Parse_reqLine (int fd)
{
  enum ReqKind_t kind;
  ReqLine_t reqline;
  getToken(fd, 1);  //modify
  printf("parse(F) uri =  %s; kind = %d; n = %d\n", token.lexeme, token.kind, TOKEN_POST);
  switch(token.kind){
  case TOKEN_HEAD:
    kind = REQ_KIND_HEAD;
    break;
  case TOKEN_GET:
    kind = REQ_KIND_GET;
    break;
  case TOKEN_POST:
    
    kind = REQ_KIND_POST;
    break;
  default:
    parseError(fd);
    break;
  }
  
  advance(fd, 1);
      
  eatToken(TOKEN_SPACE, fd, 1);
  
  advance (fd, 1); //modify
  
  char *uri = eatToken(TOKEN_STR, fd, 1);
  //eatToken(TOKEN_SPACE, fd, 1);

  advance (fd, 1);
  eatToken(TOKEN_SPACE, fd , 1);

  advance(fd, 1);
  eatToken(TOKEN_HTTP_ONE_ONE, fd, 1);

  advance(fd, 1);
  eatToken(TOKEN_CRLF, fd, 1);
  ///no more eat token

  if (DEBUG){
    fprintf (stderr, "uri=%s\n", uri);
  }

  reqline = ReqLine_new ( kind
			 , uri
			 , HTTP_ONE_ONE);
  ReqLine_print (1, reqline);
  return reqline;
}

int gLength;

void parseHeaders (int fd)
{
  advance(fd,1);
  int i = 0;
  fprintf (stderr, "header starting\n");
  while (token.kind != TOKEN_CRLF){
    // this should not be string...,
    // a dirty hack
    /*if(i)
      advance(fd, 1);
      i = 1;*/
    char *str1 = eatToken(TOKEN_STR, fd, 1);
    
    advance (fd, 1);
    eatToken(TOKEN_SPACE, fd, 0);
    
    advance (fd, 0);
    char *str2 = eatToken(TOKEN_STR, fd, 1);

    if(strcmp(str1,"Content-Length:") == 0)
      {
	printf("Content-Length Match sucess Length = %s..\n",str2);
	//read length number of body
	gLength = atoi(str2);
      }
    advance (fd, 1);
    eatToken(TOKEN_CRLF, fd, 1);
    //fprintf (stderr, "eeee");
    advance(fd, 1);
    //printf("kind = %d\n",token.kind);
  }
  return;
}

Body_t b[256];


int num;
Body_t *getBody(int *n)
{
  *n = num; 
  return b;
}

int parseBody (int fd)
{
  if(gLength==0) return 0;
  printf("Body...\n");
  char *body = (char*)malloc(gLength+1);
  memset(body,0,gLength+1);
  
  int i = 0;
  while(i < gLength)
    body[i++] = getChar(fd);
  printf("Post request body = %s\n",body);
  
  i=0;
  
  char* temp=(char*)malloc(256);

  num = 0;
  while(i<=gLength)
    {
    char *s1 = strchr(body, '=');
    *s1 = 0;
    i+=strlen(body);
    strcpy(temp,body);
    body=s1+1;
    s1=strchr(body, '&');
    if(s1 == 0){
      b[num++]=Body_new(temp,body);
      break;
    }
    else{
      *s1=0;
      i+=strlen(body);
      b[num++]=Body_new(temp,body);
      body=s1+1;
    }
  }
  
  return num;
}

ReqLine_t gReqline;

void  setReqline(int kind, char *uri)
{
  ReqLine_t t =  ReqLine_new ( kind
			       , uri
			       , HTTP_ONE_ONE);
  gReqline = t;
}

void *Parse_parse (int fd, int reqOnly)
{
  http400 = generate400();
  //getToken(fd, 1);    modify
  
  ReqLine_t reqline;
  Http_t http = 0;
  if(reqOnly){
    reqline = Parse_reqLine (fd);
    return reqline;
  }
  
  parseHeaders(fd);
  if (token.kind!=TOKEN_CRLF)
    parseError(fd);
  
  int num = parseBody(fd);  
  
  reqline = gReqline;
  
  http = Http_new (HTTP_KIND_REQUEST
		   , reqline
		   , 0
		   , 0
		   , 0);
  
  if (DEBUG)
    Http_print (1, http);
  return http;
}

////////////////////////////////////
// 400
Http_t generate400()
{
  enum HttpKind_t kind = HTTP_KIND_RESPONSE;
  RespLine_t respLine;
  Header_t header = 0;
  char *body;
  Http_t http = 0;

  respLine = 
    RespLine_new (HTTP_ONE_ONE
		  , RESP_400
		  , "bad request");
  body = "";
  http = Http_new (kind
		   , 0
		   , respLine
		   , header
		   , body);
  return http;
}


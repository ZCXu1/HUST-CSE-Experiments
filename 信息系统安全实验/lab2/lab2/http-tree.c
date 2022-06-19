#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "http-tree.h"

///////////////////////////////////////
// utilities
void error (char *s, char *file, int line)
{
  fprintf (stderr
	   , "Error: (%s, %d): %s\n"
	   , file, line, s);
  exit (0);
}

void outOfMemory()
{
  char *s = "out of memory";
  write (1, s, strlen(s));
  exit (0);
}

////////////////////////////////////////
// http version
HttpVersion_print (int fd, enum HttpVersion_t v)
{
  char *s;

  switch (v){
  case HTTP_ONE_ZERO:
    s = "HTTP/1.0";
    write (fd, s, strlen(s));
    break;
  case HTTP_ONE_ONE:
    s = "HTTP/1.1";
    write (fd, s, strlen(s));
    break;
  case HTTP_TWO_ZERO:
    s = "HTTP/2.0";
    write (fd, s, strlen(s));
    break;
  default:
    error ("invalid http version"
	   , __FILE__
	   , __LINE__);
    break;
  }
}

////////////////////////////////////////
// request line
void ReqKind_print (int fd, enum ReqKind_t kind)
{
  char *s;

  switch (kind){
  case REQ_KIND_GET:
    s = "GET";
    write (fd, s, strlen(s));
    break;
  case REQ_KIND_HEAD:
    s = "HEAD";
    write (fd, s, strlen (s));
    break;
  case REQ_KIND_POST:
    s = "POST";
    write (fd, s, strlen (s));
    break;
  default:{
    char temp[100];
    sprintf (temp, "%d", kind);
    write (fd, temp, strlen(temp));
    error ("invalid method"
    	   , __FILE__
    	   , __LINE__);
    break;
  }
  }
}

ReqLine_t ReqLine_new (enum ReqKind_t kind
		       , char *uri
		       , enum HttpVersion_t v)
{
  ReqLine_t p = malloc (sizeof(*p));
  if (0==p)
    outOfMemory();

  p->kind = kind;
  p->uri = uri;
  p->httpversion = v;
  return p;
}

void ReqLine_print (int fd, ReqLine_t req)
{
  assert (req);
  ReqKind_print(fd, req->kind);
  write (fd, " ", 1);
  write (fd, req->uri, strlen(req->uri));
  write (fd, " ", 1);
  HttpVersion_print (fd, req->httpversion);
  write (fd, "\r\n", 2);
  return;
}


////////////////////////////////////////
// response
void RespKind_print(int fd, enum RespKind_t k)
{
  char *s;

  switch (k){
  case RESP_200:
    s = "200";
    write (fd, s, strlen(s));
    break;
  case RESP_400:
    s = "400";
    write (fd, s, strlen (s));
    break;
  case RESP_404:
    s = "404";
    write (fd, s, strlen(s));
    break;
  default:
    error ("invalid response"
	   , __FILE__
	   , __LINE__);
    break;
  }
}

RespLine_t RespLine_new(enum HttpVersion_t v
			, enum RespKind_t kind
			, char *info)
{
  RespLine_t p = malloc (sizeof(*p));
  if (0==p)
    outOfMemory();
  
  p->httpversion = v;
  p->kind = kind;
  p->info = info;
  return p;
}

void RespLine_print (int fd, RespLine_t resp)
{
  assert (resp);

  HttpVersion_print (fd, resp->httpversion);
  write (fd, " ", 1);
  RespKind_print (fd, resp->kind);
  write (fd, " ", 1);
  write (fd, resp->info, strlen(resp->info));
  write (fd, "\r\n", 2);
  return;
}

////////////////////////////////////////
// http
Http_t Http_new (enum HttpKind_t kind
		 , ReqLine_t reqLine
		 , RespLine_t respLine
		 , Header_t headers
		 , char *body)
{
  Http_t p = malloc (sizeof (*p));
  if (0==p)
    outOfMemory();

  p->kind = kind;
  p->reqLine = reqLine;
  p->respLine = respLine;
  p->headers = headers;
  p->body = body;
  return p;
}


void Http_print (int fd, Http_t http)
{
  assert (http);
  assert (http->reqLine || http->respLine);
  if (http->reqLine)
    ReqLine_print(fd, http->reqLine);
  else RespLine_print (fd, http->respLine);
  // more code ...
  
  return;
}

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "handle.h"
#define DEBUG 1

char *fileNotExist =
  "<html><b>File does not exist!</b></html>";
char *noPermission = 
  "<html><b>Access denied: no permission!</b></html";


void handleHead (char *uri, int fd)
{
  // to do 
  return;
}

void handleGet (char *uri, int fd)
{
  char *path;
  char *info;
  
  assert (uri);
  // fprintf (stderr, "uri=%s\n", uri);
  printf("***********uri=[%s]\n", uri);
  if (strcmp (uri, "/")==0)
    uri = "/index.html";
  path = malloc (strlen (uri)+3);
  strcpy (path, "./");
  strcpy (path+2, uri);
  if (DEBUG)
    fprintf (stderr, "#%s#", path);
  
  printf("***********Path=[%s]\n", path);
  if (access(path, F_OK)!=0){
    fprintf (stderr, "file: %s not found\n"
	     , path);
    info = "HTTP/1.1 200 OK\r\n\r\n";
    write (fd, info, strlen(info));
    write (fd, fileNotExist, strlen (fileNotExist));
    return;
  }
  if (access(path, R_OK)!=0){
    fprintf (stderr, "file: %s no perm\n"
	     , path);
    info = "HTTP/1.1 200 OK\r\n\r\n";
    write (fd, info, strlen(info));
    write (fd, noPermission, strlen (noPermission));
    return;
  }
  fprintf (stderr, "file: %s OK\n"
	   , path);
  info = "HTTP/1.1 200 OK\r\n\r\n";
  write (fd, info, strlen(info));
  {
    char c;
    int file = open (path, O_RDONLY);
    while (read (file, &c, 1)){
      write (fd, &c, 1);
    }
    close (file);
  }
  return;
}

void Handle_post(char *uri, int fd)
{
  int num;
  char *info = "HTTP/1.1 200 OK\r\n\r\n";
  Body_t *b = getBody(&num);
  // printf("num = %d\n",num);
  char *name = b[0]->value;
  char *pwd  = b[1]->value;
  // printf("name=%s;pwd=%s\n", name, pwd);
  char *type = b[num-1]->value;
  
  init_db();
  time_t now;
  char *datetime;
	
  time(&now);
  datetime = ctime(&now);
  if(strcmp(type, "Login") == 0)//login 
    {
      int t = use_db(name, pwd, 1);
      //login success
      if(t){
	char c[1024];

	
	//printf("now datetime: %s\n", datetime);
	sprintf(c,
		"<html> <head> <title>login</title> </head><body>Welcome, <b>%s</b>, This is Login Page..<p>Date now is  %s <br><form name='logoutform' method='POST' ><table><tr><td> <input type='submit' name='submit_logout' value='Logout'></td></tr></table></form></body></html>", name, datetime);
        //printf("login = %s\n",c);
	write(fd, info, strlen(info));
	write(fd, c, strlen(c));
      }
      else
	{
	  char c[1024];
	  sprintf(c,
		  "<html> <head> <title>login_Fail</title> </head><body>Sorry, %s. Login fail..<p>Please Login again..<p><form name='logoutform' method='POST' ><table><tr><td> <input type='submit' name='submit_exit' value='Exit'></td></tr></table></form></body></html>", name);
	  write(fd, info, strlen(info));
	  write(fd, c, strlen(c));
	}
      close(fd);
      
    }
  else if(strcmp(type, "Register") == 0) //register 
    {
      int t = use_db(name, pwd, 0);
      if(t){
	char c[1024];
	sprintf(c,
		"<html> <head> <title>register_Fail</title> </head><body>Sorry, %s has been registered..!!<p>Please Register again..<p><form name='exitform' method='POST'><table><tr><td> <input type='submit' name='submit_exit' value='Exit'></td></tr></table></form></body></html>", name);
	
	write(fd, info, strlen(info));
	write(fd, c, strlen(c));
      }
      else{
	
	register_db(name, pwd);
	char c[1024];
	sprintf(c,
		"<html> <head> <title>register</title> </head><body>Welcome, <b>%s</b>, you register success..<p>Date now is  %s <p><form name='logoutform' method='POST'><table><tr><td> <input type='submit' name='submit_logout' value='Logout'></td></tr></table></form></body></html>", name, datetime);
	
	write(fd, info, strlen(info));
	write(fd, c, strlen(c));
      }
      close(fd);
      
    }
  else if(strcmp(type, "Logout") == 0 || strcpy(type, "Exit"))  //logout , Exit
    {
      char c;
      write(fd, info, strlen(info));
      int file = open ("./index.html", O_RDONLY);
      while (read (file, &c, 1))
	write (fd, &c, 1);
      close(fd);
    }
  else
    close(fd);
}

void Handle_main (int fd, Http_t tree)
{
  assert (tree);
  if (tree->kind != HTTP_KIND_REQUEST){
    fprintf (stderr, "server bug");
    close (fd);
    exit (0);
  }
  
  ReqLine_t reqline = tree->reqLine;
  
  assert (reqline);
  switch (reqline->kind){
  case REQ_KIND_GET:
    handleGet(reqline->uri, fd);
    break;
  case REQ_KIND_HEAD:
    handleHead (reqline->uri, fd);
    break;
  case REQ_KIND_POST:
    Handle_post(reqline->uri, fd);
    break;
  default:
    fprintf (stderr, "server bug");
    close (fd);
    exit (0);
    break;
  }
  
  return;
}

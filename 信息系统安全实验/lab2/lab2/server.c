#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "http.h"
#include <grp.h>
#include <unistd.h>
#include <sys/types.h>
// change the default port to 80, which will
// require root privilege to bind the port.
#define HOST_PORT 80
#define DEBUG 1
#define BUF_SIZE 1024

#define DIE(msg)						\
  do{								\
    fprintf (stderr, "Error: %s:%d: %s\n",			\
	     __FILE__, __LINE__, msg);				\
    exit (0);							\
  }while(0)							\
    
int main (int argc, char **argv)
{
  int sockfd, client_sockfd;
  int host_port;
  int yes;
  char buffer[BUF_SIZE];
  struct sockaddr_in host_addr, client_addr;

  host_port = HOST_PORT;
  if (argc>1)
    host_port = atoi(argv[1]);
  
  if ((sockfd = socket (PF_INET, SOCK_STREAM, 0))==-1){
    DIE("creating a socket");
  }
  
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    DIE("setting socket option SO_REUSEADDR");

  memset(&(host_addr), '\0', sizeof(host_addr)); // zero off the structure
  host_addr.sin_family = AF_INET;
  host_addr.sin_port = htons(host_port);
  host_addr.sin_addr.s_addr = 0;

  if (bind(sockfd, (struct sockaddr *)&host_addr
	   , sizeof(struct sockaddr)) == -1)
    DIE("binding to socket");
  
  if (listen(sockfd, 1000) == -1)
    DIE("listening on socket");

  signal(SIGCHLD, SIG_IGN);
  signal(SIGPIPE, SIG_IGN);

  /* fill in here for  chroot code snippet
   *  chroot root directory be '/jail'   
   */
  
  //code here...



  // launch two separate web services: filesv and
  // mailsv. The first one serves static files and
  // the second one serves as a mail server.
  int pid;
  int file_fds[2];
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, file_fds))
       DIE("socketpair");

  if ((pid=fork())==0){

    /* fill code in here 
     * using setresuid(ruid, euid, suid) and so on..
     */

    //your code 
 
    close (file_fds[1]);

    char pipe_fd_str[32] = {'\0'};
    char *child_name = "./filesv";    
    sprintf (pipe_fd_str, "%d", file_fds[0]);
    char *child_argv[] = {child_name 
			  , pipe_fd_str
			  , 0};
    if (DEBUG)
      printf ("file fd = %s\n", pipe_fd_str);

    if (execv (child_name, child_argv)==-1)
      DIE("execve");
  }
  sleep(1);
  close(file_fds[0]);
  if (DEBUG)
    printf ("the first web service launched...\n");

  // the second web service
  int mail_fds[2];
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, mail_fds))
       DIE("socketpair");

  if ((pid=fork())==0){

    /* fill code in here 
     * using setresuid(ruid, euid, suid) and so on..
     */

    //your code here..
   
    
    close (mail_fds[1]);
    char pipe_fd_str[32] = {'\0'};
    char *child_name = "./banksv";    
    sprintf (pipe_fd_str, "%d", mail_fds[0]);
    char *child_argv[] = {child_name 
			  , pipe_fd_str
			  , 0};
    if (DEBUG)
      printf ("mail fd = %s\n", pipe_fd_str);

    if (execv (child_name, child_argv)==-1)
      DIE("execve");
  }
  sleep(1);
  close(mail_fds[0]);
  if (DEBUG)
    printf ("the second web service launched...\n");
  
  // the http dispatcher service
  int disp_fds[2];
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, disp_fds))
       DIE("socketpair");

  if ((pid=fork())==0){
    
     /* fill code in here 
      * using setresuid(ruid, euid, suid) and so on..
      */

     //your code 
	 
     close (disp_fds[1]);
    char pipe_fd_str[32] = {'\0'};
    char *child_name = "./httpd";    
    sprintf (pipe_fd_str, "%d", disp_fds[0]);
    char *child_argv[] = {child_name 
			  , pipe_fd_str
			  , 0};
    if (DEBUG)
      printf ("%s\n", pipe_fd_str);

    if (execv (child_name, child_argv)==-1)
      DIE("execve");
  }
  sleep(1);
  close (disp_fds[0]);
  if (DEBUG)
    printf ("the http dispatcher service launched...\n");
  
  // send pipe fds to http dispatcher
  char temp[32] = {'\0'};
  sprintf (temp, "%d", file_fds[1]);
  if (-1==sendfd(disp_fds[1], temp, strlen(temp)+1, file_fds[1]))
    DIE ("send fd");
  if (DEBUG)
    printf ("sending %s...\n", temp);
  char temp2[32] = {'\0'};
  sprintf (temp2, "%d", mail_fds[1]);
  if (-1==sendfd(disp_fds[1], temp2, strlen(temp2)+1, mail_fds[1]))
    DIE ("send fd");
  if (DEBUG)
    printf ("sending %s...\n", temp2);
  close(file_fds[1]);
  close(mail_fds[1]);

  // accept connections...
  while (1){
    int size;
    
    if ((client_sockfd = accept 
	 (sockfd, (struct sockaddr *)&client_addr, &size))==-1)
      DIE("accepting client connection");

    printf("Parent process client_sockfd = %d\n",client_sockfd);	

    if (DEBUG)
      printf ("server: accepting a client from %s port %d\n"
	      , inet_ntoa (client_addr.sin_addr)
	      , ntohs (client_addr.sin_port));

    char *sock_fd_str = "hello, world\n";
    //sprintf (sock_fd_str, "%d", client_sockfd);
    if (DEBUG){
      printf ("sending a socket to the client:\n");
    }
    if (-1==sendfd(disp_fds[1], sock_fd_str, strlen(sock_fd_str)+1,client_sockfd))
      DIE ("send sock fd");

    close(client_sockfd);
  }
  return 0;
}


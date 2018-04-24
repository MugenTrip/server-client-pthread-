#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>
#include <arpa/inet.h>

void error(char *msg)
{
  perror(msg);
  exit(0);
}


char* itoa(int val, int base);



int main(int argc, char *argv[])
{
  int sockfd, portno, n;

  struct sockaddr_in serv_addr;
  struct hostent *server;

  char buffer[256];
  if (argc < 3) {
    fprintf(stderr,"usage %s hostname port\n", argv[0]);
    exit(0);
  }
  portno = atoi(argv[2]);
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  
  if (sockfd < 0) 
    error("ERROR opening socket");
  
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET; 
  serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
  serv_addr.sin_port = htons(portno);
  if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
    error("ERROR connecting");

  bzero(buffer,256);
  n = read(sockfd,buffer,255);
  if (n < 0) 
    error("ERROR reading from socket");
  printf("%s\n",buffer);

  bzero(buffer,256);
  while(1){
      fgets(buffer,256,stdin);
      if(!strcmp(buffer,"exit\n")){
        strcpy(buffer,"Client disconnects from port :  \n");
        n = send(sockfd,buffer,strlen(buffer),0);
        printf("Client disconnects from server %d\n",ntohs(serv_addr.sin_port));
        break;
      }
      else{
        n = send(sockfd,buffer,strlen(buffer),0);
        bzero(buffer,sizeof(buffer));
        if (n < 0){
          error("ERROR writing to socket");
        }
      }
  }


  return 0;
}


char* itoa(int val, int base){
  
  static char buf[32] = {0};
  
  int i = 30;
  
  for(; val && i ; --i, val /= base)
  
    buf[i] = "0123456789abcdef"[val % base];
  
  return &buf[i+1];
  
}
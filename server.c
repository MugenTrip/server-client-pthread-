#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h> 
#include <sys/syscall.h>
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
  exit(1);
}


void *connection_handler(void *socket_desc);

int main(int argc, char const *argv[])
{
  int s_sock, new_s_sock,client,portno, clilen;
  char buffer[256];
  struct sockaddr_in serv_addr;
  struct sockaddr_in cli_addr;
  int n,t_id;
  
  //Arguments check
  if (argc < 2) {
  	fprintf(stderr,"ERROR, no port provided\n");
    exit(1);
  }
  
  //Create server socket
  s_sock = socket(AF_INET, SOCK_STREAM, 0);
  
  if (s_sock < 0){ 
    error("ERROR opening socket");
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  
  //Binding
  if (bind(s_sock, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){ 
  	error("ERROR on binding");
  }
  
  //Listening
  listen(s_sock,5);

  //Accept and incoming connection
    puts("Waiting for incoming connections...");
    clilen = sizeof(cli_addr);
	pthread_t thread_id;

  //Accept a client
  while( new_s_sock = accept(s_sock, (struct sockaddr *) &cli_addr, &clilen) ){
  	if (new_s_sock < 0){ 
    	error("ERROR on accept");
  	}
  	printf("Connection accepted from  %s %d \n",inet_ntoa(cli_addr.sin_addr), ntohs( cli_addr.sin_port));
  	
  	if((t_id=pthread_create( &thread_id , NULL ,  connection_handler , (void*) &new_s_sock))< 0)
        {
            perror("could not create thread");
            return 1;
        }
        printf("Handler for client : %s assigned \n",inet_ntoa(cli_addr.sin_addr));
    }
  
  	pthread_join( thread_id , NULL);
  
  return 0; 
}


/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[2000];
     
    //Send some messages to the client
    message = "Greetings! I am your connection handler\n";
    write(sock , message , strlen(message));
     
    message = "Chat or write exit: \n";
    write(sock , message , strlen(message));
     
    //Receive a message from client
    while(read_size = recv(sock , client_message , 255 , 0))
    {	
    	if(read_size == 0)
    	{
        	printf("Client disconnected");
        	fflush(stdout);
        }
    	else if(read_size == -1)
		{
        	perror("recv failed");
    	}
		//Print client's message
		if(!strcmp(client_message,"exit\n"))
		{
			printf("Client Disconnected\n");
			break;
		}
        
        printf("Client sent: %s\n",client_message );
		bzero(client_message, sizeof(client_message));
		
    }

    close(sock);    
    return 0;
}
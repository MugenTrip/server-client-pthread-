#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>



void error(char *msg)
{
  perror(msg);
  exit(0);
}

int main(int argc, char *argv[])
{
	//create socket
	int client;
	struct hostent *server;
	client = socket(AF_INET, SOCK_STREAM,0);

	//specify an address for the socket
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(atoi(argv[1]));
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);

	getnameinfo( (struct sockaddr *) server_address, sizeof(server_address),
	 argv[2] , sizeof(argv[2]), NULL, NULL, 0);

	//Conect
	int con_status = connect(client , (struct sockaddr *) &server_address , sizeof(server_address));
	if (con_status == -1)
	{
		printf("There was an error waiting a connection to the socket \n");
	}

	//Receive data from the server
	char server_response[2560];
	recv(client,&server_response,sizeof(server_response),0);

	//print the server's response
	printf("The serever sent:\n %s",server_response);

	close(client);
}

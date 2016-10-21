/**************************************************************************************
*
*  FILE NAME	: SRCCE_server_main.c
*
*  DESCRIPTION: This file contains the code for main function.
*
*************************************************************************************/

#include "SRCCE_server_header.h"  

int main( int argc, char *argv[])
{
	/* declare the socket vairable to store the 
		server's socket descriptor */ 
	int socket_server = ZERO;
	
	/* declare the socket descriptor array for clients */
	int socket_client[MAX_CLIENTS] = {ZERO};
	
	/* Variable to store the address of server and client */
	struct sockaddr_in server,client;
	
	/* variable to store the size of client structure */
	int client_size = ZERO;
	
	/* to take return value of bind function */
	int error_bind = ZERO;
			
	/* to note the number of threads */
	int thread_count = ZERO;
	
	/* Declare the thread array */
	pthread_t tid[MAX_CLIENTS] = {ZERO};
	
	/* Variable to store the return value of thread create */
	int thread_error = ZERO;
	
	char port_no[PORT] = {ZERO};
	char ip[IP] = {ZERO};
	int port = ZERO;
	
	/* initialize the server's address */
	SRCCE_server_initialize(ip, port_no);
	
	port = atoi(port_no);
	
	printf("\n server ip is %s",ip);
	fflush(stdout);
	
	/* create a socket */
	socket_server = SRCCE_server_create_socket(socket_server, AF_INET, SOCK_STREAM, ZERO);
	
	/* load the server address in the server structure */
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);
	
	/* Bind the socket */
	error_bind = bind( socket_server, (struct sockaddr *)&server, sizeof(server));
	
	/* check for error */
	if( NEG == error_bind)
	{
		printf("\n Failed to bind the socket");
		SRCCE_server_create_log("Failed to bind the socket" , "SERVER"); 
		exit(EXIT_FAILURE);
	}
	
	printf("\n Bind Successful...\n");
	SRCCE_server_create_log("Bind Successful..." , "SERVER"); 
	
	/* listen for incoming connections  */
	listen(socket_server, 10);
	printf("\n listening for connections...\n");
	
	SRCCE_server_create_log("listening for connections..." , "SERVER"); 
	
	/* acccept the incoming connections */
	printf("\n waiting for connections...\n");
	SRCCE_server_create_log("waiting for connections..." , "SERVER"); 
	
	/* start the live server */
	while(TRUE)
	{
		signal(SIGINT, SRCCE_server_sigint_handler);
		
		/* take the size of client address structure */
		client_size = sizeof(client);
		
		/* accept the connect request from client */
		socket_client[thread_count] = accept(socket_server, (struct sockaddr *)&client, (socklen_t*)&client_size);
		
		/* check for error */
		if( NEG == socket_client[thread_count])
		{
			printf("\n failed to accept the connection...\n");
			SRCCE_server_create_log("failed to accept the connection..." , "SERVER"); 
			exit(EXIT_FAILURE);
		}
		
		printf("\n Connection accepted...\n");
		SRCCE_server_create_log("Connection accepted..." , "SERVER"); 
	
		/* check if maximum clients reached */
		if( thread_count < MAX_CLIENTS)
		{
			/* create a thread for this socket */
			thread_error = pthread_create(&tid[thread_count],NULL,&SRCCE_client_handler,(void*)&socket_client[thread_count]);
		
			/*check for error while creating thread */
			if(thread_error != ZERO)
			{
				printf("Error in thread creation for the client...");
				SRCCE_server_create_log("Error in thread creation for the client..." , "SERVER"); 
				exit(EXIT_FAILURE);
			}	
			
			/* increment thread count */
			thread_count++;
		}
		else
		{
			printf("\n maximum limit reached\n");
			fflush(stdout);
		}	
	}	
		
	/* close the socket */
	close(socket_server);
	
	return EXIT_SUCCESS;
}
 
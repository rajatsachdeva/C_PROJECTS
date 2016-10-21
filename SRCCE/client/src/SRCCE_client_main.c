/**************************************************************************************
*
*  FILE NAME	: SRCCE_client_main.c
*
*  DESCRIPTION: This file contains the main function for SRCCE system.
*
*************************************************************************************/

#include "SRCCE_client_header.h"  

int main( int argc, char *argv[])
{
	int socket_client = ZERO;
	int connect_error = ZERO;
	char *recv_msg = NULL;
	char log_msg[MSG] = {ZERO};
	int user_type = INVALID_USER;
	char *username = NULL;
	char *password = NULL;
	int length_flag = INVALID_LEN;
			
	/* structure to store the server address */
	struct sockaddr_in server;

	/* create a socket */
	socket_client = SRCCE_client_create_socket(socket_client,AF_INET, SOCK_STREAM, 0);
	
	char port_no[PORT] = {ZERO};
	char ip[IP] = {ZERO};
	int port = ZERO;
	
	SRCCE_client_initialize(ip, port_no);
	
	port = atoi(port_no);
	
	printf("\n server ip is %s",ip);
	fflush(stdout);

	/* load the server address */
	server.sin_addr.s_addr = inet_addr(ip);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	/* connect with the server */
	printf("\n connecting with server....");
	SRCCE_client_create_log( "connecting with server....", "CLIENT"); 

	connect_error = connect( socket_client, (struct sockaddr *)&server, sizeof(server));

	/* check for errror */
	if( CONNECT_ERROR == connect_error)
	{
		printf("\n Failed to connect with the server\n");
		SRCCE_client_create_log( " Failed to connect with the server", "CLIENT"); 
		exit(EXIT_FAILURE);
	}	
	
	printf("\n Connection Established\n");
	SRCCE_client_create_log( "Connection Established", "CLIENT");
	
	system("clear");
	
	/* Receive welcome message */
	recv_msg = (char *)malloc(sizeof(char) * MSG);
	
	/* check for error while allocating memory */
	if( NULL == recv_msg)
	{
		printf("\n Failed to allocate the memory to recv_msg \n");
		SRCCE_client_create_log("Failed to allocate the memory to recv_msg" , "admin"); 
		exit(EXIT_FAILURE);
	}
	SRCCE_client_receive_message(socket_client,recv_msg);
	printf(" %s ",recv_msg);
	SRCCE_client_create_log(recv_msg , "CLIENT"); 
	free(recv_msg);

	SRCCE_client_send_message(socket_client,"\ni got your message\n");
	
	do
	{
		/* Receive username message */
		recv_msg = (char *)malloc(sizeof(char) * MSG);
		if( NULL == recv_msg)
		{
			printf("\n Failed to allocate the memory to recv_msg \n");
			SRCCE_client_create_log("Failed to allocate the memory to recv_msg" , "admin"); 
			exit(EXIT_FAILURE);
		}
		SRCCE_client_receive_message(socket_client,recv_msg);
		printf(" %s",recv_msg);
		fflush(stdout);
		SRCCE_client_create_log(recv_msg , "CLIENT"); 
		free(recv_msg);
	
		while( INVALID_LEN == length_flag)
		{
			
			username = (char *)malloc(sizeof(char)* MSG);
			if( NULL == username)
			{
				printf("\n Failed to allocate the memory to username \n");
				SRCCE_client_create_log("Failed to allocate the memory to username" , "CLIENT"); 
				exit(EXIT_FAILURE);
			}
			
			/* sending username to server */
			scanf(" %[^\n]%*c",username);
			length_flag = SRCCE_client_validate_string_length(username, USERNAME_LEN);
			
			if(VALID_LEN == length_flag)
			{
				SRCCE_client_send_message(socket_client,username);
			}
			else
			{
				printf("\nMaximum characters allowed are 25.\nPlease re-enter username:");
				fflush(stdout);
				SRCCE_client_create_log("Maximum characters allowed are 25.Please re-enter username :" , "CLIENT"); 
				free(username);
			}
		}
				
		length_flag = INVALID_LEN;
		
		/* sending password to server */
		while( INVALID_LEN == length_flag)
		{
			password = SRCCE_client_password_mask();
			
			length_flag = SRCCE_client_validate_string_length(password, PASSWORD_LENGTH);
			if(VALID_LEN == length_flag)
			{
				/* send the password */
				SRCCE_client_send_message(socket_client, password);
				SRCCE_client_create_log("Valid Password length..." , "CLIENT");
				free(password);
				sleep(1);
			}
			else
			{
				printf("\nInvalid password max length is 25 characters...");
				fflush(stdout);
				SRCCE_client_create_log("Invalid password max length is 25 characters..." , "CLIENT");
				free(password);
				sleep(2);
			}
			
		}
		
		length_flag = INVALID_LEN;
		
		/* recieve the user_type */
		recv_msg = (char *)malloc(sizeof(char) * MSG);
		if( NULL == recv_msg)
		{
			printf("\n Failed to allocate the memory to recv_msg \n");
			SRCCE_client_create_log("Failed to allocate the memory to recv_msg" , "admin"); 
			exit(EXIT_FAILURE);
		}

		SRCCE_client_receive_message(socket_client,recv_msg);
		user_type = atoi(recv_msg);
		free(recv_msg);
		
		printf("\nuser type is %d",user_type);
		fflush(stdout);
		snprintf(log_msg, MSG, "user type is %d",user_type);
		SRCCE_client_create_log( log_msg, "CLIENT"); 

		sleep(1);
		
		if( INVALID_USER == user_type)
		{
			printf("\n Invalid details \n Please re-enter login details \n");
			SRCCE_client_create_log("Invalid details \n Please re-enter login details", "CLIENT"); 
			fflush(stdout);
			free(username);
			sleep(2);
		}
		if( IS_ADMIN == user_type)
		{
			printf("\ncalling admin process...\n");
			SRCCE_client_create_log("calling admin process...", "admin");
			fflush(stdout);
			free(username);
			sleep(2);
			break;
		}
		if( LOGGED_IN == user_type)
		{
			printf("\n%s is already logged in...\n",username);
			fflush(stdout);
			snprintf(log_msg, MSG, "%s is already logged in...",username);
			SRCCE_client_create_log( log_msg, username);
			free(username);
			sleep(2);
		}
		if(USER_LOGGED_IN == user_type)
		{
			printf("\nUsers are currently working...\nplease come after sometime...");
			fflush(stdout);
			SRCCE_client_create_log("Users are currently working...please come after sometime...", "CLIENT");
			free(username);
			sleep(2);
		}
		if(ADMIN_LOGGED_IN == user_type)
		{
			printf("\nadmin is working...\nPlease try later...");
			fflush(stdout);
			SRCCE_client_create_log("admin is working...Please try later...", "CLIENT");
			free(username);
			sleep(2);
		}
		if( IS_USER == user_type )
		{
			printf("\ncalling user process...\n");
			fflush(stdout);
			SRCCE_client_create_log("calling user process...", "CLIENT");
			sleep(2);
			break;
		}
		system("clear");
		
	}while(TRUE);
	
	/* admin process */
	if(IS_ADMIN == user_type)
	{	
		/* calling admin process */
		SRCCE_client_admin_process(socket_client);
	}
	if(IS_USER == user_type)
	{
		/* calling user process */
		SRCCE_client_user_process(socket_client, username);
	}	
	return EXIT_SUCCESS;	
}		





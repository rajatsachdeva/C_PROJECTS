/**************************************************************************************
*
*  FILE NAME	: SRCCE_client_add_remove_user.c
*
*  DESCRIPTION: This file contains the functions for admin's add remove user process.
*
*************************************************************************************/

#include "SRCCE_client_header.h"

/***************************************************************************
 * FUNCTION NAME: SRCCE_client_display_user
 *
 * DESCRIPTION: Function to receive the user credentials to the admin.
 * 	
 * ARGUMENTS: socket: socket descriptor.
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_client_display_user(int socket)
{
	char *recv_msg = NULL;
	
	/* iterator variable */
	int iterator = ZERO;
	
	int total_users = ZERO;
	
	printf("\nentering display users...\n");
	SRCCE_client_create_log("entering display users...", "admin"); 

	fflush(stdout);
	sleep(1);
	
	/* receive the total number of users */
	recv_msg = (char *)malloc(sizeof(char) * MSG);
	/* check for error while allocating memory */
	if( NULL == recv_msg)
	{
		printf("\n Failed to allocate the memory to recv_msg \n");
		SRCCE_client_create_log("Failed to allocate the memory to recv_msg" , "admin"); 
		exit(EXIT_FAILURE);
	} 
	SRCCE_client_receive_message(socket,recv_msg);
	total_users = atoi(recv_msg);
	free(recv_msg);
	
	printf("\n\tusername\t\tpassword\t\tlogin_status\n");
	fflush(stdout);
	SRCCE_client_create_log("\tusername\t\tpassword\t\tlogin_status" , "admin"); 
	if(ZERO == total_users)
	{
		printf("\nThere 0 Users in the Database...\n");
		fflush(stdout);
		SRCCE_client_create_log("There 0 Users in the Database..." , "admin"); 
		sleep(2);
	}
	else
	{
		/* display the users details to the admin */
		for( iterator = ZERO; iterator < total_users; iterator++)
		{
			/* receive user details and display it  */
			recv_msg = (char *)malloc(sizeof(char) * MSG);
			/* check for error while allocating memory */
			if( NULL == recv_msg)
			{
				printf("\n Failed to allocate the memory to recv_msg \n");
				SRCCE_client_create_log("Failed to allocate the memory to recv_msg" , "admin"); 
				exit(EXIT_FAILURE);
			} 

			SRCCE_client_receive_message(socket,recv_msg);
			printf("%s\n",recv_msg );
			fflush(stdout);
			SRCCE_client_create_log(recv_msg, "admin"); 
			free(recv_msg);
		}	
	}
	
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_client_add_user
 *
 * DESCRIPTION: Admin can add a user. Admin enters the user's name and password
 * for the user. These are then sent to the server and receives the acknowledgement.
 * 	
 * ARGUMENTS: socket: socket descriptor.
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_client_add_user(int socket)
{	
	char *recv_msg = NULL;
	char *username = NULL;
	char *password = NULL;
	int length_flag = INVALID_LEN; 
	
	printf("\nentering add user...\n");
	SRCCE_client_create_log("entering add user...", "admin");
	fflush(stdout);
	sleep(1);
	
	/* check the length of the user name */
	while( INVALID_LEN == length_flag)
	{
		printf("\nEnter the username : ");
		SRCCE_client_create_log("Enter the username : ", "admin");
		fflush(stdout);
		username = (char *)malloc(sizeof(char) * MSG);
		/* check for error while allocating memory */
		if( NULL == username)
		{
			printf("\n Failed to allocate the memory to username \n");
			SRCCE_client_create_log("Failed to allocate the memory to username" , "admin"); 
			exit(EXIT_FAILURE);
		} 
		/* entering the username to be added */
		scanf(" %[^\n]" , username);
		
		length_flag = SRCCE_client_validate_string_length(username, USERNAME_LEN);

		if( INVALID_LEN == length_flag)
		{
			printf("\nInvalid length for username...\nMaximum allowed characters is 25.");
			fflush(stdout);
			SRCCE_client_create_log("Invalid length for username...Maximum allowed characters is 25." , "admin"); 
			free(username);
			sleep(1);
		}
		else
		{
			SRCCE_client_send_message(socket,username);
			free(username);
		}
	}
		
	length_flag = INVALID_LEN;
	
	/* check the length of the password */
	while(INVALID_LEN == length_flag)
	{
		/* function to "*" when password is entered*/
		password = SRCCE_client_password_mask();
		
		length_flag = SRCCE_client_validate_string_length(password, PASSWORD_LENGTH);

		/* if password length is greater than 25 */
		if( INVALID_LEN == length_flag)
		{
			printf("\nInvalid length for password...\nMaximum allowed characters is 25.");
			fflush(stdout);
			SRCCE_client_create_log("Invalid length for password...Maximum allowed characters is 25." , "admin"); 
			free(password);
			sleep(1);
		}
		else
		{
			SRCCE_client_send_message(socket, password);
			free(password);
		}
		
	}
		
	/* Receive the acknowledgement from server */
	recv_msg = (char *)malloc(sizeof(char) * MSG);
	
	/* check for error while allocating memory */
	if( NULL == recv_msg)
	{
		printf("\n Failed to allocate the memory to recv_msg \n");
		SRCCE_client_create_log("Failed to allocate the memory to recv_msg" , "admin"); 
		exit(EXIT_FAILURE);
	} 

	SRCCE_client_receive_message(socket,recv_msg);
	printf(" %s ",recv_msg);
	fflush(stdout);
	SRCCE_client_create_log(recv_msg , "admin"); 
	free(recv_msg);
	sleep(2);
}

/***************************************************************************
 * FUNCTION NAME: SRCCE_client_remove_user
 *
 * DESCRIPTION: Admin enters the user's name to remove and receives an 
 * acknowledgement from server.
 * 	
 * ARGUMENTS: socket: socket descriptor.
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_client_remove_user(int socket)
{
	char *remove_user = NULL;
	char *recv_msg = NULL;
	int length_flag = INVALID_LEN;
	
	printf("\nEntering remove user...\n");
	fflush(stdout);
	SRCCE_client_create_log("Entering remove user..." , "admin"); 
	sleep(1);
	
	/* check the length of the user name */
	while( INVALID_LEN == length_flag)
	{
		printf("\nEnter the username that you want to be removed : ");
		fflush(stdout);
		SRCCE_client_create_log("Enter the username that you want to be removed :" , "admin"); 

		remove_user = (char *)malloc(sizeof(char) * MSG);
		
		/* check for error while allocating memory */
		if( NULL == remove_user)
		{
			printf("\n Failed to allocate the memory to remove_user \n");
			SRCCE_client_create_log("Failed to allocate the memory to remove_user" , "admin"); 
			exit(EXIT_FAILURE);
		} 
		
		/* entering the username to be removed */
		scanf(" %[^\n]" , remove_user);
		
		length_flag = SRCCE_client_validate_string_length(remove_user, USERNAME_LEN);
		
		/*if length of username to be removed is greater than 25*/
		if( INVALID_LEN == length_flag)
		{
			printf("\nInvalid length for username...\nMaximum allowed characters is 25.");
			fflush(stdout);
			SRCCE_client_create_log("Invalid length for username...Maximum allowed characters is 25." , "admin"); 
			free(remove_user);
			sleep(1);
		}
		else
		{
			SRCCE_client_send_message(socket,remove_user);
			free(remove_user);
		}
	}
	
	/* Receive the acknowledgement from server */
	recv_msg = (char *)malloc(sizeof(char) * MSG);
	
	/* check for error while allocating memory */
	if( NULL == recv_msg)
	{
		printf("\n Failed to allocate the memory to recv_msg \n");
		SRCCE_client_create_log("Failed to allocate the memory to recv_msg" , "admin"); 			
		exit(EXIT_FAILURE);
	} 

	SRCCE_client_receive_message(socket,recv_msg);
	printf(" %s ",recv_msg);
	fflush(stdout);
	SRCCE_client_create_log( recv_msg , "admin"); 
	free(recv_msg);
	sleep(2);
}

/***************************************************************************
 * FUNCTION NAME: SRCCE_client_add_remove_user
 *
 * DESCRIPTION: Admin is given with choices to add or remove the user and to
 * exit from this menu.
 *
 * ARGUMENTS: socket: socket descriptor.
 * 	
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_client_add_remove_user(int socket)
{
	/* 1 for add user 2 for remove user */
	char choice[CHOICE_SIZE] = {ZERO};
	
	/* variable to store the choice valid status */
	int valid = INVALID_CHOICE;
	
	printf("\nEntering add/remove user...\n");
	fflush(stdout);
	sleep(1);
	
	do
	{
		/* display choices */
		printf("\n----------------ADD/REMOVE USER--------------------\n");
		printf("\n	1. ADD USER");
		printf("\n	2. REMOVE USER");
		printf("\n	3. DISPLAY USERS");
		printf("\n	4. EXIT");
		printf("\n	5. CLEAR");
		printf("\n enter choice :");
		fflush(stdout);
		SRCCE_client_create_log( "----------------ADD/REMOVE USER--------------------\n" , "admin");
		SRCCE_client_create_log( "	1. ADD USER", "admin");
		SRCCE_client_create_log( "	2. REMOVE USER" , "admin");
		SRCCE_client_create_log("	3. DISPLAY USERS" , "admin");
		SRCCE_client_create_log( "	4. EXIT" , "admin");
		SRCCE_client_create_log(" enter choice :" , "admin");
		
		/* take choice from admin */
		scanf(" %[^\n]",choice);
	
		/* validate the choice */
		valid = SRCCE_client_validate_choice(choice);
		
		/* if invalid choice */
		if( INVALID_CHOICE == valid || CHAR_CHOICE == valid )
		{
			printf("\n Invalid choice \n Please re-enter \n");
			SRCCE_client_create_log("Invalid choice...Please re-enter" , "admin");
			sleep(1);
		}
		else/* if valid choice */
		{
			if( FIVE != atoi(choice) )
			{
				/* send choice to server */
				SRCCE_client_send_message(socket,choice);
			}
			
			switch(atoi(choice))
			{
				/* if choice is 1 call add_user function */
				case ONE: SRCCE_client_add_user(socket);
						break;
						
				/* if choice is 2 call remove_user function */
				case TWO: SRCCE_client_remove_user(socket);
						break;
				
				/* if choice is 3 call display_user function */
				case THREE: SRCCE_client_display_user(socket);
						break;
						
				/* if choice is 4 call EXIT */		
				case FOUR: printf("\nExiting from add/remove user...\n");
						fflush(stdout);
						SRCCE_client_create_log(" Exiting from add/remove user..." , "admin");
						sleep(1);
						break;
						
				case FIVE: system("clear");
						break;
						
				default: printf("\nInvalid choice... \n");
						fflush(stdout);
						SRCCE_client_create_log("Invalid choice..." , "admin");
						sleep(1);
			}
		}
	
	}while(FOUR != atoi(choice));
}
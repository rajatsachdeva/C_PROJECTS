/**************************************************************************************
*
*  FILE NAME	: SRCCE_client_add_remove_testcase.c
*
*  DESCRIPTION: This file contains the functions for admin's add remove testcase process.
*
*************************************************************************************/

#include "SRCCE_client_header.h"

/***************************************************************************
 * FUNCTION NAME: SRCCE_client_update_testcase
 *
 * DESCRIPTION: Admin can update the testcases of selected question.
 * 	
 * ARGUMENTS: socket: socket descriptor.
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_client_update_testcase(int socket)
{
	/* string variable to store the question name */
	char question_name[MSG];
	
	/* pointer variable to store the message received from server */
	char *recv_msg = NULL;
	
	/* flag if question exists or not */
	int flag = INVALID_QUESTION;
	
	/* string to store the admin's choice */
	char choice[CHOICE_SIZE] = "n";
	
	/* filesize flag to check if file size is greater than 64KB or not */
	int filesize_flag = INVALID_FILESIZE;
	
	/* variable to store the if choice is valid or not */
	int valid_choice = INVALID_CHOICE;
	
	/* flag vairable to check the size of the question name */
	int length_flag = INVALID_LEN;
	
	printf("\nEntering add testcase...\n");
	fflush(stdout);
	SRCCE_client_create_log("Entering add testcase...", "admin"); 
	sleep(1);
	
	/* receive question name request from server */
	recv_msg = (char *)malloc(sizeof(char) * MSG);
	
	/* check for error while allocating memory */
	if( NULL == recv_msg)
	{
		printf("\n Failed to allocate the memory to recv_msg \n");
		SRCCE_client_create_log("Failed to allocate the memory to recv_msg" , "admin"); 
		exit(EXIT_FAILURE);
    } 
	SRCCE_client_receive_message(socket,recv_msg);
	printf("%s", recv_msg);
	fflush(stdout);
	SRCCE_client_create_log(recv_msg, "admin"); 
	free(recv_msg);
	
	while( INVALID_LEN == length_flag)
	{
		/* take Question name from user */
		scanf(" %[^\n]%*c",question_name);
		
		/* check the size of question name */
		length_flag = SRCCE_client_validate_string_length(question_name, Q_NAME);
		
		if(INVALID_LEN == length_flag)
		{
			printf("\nInvalid Question name\n\nMaximum allowd characters are 8\nEnter Question name: ");
			fflush(stdout);
			SRCCE_client_create_log("Invalid Question name...Maximum allowd characters are 50...Enter Question name:" , "admin");
			sleep(1);
		}
	}
	/* send the question name to server*/
	SRCCE_client_send_message(socket, question_name);
	
	/* receive flag from admin that does the question name exists or not*/
	recv_msg = (char *)malloc(sizeof(char) * MSG);
	/* check for error while allocating memory */
	if( NULL == recv_msg)
	{
		printf("\n Failed to allocate the memory to recv_msg \n");
		SRCCE_client_create_log("Failed to allocate the memory to recv_msg" , "admin"); 
		exit(EXIT_FAILURE);
    } 

	SRCCE_client_receive_message(socket,recv_msg);
	flag = atoi(recv_msg);
	free(recv_msg);
	
	if(INVALID_QUESTION == flag) /* Invalid Question name */
	{
		/* receive acknowledgement */
		recv_msg = (char *)malloc(sizeof(char) * MSG);
		/* check for error while allocating memory */
		if( NULL == recv_msg)
		{
			printf("\n Failed to allocate the memory to recv_msg \n");
			SRCCE_client_create_log("Failed to allocate the memory to recv_msg" , "admin"); 
			exit(EXIT_FAILURE);
       	} 

		SRCCE_client_receive_message(socket,recv_msg);
		printf("%s", recv_msg);
		fflush(stdout);
		SRCCE_client_create_log(recv_msg, "admin");
		free(recv_msg);	
		sleep(2);
		
	}  /* if ends*/
	else /* valid Question */
	{
		/* receive acknowledgement */
		recv_msg = (char *)malloc(sizeof(char) * MSG);
		/* check for error while allocating memory */
		if( NULL == recv_msg)
		{
			printf("\n Failed to allocate the memory to recv_msg \n");
			SRCCE_client_create_log("Failed to allocate the memory to recv_msg" , "admin"); 
			exit(EXIT_FAILURE);
       	} 

		SRCCE_client_receive_message(socket,recv_msg);
		printf("%s", recv_msg);
		fflush(stdout);
		SRCCE_client_create_log(recv_msg, "admin");

		free(recv_msg);
		
		/* receive the test case file from server */
		SRCCE_client_receive_file(socket, "testcase");
		
		/* edit the test case file */
		while( INVALID_FILESIZE == filesize_flag)
		{
			strcpy(choice, "n");
			filesize_flag = INVALID_FILESIZE;
			
			printf("\nopening the test case file...");
			fflush(stdout);
			SRCCE_client_create_log("opening the test case file...", "admin");

			sleep(2);
			
			while(ZERO == strcmp(choice,"n") || ZERO == strcmp(choice,"N"))
			{
				/* open the test case file for user */
				system("vi testcase");
								
				valid_choice = INVALID_CHOICE;
				
				while( CHAR_CHOICE != valid_choice )
				{
					printf("\nAre you done editing ?(y/n): ");
					fflush(stdout);
					SRCCE_client_create_log("Are you done editing ?(y/n): ", "admin");

					/* take the choice from admin */
					scanf(" %[^\n]",choice);
					
					/* validate the choice */
					valid_choice = SRCCE_client_validate_choice(choice);
					
					/* check if the choice is vaild or not */
					if(INVALID_CHOICE == valid_choice || DIGIT_CHOICE == valid_choice)
					{
						printf("\nInvalid Choice\nPlease Re-enter...");
						fflush(stdout);
						SRCCE_client_create_log("Invalid Choice...Please Re-enter...", "admin");

						sleep(1);
					}
				}
			}
			
			/* get the file size */
			filesize_flag = SRCCE_client_check_filesize("./testcase");
			
			/* if size of testcase file is vaild */
			if( VALID_FILESIZE == filesize_flag)
			{
				/* Send test case file */

				SRCCE_client_send_file(socket, "testcase");
				system("rm testcase");
			}
			else
			{
				printf("\nMaximum file size is 64KB.\nPlease check the size of testcase file\n");
				SRCCE_client_create_log("Maximum file size is 64KB...Please check the size of testcase file", "admin");

				fflush(stdout);
				sleep(2);
			}
		}/* end of while loop for testcase file size */
		
		/* receive the Expected_output file from server */
		SRCCE_client_receive_file(socket, "Expected_output");
		
		strcpy(choice, "n");
		filesize_flag = INVALID_FILESIZE;
		
		/* edit the expected output file */
		while(INVALID_FILESIZE == filesize_flag)
		{
			strcpy(choice, "n");
			filesize_flag = INVALID_FILESIZE;
			
			printf("\nopening the Expected output file...");
			fflush(stdout);
			SRCCE_client_create_log("opening the Expected output file...", "admin");

			sleep(2);
						
			while(ZERO == strcmp(choice,"n") || ZERO == strcmp(choice,"N"))
			{	
				/* open the Expected_output file for user */
				system("vi Expected_output");
				valid_choice = INVALID_CHOICE;
				
				while( CHAR_CHOICE != valid_choice )
				{
					printf("\nAre you done editing ?(y/n): ");
					fflush(stdout);
					SRCCE_client_create_log("Are you done editing ?(y/n):", "admin");

					
					/* take the choice from admin */
					scanf(" %[^\n]",choice);
					
					/* validate the choice */
					valid_choice = SRCCE_client_validate_choice(choice);
					
					/* check if the choice is vaild or not */
					if(INVALID_CHOICE == valid_choice || DIGIT_CHOICE == valid_choice)
					{
						printf("\nInvalid Choice\nPlease Re-enter...");
						fflush(stdout);
						SRCCE_client_create_log("Invalid Choice\nPlease Re-enter...", "admin");

						sleep(1);
					}
				}
			}
			
			/* get the file size for expected output file */
			filesize_flag = SRCCE_client_check_filesize("./Expected_output");
			
			/* if the file size is the valid file size then send the file */
			if( VALID_FILESIZE == filesize_flag)
			{
				/* Send Expected_output file */
				SRCCE_client_send_file(socket, "Expected_output");
				system("rm Expected_output");	
			}
			else
			{
				printf("\nMaximum file size is 64KB.\nPlease limit the size of Expected_output file\n");
				fflush(stdout);
				SRCCE_client_create_log("Maximum file size is 64KB...Please limit the size of Expected_output file", "admin");

				sleep(2);
			}
		}/* end of while check file size for expected output file */
		
		printf("\n Test Cases updated Successfully...");
		SRCCE_client_create_log("Test Cases updated Successfully...", "admin");
		fflush(stdout);
		sleep(1);
	}
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_client_add_remove_testcase
 *
 * DESCRIPTION: Admin is shown with choices to update the testcases of a 
 * selected question. 
 * 	
 * ARGUMENTS: socket: socket descriptor.
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_client_add_remove_testcase(int socket)
{
	/* 1 for update testcase 2 for exit */
	char choice[CHOICE_SIZE] = {ZERO};
	
	/* variable to store if choice is valid or not */
	int valid = INVALID_CHOICE;
	
	do
	{
		/* clear the screen */
		system("clear");
		
		/* display choices */
		printf("\n----------------ADD/REMOVE testcase--------------------\n");
		printf("\n	1. Update testcase");
		printf("\n	2.	EXIT");
		printf("\n enter choice :");
		fflush(stdout);
		SRCCE_client_create_log("----------------ADD/REMOVE testcase--------------------\n", "admin");
		SRCCE_client_create_log("	1. Update testcase", "admin");
		SRCCE_client_create_log("	2.	EXIT", "admin");
		SRCCE_client_create_log(" enter choice :", "admin");

		/* take choice from admin */
		scanf(" %[^\n]",choice);
		
		/* validate the choice */
		valid = SRCCE_client_validate_choice(choice);
		
		/* check for error */
		if( INVALID_CHOICE == valid || CHAR_CHOICE == valid )
		{
			printf("\n Invalid choice \n Please re-enter \n");
			sleep(1);
		}
		else /* if a valid choice */
		{
			/* send choice to server */
			SRCCE_client_send_message(socket,choice);
		
			/* Select the choice */
			switch(atoi(choice))
			{
				case ONE: SRCCE_client_update_testcase(socket);
						break;
			
				case TWO: printf("\nExiting from add/remove testcase...\n");
						fflush(stdout);
						SRCCE_client_create_log("Exiting from add/remove testcase...", "admin");
						sleep(2);
						break;
					
				default: printf("\n Invalid choice...\n");
						fflush(stdout);
						SRCCE_client_create_log("Invalid choice...", "admin");
						sleep(1);
			}
		}	
		
	}while(TWO != atoi(choice));
}
/**************************************************************************************
*
*  FILE NAME	: SRCCE_client_add_remove_question.c
*
*  DESCRIPTION: This file contains the functions for admin's add remove question.
*
*************************************************************************************/

#include "SRCCE_client_header.h"

/***************************************************************************
 * FUNCTION NAME: SRCCE_client_add_question
 *
 * DESCRIPTION: Allows the admin to create a question for the user. Admin adds
 * the question's name and description and after their validation admin is 
 * allowed to create the main.c file, testcases and expected output file and all
 * these files are then sent to the server where they are saved in the question 
 * bank.
 * 	
 * ARGUMENTS: socket: socket descriptor.
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_client_add_question(int socket)
{
	/* Pointer to store the received message from server */
	char *recv_msg = NULL;
	char log_msg[MSG] = {ZERO};

	/* question name string */
	char question_name[MSG] = {ZERO};
	
	/* question description string */
	char question_description[MSG] = {ZERO};
	
	/* string to store the admin's choice */
	char choice[CHOICE_SIZE]  = "n";
	
	/* flag if question exists or not */
	int flag = INVALID_QUESTION;
	
	/* variable to store if the choice entered is valid or not */
	int valid_choice = INVALID_CHOICE;
	
	/* flags to check if the file size < 64KB */
	int filesize_flag_1 = INVALID_FILESIZE;
	int filesize_flag_2 = INVALID_FILESIZE;
	int filesize_flag_3 = INVALID_FILESIZE;
	
	/* acknowledgement flag if file received successfully */
	int ack_file = RECEIVE_FAIL;
	
	/* flag to check size of the question name and description */
	int length_flag = INVALID_LEN;
	
	printf("\nEntering add question...\n");
	fflush(stdout);
	SRCCE_client_create_log( "Entering add question...", "admin"); 
	
	sleep(1);
	
	/* receive question name request from server */
	recv_msg = (char *)malloc(sizeof(char) * MSG);
	if( NULL == recv_msg)
    {
      	printf("\n Failed to allocate the memory to recv_msg \n");
        SRCCE_client_create_log("Failed to allocate the memory to recv_msg" , "admin"); 
        exit(EXIT_FAILURE);
    } 
	SRCCE_client_receive_message(socket,recv_msg);
	printf("%s", recv_msg);
	fflush(stdout);
	SRCCE_client_create_log( recv_msg, "admin"); 

	free(recv_msg);
	
	while(INVALID_LEN == length_flag )
	{
		/* take Question name from user */
		scanf(" %[^\n]%*c",question_name);
	
		/* check the size of question name */
		length_flag = SRCCE_client_validate_string_length(question_name, Q_NAME);
		
		if(INVALID_LEN == length_flag)
		{
			printf("\nInvalid Question name\n\nMaximum allowd characters are 8\nEnter Question name: ");
			fflush(stdout);
			SRCCE_client_create_log("Invalid Question name...Maximum allowd characters are 8...Enter Question name:" , "admin"); 
			sleep(1);
		}
	}

	/* send the question name to server*/
	SRCCE_client_send_message(socket, question_name);
	
	length_flag = INVALID_LEN;
	
	/* receive question description request from server */
	recv_msg = (char *)malloc(sizeof(char) * MSG);
	if( NULL == recv_msg)
    {
     	printf("\n Failed to allocate the memory to recv_msg \n");
        SRCCE_client_create_log("Failed to allocate the memory to recv_msg" , "admin"); 
        exit(EXIT_FAILURE);
    } 

	SRCCE_client_receive_message(socket,recv_msg);
	printf("%s", recv_msg);
	fflush(stdout);
	SRCCE_client_create_log( recv_msg, "admin"); 
	free(recv_msg);
	
	while(INVALID_LEN == length_flag)
	{
		/* take the question description from the user */
		scanf(" %[^\n]%*c",question_description);
				
		/* check the size of question description */
		length_flag = SRCCE_client_validate_string_length(question_description, Q_DESCRIPTION);

		if(INVALID_LEN == length_flag)
		{
			printf("\nInvalid Question Description...\n\nMaximum allowd characters are 125\nEnter Question description: ");
			fflush(stdout);
			SRCCE_client_create_log("Invalid Question Description...Maximum allowd characters are 125...Enter Question description:" , "admin"); 
			sleep(1);
		}
	
	}
	
	/* send the question description to server*/
	SRCCE_client_send_message(socket, question_description);

	/* receive flag from admin that does the question name exists or not*/
	recv_msg = (char *)malloc(sizeof(char) * MSG);
	if( NULL == recv_msg)
    {
		printf("\n Failed to allocate the memory to recv_msg \n");
        SRCCE_client_create_log("Failed to allocate the memory to recv_msg" , "admin"); 
        exit(EXIT_FAILURE);
    } 
	SRCCE_client_receive_message(socket,recv_msg);
	flag = atoi(recv_msg);
	free(recv_msg);
	
	/* if question name exists */
	if( VALID_QUESTION == flag)
	{
		/* receive acknowledgement */
		recv_msg = (char *)malloc(sizeof(char) * MSG);
		if( NULL == recv_msg)
        {
       		printf("\n Failed to allocate the memory to recv_msg \n");
           	SRCCE_client_create_log("Failed to allocate the memory to recv_msg" , "admin"); 
          	exit(EXIT_FAILURE);
       	} 
		SRCCE_client_receive_message(socket,recv_msg);
		printf("%s", recv_msg);
		fflush(stdout);
		SRCCE_client_create_log( recv_msg, "admin"); 
		sleep(2);
		free(recv_msg);
	}
	else /* if question name does not exists */
	{
		/* check if the  main.c file size is valid or not */
		while(INVALID_FILESIZE == filesize_flag_1)
		{
			strcpy(choice, "n");
			filesize_flag_1 = INVALID_FILESIZE;
			
			printf("\nopening the main.c file\n");
			SRCCE_client_create_log("opening the main.c file" , "admin"); 
			sleep(2);
			
			/* edit the main.c file */
			while(ZERO == strcmp(choice,"n") || ZERO == strcmp(choice,"N"))
			{
				/* open the file to the admin */
				system("vi main.c");
			
				valid_choice = INVALID_CHOICE;
				
				while( CHAR_CHOICE != valid_choice )
				{
					printf("\nAre you done editing ?(y/n): ");
					fflush(stdout);
					SRCCE_client_create_log("Are you done editing ?(y/n):" , "admin"); 
								
					/* take the choice from admin */
					scanf(" %[^\n]",choice);
					
					/* validate the choice */
					valid_choice = SRCCE_client_validate_choice(choice);
					
					/* check if the choice is vaild or not */
					if(INVALID_CHOICE == valid_choice || DIGIT_CHOICE == valid_choice)
					{
						printf("\nInvalid Choice\nPlease Re-enter...");
						fflush(stdout);
						SRCCE_client_create_log("Invalid Choice...Please Re-enter..." , "admin"); 
						sleep(1);
					}
				}
				
			}/* end of while for editing main.c */
			
			/* check the size of the file main.c */
			filesize_flag_1 = SRCCE_client_check_filesize("./main.c");
			printf("\nmain flag is :%d", filesize_flag_1);
			fflush(stdout);
			snprintf(log_msg, MSG, "main flag is :%d", filesize_flag_1);
			SRCCE_client_create_log( log_msg, "admin"); 
			sleep(2);
			
			/* if a valid file size */
			if( VALID_FILESIZE == filesize_flag_1)
			{
				/* send the file */
				SRCCE_client_send_file(socket, "./main.c");
		
				/* receive the acknowledgement for successfull file sent */
				recv_msg = (char *)malloc(sizeof(char) * MSG);
				if( NULL == recv_msg)
                {
                   	printf("\n Failed to allocate the memory to recv_msg \n");
                    SRCCE_client_create_log("Failed to allocate the memory to recv_msg" , "admin"); 
                    exit(EXIT_FAILURE);
                } 
				SRCCE_client_receive_message(socket,recv_msg);
				ack_file = atoi(recv_msg);
				free(recv_msg);
		
				/* check ack_file flag */
				if( RECEIVE_SUCCESS == ack_file)
				{
					/* send the test case file */
					printf("\nmain.c file sent successfully\n");
					SRCCE_client_create_log("main.c file sent successfully" , "admin"); 

					sleep(2);
		
					strcpy(choice, "n");
					filesize_flag_2 = INVALID_FILESIZE;
					
					/* check if the testcase file size is valid */
					while( INVALID_FILESIZE == filesize_flag_2)
					{
						strcpy(choice, "n");
						filesize_flag_2 = INVALID_FILESIZE;
					
						/* open the test case file */
						printf("\nNow opening test case file...\n");
						SRCCE_client_create_log("Now opening test case file..." , "admin"); 
						sleep(1);
		
						/* send the testcase file */
						while(ZERO == strcmp(choice,"n") || ZERO == strcmp(choice,"N"))
						{
							/* open the file to the admin */
							system("vi testcase");
			
							valid_choice = INVALID_CHOICE;
							
							while( CHAR_CHOICE != valid_choice )
							{		
								printf("\nAre you done editing ?(y/n): ");
								fflush(stdout);
								SRCCE_client_create_log("Are you done editing ?(y/n):" , "admin"); 
								/* take the choice from admin */
								scanf(" %[^\n]",choice);
								
								/* validate the choice */
								valid_choice = SRCCE_client_validate_choice(choice);
					
								/* check if the choice is vaild or not */
								if(INVALID_CHOICE == valid_choice || DIGIT_CHOICE == valid_choice)
								{
									printf("\nInvalid Choice\nPlease Re-enter...");
									fflush(stdout);
									SRCCE_client_create_log("Invalid Choice\nPlease Re-enter..." , "admin");
									sleep(1);
								}
							}
						}
						
						/* check if the size of testcase file is valid */
						filesize_flag_2 = SRCCE_client_check_filesize("./testcase");
						
						/* if the size of the file testcase is valid */
						if( VALID_FILESIZE == filesize_flag_2)
						{
							/* send the file */
							SRCCE_client_send_file(socket, "./testcase");
			
							/* receive the acknowledgement for successfull file sent */
							recv_msg = (char *)malloc(sizeof(char) * MSG);
							if( NULL == recv_msg)
                           	{
                           		printf("\n Failed to allocate the memory to recv_msg \n");
                                SRCCE_client_create_log("Failed to allocate the memory to recv_msg" , "admin"); 
                                exit(EXIT_FAILURE);
                            } 

							SRCCE_client_receive_message(socket,recv_msg);
							ack_file = atoi(recv_msg);
							free(recv_msg);
			
							/* check ack_file flag */
							if( RECEIVE_SUCCESS == ack_file )
							{
								printf("\ntestcase file sent successfully\n Now opening Expected output file\n");
								SRCCE_client_create_log("testcase file sent successfully\n Now opening Expected output file" , "admin"); 
								sleep(2);
				
								strcpy(choice, "n");
								filesize_flag_3 = INVALID_FILESIZE;
								
								/* check if the size of file Expected_output is valid */
								while( INVALID_FILESIZE == filesize_flag_3)
								{
									strcpy(choice, "n");
									filesize_flag_3 = INVALID_FILESIZE;
								
									/* send the expected output file */
									while(ZERO == strcmp(choice,"n") || ZERO == strcmp(choice,"N"))
									{
										/* open the file to the user */
										system("vi Expected_output");
			
										valid_choice = INVALID_CHOICE;
										
										while( CHAR_CHOICE != valid_choice )
										{
											printf("\nAre you done editing ?(y/n): ");
											fflush(stdout);
											SRCCE_client_create_log("Are you done editing ?(y/n):" , "admin"); 
											
											/* take the choice from admin */
											scanf(" %[^\n]",choice);
											
											/* validate the choice */
											valid_choice = SRCCE_client_validate_choice(choice);
					
											/* check if the choice is vaild or not */
											if(INVALID_CHOICE == valid_choice || DIGIT_CHOICE == valid_choice)
											{
												printf("\nInvalid Choice\nPlease Re-enter...");
												SRCCE_client_create_log("Invalid Choice\nPlease Re-enter..." , "admin"); 

												fflush(stdout);
												sleep(1);
											}
										}	
										
									}
								
									/* check the size of the expected output file */
									filesize_flag_3 = SRCCE_client_check_filesize("./Expected_output");
								
									/* if size of file Expected output is valid */
									if(VALID_FILESIZE == filesize_flag_3)
									{
										/* send the file */
										SRCCE_client_send_file(socket, "./Expected_output");
				
										/* receive acknowledgement */
										recv_msg = (char *)malloc(sizeof(char) * MSG);
										if( NULL == recv_msg)
                                        {
                                        	printf("\n Failed to allocate the memory to recv_msg \n");
                                        	SRCCE_client_create_log("Failed to allocate the memory to recv_msg" , "admin"); 
                                        	exit(EXIT_FAILURE);
                                        } 

										SRCCE_client_receive_message(socket,recv_msg);
										ack_file = atoi(recv_msg);
										free(recv_msg);
										if( RECEIVE_SUCCESS == ack_file)
										{
											printf("Question Added successfully...\n");
											fflush(stdout);
											SRCCE_client_create_log("Question Added successfully..." , "admin");
											sleep(2);
										}/* End of if for ack Expected_output */ 
										else
										{
											printf("Question NOT Added successfully...\n");
											fflush(stdout);
											SRCCE_client_create_log("Question NOT Added successfully..." , "admin");
											sleep(2);
										}
									}
									else /* if Expected output file is invalid */
									{
										printf("\n File Size is greater than 64KB\n Please limit the size of the file \n");
										fflush(stdout);
										SRCCE_client_create_log("File Size is greater than 64KB...Please limit the size of the file" , "admin");

										sleep(2);
									}
								}/* End of while for Expected_output file */
							}/* end of if for ack testcase */
			
						}/* end of if for test case file size valid */
						else/* if size of file testcase is invalid */
						{
							printf("\n File Size is greater than 64KB\n Please limit the size of the file \n");
							fflush(stdout);
							SRCCE_client_create_log("File Size is greater than 64KB...Please limit the size of the file" , "admin");
							sleep(2);
						}
					}/* end of while for testcase file */
				}/* end of if for ack main.c*/
			}/* end of if for file size for main.c */
			else/* if size of file main.c is invalid */
			{
				printf("\n File Size is greater than 64KB\n Please limit the size of the file \n");
				fflush(stdout);
				SRCCE_client_create_log("File Size is greater than 64KB...Please limit the size of the file" , "admin");

				sleep(2);
			}
			
		}/* end of while for testcase file */
		
		
		/* remove the file from admin side */
		system("rm ./main.c ./.main.c.swp &> error.txt");
		
		/* remove the file from admin side */
		system("rm ./testcase ./.testcase.swp &> error.txt");
				
		/* remove the file from admin side */
		system("rm ./Expected_output ./.Expected_output.swp &> error.txt");
		
	}
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_client_remove_question
 *
 * DESCRIPTION: Takes the name of the question to be removed from admin.
 * Sends that name to the server for validation. If question name is correct
 * It gets the acknowledgement that the required question is removed. Otherwise
 * admin is displayed "incorrect question" and taken to previous menu.
 * 	
 * ARGUMENTS: socket: socket descriptor.
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_client_remove_question(int socket)
{
	char* recv_msg = NULL;

	char question_name[MSG] = {ZERO};
	
	int length_flag = INVALID_LEN;

	printf("\nEntering remove question...\n");
	fflush(stdout);
	SRCCE_client_create_log("Entering remove question..." , "admin");

	sleep(1);
	
	/* receive question name request from admin */
	recv_msg = (char *)malloc(sizeof(char) * MSG);
	if( NULL == recv_msg)
    {
       	printf("\n Failed to allocate the memory to recv_msg \n");
        SRCCE_client_create_log("Failed to allocate the memory to recv_msg" , "admin"); 
        exit(EXIT_FAILURE);
    } 

	SRCCE_client_receive_message(socket,recv_msg);
	printf("%s", recv_msg);
	fflush(stdout);
	SRCCE_client_create_log(recv_msg , "admin");
	free(recv_msg);
	
	while( INVALID_LEN == length_flag )
	{
		/* take Question name from admin */
		scanf(" %[^\n]%*c",question_name);
	
		/* check the size of question name */
		length_flag = SRCCE_client_validate_string_length(question_name, Q_NAME);
		
		if(INVALID_LEN == length_flag)
		{
			printf("\nInvalid Question name\n\nMaximum allowd characters are 50\nEnter Question name: ");
			fflush(stdout);
			SRCCE_client_create_log("Invalid Question name...Maximum allowd characters are 50...Enter Question name:" , "admin");
			sleep(1);
		}
	}
	
	/* send the question name to be deleted  to server*/
	SRCCE_client_send_message(socket, question_name);

	/* receive acknowledgement */
	recv_msg = (char *)malloc(sizeof(char) * MSG);
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
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_client_add_remove_question
 *
 * DESCRIPTION: Admin is shown with choices to add / remove a question. 
 * And according to the admin's choice he is allowed to add/remove a question. 
 * 	
 * ARGUMENTS: socket: socket descriptor.
 *
 * RETURNS: Nothing. 
 *	   
***************************************************************************/
void SRCCE_client_add_remove_question(int socket)
{
	/* 1 for add question 2 for remove question */
	char choice[CHOICE_SIZE] = {ZERO};
	
	/* variable to check if the choice entered is valid or not */
	int valid = INVALID_CHOICE;
		
	do
	{
		/* display choices */
		printf("\n----------------ADD/REMOVE question--------------------\n");
		printf("\n	1. ADD question");
		printf("\n	2. REMOVE question");
		printf("\n	3. Display Questions");
		printf("\n	4. EXIT");
		printf("\n	5. CLEAR");
		printf("\n enter choice : ");
		fflush(stdout);
		SRCCE_client_create_log("----------------ADD/REMOVE question--------------------", "admin");
		SRCCE_client_create_log("1. ADD question", "admin");
		SRCCE_client_create_log("2. REMOVE question", "admin");
		SRCCE_client_create_log("3. Display Questions", "admin");
		SRCCE_client_create_log("4. EXIT", "admin");
		SRCCE_client_create_log("enter choice : ", "admin");

		/* take choice from question */
		scanf(" %[^\n]",choice);
	
		/* validate the choice */
		valid = SRCCE_client_validate_choice(choice);
		
		if( INVALID_CHOICE == valid || CHAR_CHOICE == valid )
		{
			printf("\n Invalid choice \n Please re-enter \n");
			SRCCE_client_create_log("Invalid choice...Please re-enter ", "admin");
			sleep(1);
		}
		else
		{
			
			if( FIVE != atoi(choice))
			{
				/* send choice to server */
				SRCCE_client_send_message(socket,choice);
			}
			/* select according to the user */
			switch(atoi(choice))
			{
				case ONE: SRCCE_client_add_question(socket);
						break;
						
				case TWO: SRCCE_client_remove_question(socket);
						break;
						
				case THREE: SRCCE_client_display_questions(socket);
						break;
						
				case FOUR: printf("\nExiting from add/remove question...\n");
						fflush(stdout);
						sleep(2);
						SRCCE_client_create_log("Exiting from add/remove question...", "admin");
						break;
						
				case FIVE: system("clear");
						break;
						
				default: printf("\n Invalid choice \n");
						fflush(stdout);
						SRCCE_client_create_log("Invalid choice", "admin");

			}
		}
	}while(FOUR != atoi(choice));
}
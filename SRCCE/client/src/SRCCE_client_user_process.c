/**************************************************************************************
*
*  FILE NAME	: SRCCE_client_user_process.c
*
*  DESCRIPTION: This contains functions for user process.
*
*************************************************************************************/

#include "SRCCE_client_header.h"

/***************************************************************************
 * FUNCTION NAME: SRCCE_client_edit_question
 *
 * DESCRIPTION: This allows the user to receive a question's answer file and
 * edit and send it back to the server. 
 *
 * ARGUMENTS: socket: socket descriptor.
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_client_edit_question(int socket)
{
	char choice[CHOICE_SIZE] = "n";
	int filesize_flag = INVALID_FILESIZE;
	int valid_choice = INVALID_CHOICE;
	
	char *recv_msg = NULL;
	recv_msg = (char *)malloc(sizeof(char) * MSG);

	/* check for error while allocating memory */
	if( NULL == recv_msg)
	{
		printf("\n Failed to allocate the memory to recv_msg \n");
		SRCCE_client_create_log("Failed to allocate the memory to recv_msg" , "user"); 
		exit(EXIT_FAILURE);
	} 

	SRCCE_client_receive_message(socket,recv_msg);
	printf(" %s ",recv_msg);
	SRCCE_client_create_log( recv_msg, "user"); 
	fflush(stdout);
	free(recv_msg);
			
	/* receive and create the main.c file */
	SRCCE_client_receive_file(socket, "main.c");
	
	/* edit the answer file */ 
	while(INVALID_FILESIZE == filesize_flag)
	{
		filesize_flag = INVALID_FILESIZE;
		strcpy(choice, "n");
		
		printf("\nopening the answer file...\n");
		fflush(stdout);
		SRCCE_client_create_log( "opening the answer file...", "user"); 
		sleep(2);
			
		/* edit the main.c file */
		while(ZERO == strcmp(choice,"n") || ZERO == strcmp(choice,"N"))
		{
			/* open the file to the user */
			system("vi main.c");
			
			valid_choice = INVALID_CHOICE;
			while( CHAR_CHOICE != valid_choice )
			{
				printf("\nAre you done editing ?(y/n): ");
				fflush(stdout);
				SRCCE_client_create_log("Are you done editing ?(y/n): ", "user");
							
				scanf(" %[^\n]",choice);
				valid_choice = SRCCE_client_validate_choice(choice);
				
				/* check if the choice is vaild or not */
				if(INVALID_CHOICE == valid_choice || DIGIT_CHOICE == valid_choice)
				{
					printf("\nInvalid Choice\nPlease Re-enter...");
					fflush(stdout);
					SRCCE_client_create_log("Invalid Choice...Please Re-enter... ", "user");
					sleep(1);
				}
			}	
		}
		
		/* check the file size */
		filesize_flag = SRCCE_client_check_filesize("./main.c");
		
		/* if a valid size send the file */
		if( VALID_FILESIZE == filesize_flag)
		{
			/* send the file */
			SRCCE_client_send_file(socket, "./main.c");
		
			/* remove the file from user side */
			system("rm ./main.c ./.main.c.swp &> error.txt");
		
			printf("\nmain.c file sent successfully\n");
			SRCCE_client_create_log("main.c file sent successfully", "user");
			sleep(2);
		}
		else
		{
			printf("\nMaximum file size is 64KB.\nPlease limit the size of answer file\n");
			fflush(stdout);
			SRCCE_client_create_log("Maximum file size is 64KB...Please limit the size of answer file", "user");
			sleep(2);
		}
	}/* end of while for file size*/
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_client_compile_run
 *
 * DESCRIPTION:  This allows the user to compile and run the code. It displays
 * all the warnings and errors to the user if there are any. In case of successful
 * compilation it displays the output of the user's code.
 * In case of unsuccessful compilation it only shows the warnings and errors.
 *
 * ARGUMENTS: socket: socket descriptor. 
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_client_compile_run(int socket)
{
	
	char *recv_msg = NULL;
	char *send_msg = NULL;
	int error_flag = ZERO;
	int compile_flag = ZERO;
	int ack_file = RECEIVE_FAIL;
		
	recv_msg = (char *)malloc(sizeof(char) * MSG);
	/* check for error while allocating memory */
	if( NULL == recv_msg)
	{
		printf("\n Failed to allocate the memory to recv_msg \n");
		SRCCE_client_create_log("Failed to allocate the memory to recv_msg" , "user"); 
		exit(EXIT_FAILURE);
	} 

	SRCCE_client_receive_message(socket,recv_msg);
	printf(" %s ",recv_msg);
	fflush(stdout);
	SRCCE_client_create_log( recv_msg, "user"); 
	free(recv_msg);
	
	/* receive the compile_flag flag from server */
	recv_msg = (char *)malloc(sizeof(char) * MSG);
	if( NULL == recv_msg)
	{
		printf("\n Failed to allocate the memory to recv_msg \n");
		SRCCE_client_create_log("Failed to allocate the memory to recv_msg" , "user"); 
		exit(EXIT_FAILURE);
	} 
	SRCCE_client_receive_message(socket,recv_msg);
	compile_flag = atoi(recv_msg);
	free(recv_msg);
	
	if( ZERO == compile_flag)
	{
		printf("\n Too many errors or warnings ....");
		SRCCE_client_create_log( "Too many errors or warnings ....", "user");
	}
	else
	{
		
		while(RECEIVE_FAIL == ack_file)
		{
			/* receive the compilation result */
			ack_file = SRCCE_client_receive_file_compile(socket, "compile.txt");
			
			/* send the acknowlegment to the server */
			send_msg = (char *)malloc(sizeof(char) * MSG);
			/* check for error while allocating memory */
			if( NULL == recv_msg)
			{
				printf("\n Failed to allocate the memory to recv_msg \n");
				SRCCE_client_create_log("Failed to allocate the memory to recv_msg" , "user"); 
				exit(EXIT_FAILURE);
			} 

			snprintf(send_msg,MSG,"%d",ack_file);
			SRCCE_client_send_message(socket,send_msg);
			free(send_msg);
			
			if( RECEIVE_SUCCESS == ack_file)
			{
				printf("\ncompile file received...");
				fflush(stdout);
				SRCCE_client_create_log("compile file received..." , "user"); 
				sleep(1);
				break;
			}
			else
			{
				printf("\ncompile file not received...");
				fflush(stdout);
				SRCCE_client_create_log("compile file not received..." , "user"); 
				sleep(1);
			}
			
		}
		printf("\n--------COMPILATION RESULT----------\n");
		SRCCE_client_create_log("--------COMPILATION RESULT----------" , "user"); 
		fflush(stdout);
		system("cat compile.txt &> error.txt");
		fflush(stdout);	
		sleep(2);	
		/* remove the file from user side */
		system("rm ./compile.txt ./.compile.txt.swp &> error.txt");
	}
		
	/* receive the error_flag from server */
	recv_msg = (char *)malloc(sizeof(char) * MSG);
	/* check for error while allocating memory */
	if( NULL == recv_msg)
	{
		printf("\n Failed to allocate the memory to recv_msg \n");
		SRCCE_client_create_log("Failed to allocate the memory to recv_msg" , "user"); 
		exit(EXIT_FAILURE);
	}
	SRCCE_client_receive_message(socket,recv_msg);
	error_flag = atoi(recv_msg);
	free(recv_msg);
		
	printf("\n--------OUTPUT----------\n");
	fflush(stdout);
	SRCCE_client_create_log("--------OUTPUT----------" , "user");	
	
	/* Compilataion successful */
	if( ONE == error_flag)
	{
		SRCCE_client_receive_file(socket,"actual_output");
		system("cat actual_output");
		fflush(stdout);
		
		/* remove the file from user side */
		system("rm ./actual_output ./.actual_output.swp &> error.txt");
	}
	else/* warnings are there */
	{
		printf("\nNo Output generated as compilation failed\n");
		SRCCE_client_create_log("No Output generated as compilation failed" , "user");	
	}	
}

/***************************************************************************
 * FUNCTION NAME: SRCCE_client_see_result
 *
 * DESCRIPTION:  Shows that which of the test cases passed and which failed.
 * It also shows how many of them passed.
 * 	
 * ARGUMENTS: socket: socket descriptor.
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_client_see_result(int socket)
{
	char *recv_msg = NULL;
	int size_of_file = ZERO;
	
	recv_msg = (char *)malloc(sizeof(char) * MSG);
	/* check for error while allocating memory */
	if( NULL == recv_msg)
	{
		printf("\n Failed to allocate the memory to recv_msg \n");
		SRCCE_client_create_log("Failed to allocate the memory to recv_msg" , "user"); 
		exit(EXIT_FAILURE);
	}

	SRCCE_client_receive_message(socket,recv_msg);
	printf(" %s ",recv_msg);
	fflush(stdout);
	SRCCE_client_create_log( recv_msg, "user"); 
	free(recv_msg);
	
	/* receive the actual output file size */
	recv_msg = (char *)malloc(sizeof(char) * MSG);
	/* check for error while allocating memory */
	if( NULL == recv_msg)
	{
		printf("\n Failed to allocate the memory to recv_msg \n");
		SRCCE_client_create_log("Failed to allocate the memory to recv_msg" , "user"); 
		exit(EXIT_FAILURE);
	}

	SRCCE_client_receive_message(socket,recv_msg);
	size_of_file = atoi(recv_msg);
	free(recv_msg);
	
	/* No output generated as compilation failed */
	if( ZERO == size_of_file)
	{
		printf("\n\t----------RESULT---------\t\n");
		printf("\n 0 Test cases passed....");
		SRCCE_client_create_log("----------RESULT---------\n 0 Test cases passed...." , "user"); 
		fflush(stdout);
	}
	else
	{
		printf("\n\t----------RESULT---------\t\n");
		fflush(stdout);
		SRCCE_client_create_log("----------RESULT---------" , "user"); 
		SRCCE_client_receive_file(socket, "Result");
		system("cat Result");
		
		/* remove the file from user side */
		system("rm ./Result ./.Result.swp &> error.txt");
	}
}

/***************************************************************************
 * FUNCTION NAME: SRCCE_client_display_questions
 *
 * DESCRIPTION: Displays the questions name and their description to user.
 * 	
 * ARGUMENTS: socket: socket descriptor.
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_client_display_questions(int socket)
{
	char *recv_msg = NULL;
	
	/* iterator variable */
	int iterator = ZERO;
	
	int total_questions = ZERO;
	
	/* receive the total number of questions */
	recv_msg = (char *)malloc(sizeof(char) * MSG);
	SRCCE_client_receive_message(socket,recv_msg);
	total_questions = atoi(recv_msg);
	free(recv_msg);
	
	printf("\n\t\t----Questions---\t\t\n");
	fflush(stdout);
	
	if(ZERO == total_questions)
	{
		printf("\nThere 0 Questions in the Question_Bank...\nPlease check after some time..");
		fflush(stdout);
		sleep(2);
	}
	else
	{
		/* display the questions to the user */
		for( iterator = ZERO; iterator < total_questions; iterator++)
		{
			/* receive question and display it  */
			recv_msg = (char *)malloc(sizeof(char) * MSG);
			/* check for error while allocating memory */
			if( NULL == recv_msg)
			{
				printf("\n Failed to allocate the memory to recv_msg \n");
				SRCCE_client_create_log("Failed to allocate the memory to recv_msg" , "user"); 
				exit(EXIT_FAILURE);
			}

			SRCCE_client_receive_message(socket,recv_msg);
			printf("%s\n",recv_msg );
			fflush(stdout);
			SRCCE_client_create_log(recv_msg , "user");
			free(recv_msg);
		}	
	}
}

/***************************************************************************
 * FUNCTION NAME: SRCCE_client_select_question
 *
 * DESCRIPTION: It displays the questions to the user and allows him to select
 * the question. After a question is selected he is presented with a set of 
 * options that he can perform on the question.
 * 	
 * ARGUMENTS: socket: socket descriptor.
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_client_select_question(int socket)
{
	char question[Q_NAME] = {ZERO};
	char *recv_msg = NULL;
	int valid_flag = INVALID_QUESTION;
	char user_choice[CHOICE_SIZE] = {ZERO};
	int valid = INVALID_CHOICE;
		
	/* display questions */
	SRCCE_client_display_questions(socket);
	
	printf("\n Please type EXIT for previous menu....");
	fflush(stdout);
	SRCCE_client_create_log("Please type EXIT for previous menu....", "user");
	
	/* take question name from user */
	printf("\n Enter Question Name: ");
	SRCCE_client_create_log(" Enter Question Name:", "user");
	scanf(" %[^\n]",question);
	
	/* send the question name to server */
	SRCCE_client_send_message(socket, question);
	
	/* check if user choose "exit/EXIT" */
	if( ZERO == strcasecmp(question,"exit"))
	{
		printf("\nEXTING FROM THIS MENU....");
		SRCCE_client_create_log("EXTING FROM THIS MENU....", "user");
		fflush(stdout);
		sleep(1);
		return;
	}
	
	/* receive the valid_flag if question name is valid or not */
	recv_msg = (char *)malloc(sizeof(char) * MSG);
	/* check for error while allocating memory */
	if( NULL == recv_msg)
	{
		printf("\n Failed to allocate the memory to recv_msg \n");
		SRCCE_client_create_log("Failed to allocate the memory to recv_msg" , "user"); 
		exit(EXIT_FAILURE);
	}

	SRCCE_client_receive_message(socket,recv_msg);
	valid_flag = atoi(recv_msg);
	free(recv_msg);
	
	/* if question name is invalid */
	if( INVALID_QUESTION == valid_flag)
	{
		printf("\n Invalid choice \n Please re-enter \n");
		fflush(stdout);
		SRCCE_client_create_log("Invalid choice...Please re-enter" , "user"); 
		sleep(1);
	}
	else
	{
		/* 1. Answer Question
		   2. Compile/run
	       3. See Results
	       4. EXIT
		*/
		do
		{
			printf("\n --------------- MAIN MENU  -----------------\n");
			printf("\n1. Answer Question");
			printf("\n2. Compile/Run");
			printf("\n3. See Result");
			printf("\n4. Exit");
			printf("\nenter choice:");
			fflush(stdout);
			SRCCE_client_create_log("--------------- MAIN MENU  -----------------" , "user"); 
			SRCCE_client_create_log("1. Answer Question" , "user"); 
			SRCCE_client_create_log("2. Compile/Run" , "user"); 
			SRCCE_client_create_log("3. See Result" , "user"); 
			SRCCE_client_create_log("4. Exit", "user"); 
			SRCCE_client_create_log("Enter choice:" , "user"); 

			scanf(" %[^\n]",user_choice);
			valid = SRCCE_client_validate_choice(user_choice);
			if( INVALID_CHOICE == valid || CHAR_CHOICE == valid )
			{
				printf("\n Invalid choice \n Please re-enter \n");
				SRCCE_client_create_log("Invalid choice...Please re-enter" , "user");
				sleep(1);
			}
			else
			{
				/* send the user_choice */
				SRCCE_client_send_message(socket,user_choice);
				
				/* select function according to admin's choice */
				switch(atoi(user_choice))
				{
					case ONE: SRCCE_client_edit_question(socket);
							break;
					case TWO: SRCCE_client_compile_run(socket);
							break;
					case THREE: SRCCE_client_see_result(socket);
							break;
					case FOUR: printf("\nEXIT\n");
							fflush(stdout);
							sleep(1);
							break;					
					default: printf("\n invalid choice by user\n ");
							SRCCE_client_create_log("Invalid choice by user" , "user");
							sleep(1);
				}
			}
		
		}while(FOUR != atoi(user_choice));	
	}
}

/***************************************************************************
 * FUNCTION NAME: SRCCE_client_user_process
 *
 * DESCRIPTION:  This allows the user to either go into the question menu or
 * log off from the system.
 * 	
 * ARGUMENTS: socket: socket descriptor.
 *
 * RETURNS: Nothing. 
 *	   
***************************************************************************/
void SRCCE_client_user_process(int socket, char *username)
{
	/* 1. Select question
	   2. LOGOFF
	*/
	char log_msg[MSG] = {ZERO};
	char user_choice[CHOICE_SIZE] = {ZERO};
	int valid = INVALID_CHOICE;
	
	do
	{
		printf("\n --------------- welcome %s -----------------\n", username);
		printf("\n1. Select Question");
		printf("\n2. LOGOFF");
		printf("\nenter choice:");
		fflush(stdout);
	
		snprintf(log_msg, MSG, "--------------- welcome %s -----------------", username);
		SRCCE_client_create_log( log_msg, username); 
		SRCCE_client_create_log("1. Select Question" , username); 
		SRCCE_client_create_log("2. LOGOFF" , username); 
		SRCCE_client_create_log("Enter choice:" , username); 
		
		scanf(" %[^\n]",user_choice);
		valid = SRCCE_client_validate_choice(user_choice);
		if( INVALID_CHOICE == valid || CHAR_CHOICE == valid)
		{
			printf("\n Invalid choice \n Please re-enter \n");
			SRCCE_client_create_log("Invalid choice...Please re-enter" , username);
			sleep(1);
		}
		else
		{
			/* send the user_choice */
			SRCCE_client_send_message(socket,user_choice);
				
			/* select function according to admin's choice */
			switch(atoi(user_choice))
			{
				case ONE: SRCCE_client_select_question(socket);
						break;
				case TWO: printf("\n LOGOFF\n");
						fflush(stdout);
						SRCCE_client_create_log("logged off" , username);
						free(username);
						break;		
				default: printf("\n invalid choice by %s user\n ",username);
						 snprintf(log_msg, MSG, "Invalid choice by %s user",username);
						 SRCCE_client_create_log( log_msg, username); 
			}
		}
		
	}while(TWO != atoi(user_choice));	
}

/**************************************************************************************
*
*  FILE NAME	: SRCCE_server_add_remove_question.c
*
*  DESCRIPTION: This file contains functions for  admin's add remove question process.
*
*************************************************************************************/

#include "SRCCE_server_header.h"

/***************************************************************************
 * FUNCTION NAME: SRCCE_server_add_question
 *
 * DESCRIPTION: Allows the admin to create a question for the user. Admin sends
 * the question's name and description and after their validation admin is 
 * allowed to create the main.c file, testcases and expected output file and all
 * these files are then received by the server where they are saved in the question 
 * bank. All users question bank is then updated.
 * 	
 * ARGUMENTS: socket: socket descriptor.
 * 
 * RETURNS: Nothing.
 * 				   
***************************************************************************/
void SRCCE_server_add_question(int socket)
{
	/* flag vairable to store if question exists or not */
	int flag = INVALID_QUESTION;
	
	/* variable to store the acknowledgement
		if file received successfully */
	int ack_file = RECEIVE_FAIL;
	
	/* Pointer variable to take the send message */
	char *send_msg = NULL;
	
	/* query string */
	char query[MSG] = {ZERO};
	
	/* strings to store question name and question description */
	char question_name[Q_NAME] = {ZERO};
	char question_description[Q_DESCRIPTION] = {ZERO};

	/* string to store the Question Bank path */
	char Question_path[PATH] = "../../Question_Bank/";
	char Question_path1[PATH] = "../../Question_Bank";

	/* string to store the user directory path */
	char user_dir_path[PATH] = "../../users/";

	/* temp string to store the path */
	char temp_dir_path[PATH] = {ZERO};
	
	/* string to store the command */
	char command[COMMAND_LENGTH] = {ZERO};
	
	/* log message string */
	char log_msg[MSG] = {ZERO};
	
	printf("\nadmin entered add question...\n");
	fflush(stdout);
	
	/* add to log file */
	SRCCE_server_create_log("admin entered add question...", "admin");
	
	/* send enter question name message */
	SRCCE_server_send_message(socket, "\n Enter the question name : ");
	
	/* receive question name from admin */
	SRCCE_server_receive_message(socket,question_name);
	
	printf("\nReceived the question name...");
	puts(question_name);
	fflush(stdout);
	
	/* add to log file */
	SRCCE_server_create_log("Received the question name...", "admin");
	snprintf(log_msg, MSG, "Question name: %s", question_name);
	SRCCE_server_create_log(log_msg, "admin");
	
	/* send enter question description message */
	SRCCE_server_send_message(socket, "\n Please enter the question description:");
	
	/* receive question description from admin */
	SRCCE_server_receive_message(socket,question_description);
	
	printf("\nReceived the question description...");
	puts(question_description);
	fflush(stdout);
	
	/* add to log file */
	SRCCE_server_create_log("Received the question description...", "admin");
	snprintf(log_msg, MSG, "Question Description: %s", question_description);
	SRCCE_server_create_log(log_msg, "admin");
		
	/* check if the question name exists in the Question Bank */	
	MYSQL *con = my_sql_init();
  
	/* Enter the Sql query and run  it */
	if (mysql_query(con, "SELECT * FROM Question_Bank")) 
	{
		SRCCE_server_finish_with_error(con);
	}
  
	/* store rows in a result set */
	MYSQL_RES *result = mysql_use_result(con);
	
	/* check for error */
	if (result == NULL) 
	{
    	SRCCE_server_finish_with_error(con);
	}
	
	MYSQL_ROW row;
	
	/* fetch until end of table reached */
	while ((row = mysql_fetch_row(result))) 
	{
		flag = INVALID_QUESTION ;
      
    	if(strcmp(question_name,row[ZERO])== ZERO )
		{
			flag = VALID_QUESTION;
			break;
		}
	}
	
	/* send flag if question name exists or not */
	send_msg = (char *)malloc(sizeof(char) * MSG);
	
	/* check for error while allocating memory */
	if( NULL == send_msg)
	{
		printf("\n Failed to allocate the memory to send_msg \n");
		SRCCE_server_create_log("Failed to allocate the memory to send_msg" , "admin"); 
		exit(EXIT_FAILURE);
	}
	snprintf(send_msg,MSG,"%d",flag);
	SRCCE_server_send_message(socket,send_msg);
	free(send_msg);
	
	printf("\nquestion exists: %d", flag);
	fflush(stdout);
	
	/* free the result set */
	mysql_free_result(result);
	
	/* If the question name already exists */
	if(flag == VALID_QUESTION)
	{
		SRCCE_server_send_message(socket,"Question Name already exist...\n");
		printf("\nQuestion name:%s, Already exists in the Question Bank...\n",question_name);
		fflush(stdout);
		
		/* add to log file */
		snprintf(log_msg, MSG, "Question name:%s, Already exists in the Question Bank...",question_name);
		SRCCE_server_create_log(log_msg, "admin");
		
		/* close the connection to database and clean up memory */
		mysql_close(con);
		mysql_library_end();
	}
	else /* if question name doesn't exists */
	{
		/* create the question directory in question bank */
		strcpy(temp_dir_path, Question_path);
		strcat(temp_dir_path,question_name);
		strcpy(command,"mkdir ");
		strcat(command, temp_dir_path);
		system(command);
		
		/* receive and create the main.c file */
		ack_file = SRCCE_server_receive_file(socket, "main.c");
		
		/* now send the ack_file flag to admin */
		send_msg = (char * )malloc(sizeof(char)* MSG);
		
		/* check for error while allocating memory */
		if( NULL == send_msg)
		{
			printf("\n Failed to allocate the memory to send_msg \n");
			SRCCE_server_create_log("Failed to allocate the memory to send_msg" , "admin"); 
			exit(EXIT_FAILURE);
		}
		snprintf(send_msg, MSG, "%d", ack_file);
		SRCCE_server_send_message(socket, send_msg);
		free(send_msg);
			
		/* if main.c received successfully */
		if(RECEIVE_SUCCESS == ack_file)
		{
			/* move the question file(main.c) to it's directory */
			strcpy(command,"mv ./main.c ");
			strcpy(temp_dir_path, Question_path);
			strcat(temp_dir_path,question_name);
			strcat(command, temp_dir_path);
			system(command);
			
			/* receive and create the testcase file */
			ack_file = SRCCE_server_receive_file(socket, "testcase");
			
			/* now send the ack_file flag to admin */
			send_msg = (char * )malloc(sizeof(char)* MSG);
			
			/* check for error while allocating memory */
			if( NULL == send_msg)
			{
				printf("\n Failed to allocate the memory to send_msg \n");
				SRCCE_server_create_log("Failed to allocate the memory to send_msg" , "admin"); 
				exit(EXIT_FAILURE);
			}
			
			snprintf(send_msg, MSG, "%d", ack_file);
			SRCCE_server_send_message(socket, send_msg);
			free(send_msg);
		
			/* if testcase file received successfully */
			if( RECEIVE_SUCCESS == ack_file)
			{	
				/* move the testcase file(testcase) to it's directory */
				strcpy(command,"mv ./testcase ");
				strcpy(temp_dir_path, Question_path);
				strcat(temp_dir_path,question_name);
				strcat(command, temp_dir_path);
				system(command);
								
				/* receive and create the Expected_output file */
				ack_file = SRCCE_server_receive_file(socket, "Expected_output");
				
				if( RECEIVE_SUCCESS == ack_file )
				{
					/* move the Expected_output file(Expected_output) to it's directory */
					strcpy(command,"mv ./Expected_output ");
					strcpy(temp_dir_path, Question_path);
					strcat(temp_dir_path,question_name);
					strcat(command, temp_dir_path);
					system(command);
				
					/* now send the ack_file flag to admin */
					send_msg = (char * )malloc(sizeof(char)* MSG);
				
					/* check for error while allocating memory */
					if( NULL == send_msg)
					{
						printf("\n Failed to allocate the memory to send_msg \n");
						SRCCE_server_create_log("Failed to allocate the memory to send_msg" , "admin"); 
						exit(EXIT_FAILURE);
					}
					snprintf(send_msg, MSG, "%d", ack_file);
					SRCCE_server_send_message(socket, send_msg);
					free(send_msg);
										
					printf("\nQuestion name:%s, Adding in the Question Bank...\n",question_name);
					fflush(stdout);
					
					printf("\nQuestion name:%s, successfully added in the Question Bank...\n",question_name);
					fflush(stdout);
				
					/* add to log file */
					snprintf(log_msg, MSG, "Question name:%s, successfully added in the Question Bank...\n",question_name);
					SRCCE_server_create_log(log_msg, "admin");
					
					/* add to log file */
					snprintf(log_msg, MSG, "Question name:%s, Adding in the Question Bank...",question_name);
					SRCCE_server_create_log(log_msg, "admin");
		
					/* Insert question details in the database */
					snprintf(query, MSG, "INSERT INTO Question_Bank VALUES ('%s','%s')", question_name, question_description);
					if (mysql_query(con, query))
					{
						SRCCE_server_finish_with_error(con);
					}
				
					printf("\nQuestion name:%s, Added in the Question Bank...\n",question_name);
					fflush(stdout);
		
					/* add to log file */
					snprintf(log_msg, MSG, "Question name:%s, Added in the Question Bank...\n",question_name);
					SRCCE_server_create_log(log_msg, "admin");
					
				}/* end of ack =1 for expected output */
				
			}/* end of if 1 == ack_file  for test case*/
		}
	
		if( RECEIVE_FAIL == ack_file)
		{
			/* remove the directories */
			strcpy(temp_dir_path, Question_path);
			strcat(temp_dir_path,question_name);
			strcpy(command,"rm -rf ");
			strcat(command, temp_dir_path);
			system(command);
		}
		else /* if ack_file is 1 */
		{
		
			/* updating Question bank for all registered user */
			if (mysql_query(con, "SELECT * FROM login_credentials")) 
			{
				SRCCE_server_finish_with_error(con);
			}
  
			MYSQL_RES *result = mysql_use_result(con);
			if (result == NULL) 
			{
				SRCCE_server_finish_with_error(con);
			}

			MYSQL_ROW row;
		
			while ((row = mysql_fetch_row(result))) 
			{

				if(!strcmp(row[ZERO],"admin"))
				{
					continue;
				}
			
				strcpy(command,"rm -rf ");
				strcpy(temp_dir_path, user_dir_path);
				strcat(temp_dir_path,row[ZERO]);
				strcat(temp_dir_path, "/Question_Bank");
				strcat(command, temp_dir_path);
				system(command);
						
				/* copying the Question bank into each new user directory */
				strcpy(temp_dir_path, user_dir_path);
				strcat(temp_dir_path,row[ZERO]);
				strcat(temp_dir_path,"/");
				strcpy(command,"cp -r ");
				strcat(command, Question_path1);
				strcat(command," ");
				strcat(command, temp_dir_path);
				system(command);			
			}
			/* free the result set */
			mysql_free_result(result);
		
			/* close the connection and free up memory */
			mysql_close(con);
			mysql_library_end();
		}
		
	}/* end of else if question name doesn't exists */
}

/***************************************************************************
 * FUNCTION NAME: SRCCE_server_remove_question
 *
 * DESCRIPTION: Receives the name of the question to be removed from admin.
 * It then validates the question_name. If question name is correct it sends
 * the acknowledgement that the required question is removed.
 * The question is removed from the Question Bank and each user's Question Bank
 * is updated.
 * Otherwise admin is displayed "incorrect question" and taken to previous menu.
 * 	
 * ARGUMENTS: socket: socket descriptor.
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_server_remove_question(int socket)
{
	/* flag to store if the question exists or not */
	int flag = INVALID_QUESTION;
	
	/* query string */
	char query[MSG] = {ZERO};
	
	/* question name string */
	char question_name[Q_NAME] = {ZERO};
	
	/* string to store the question bank path */
	char Question_path1[PATH] = "../../Question_Bank";
	char Question_path[PATH] = "../../Question_Bank/";
	
	/* string to store the user directory path */
	char user_dir_path[PATH] = "../../users/";

	/* string to store temp path */
	char temp_dir_path[PATH] = {ZERO};
	
	/* command string */
	char command[COMMAND_LENGTH] = {ZERO};
	
	/* log message string */
	char log_msg[MSG] = {ZERO};

	printf("\nAdmin entered remove question...\n");
	fflush(stdout);
	
	/* add to log file */
	SRCCE_server_create_log("Admin entered remove question..." , "admin"); 
	
	/* send the enter question name message to admin */
	SRCCE_server_send_message(socket, "\n Enter the question name : ");
	
	/* receive question name from admin that is to be deleted*/
	SRCCE_server_receive_message(socket,question_name);
	
	printf("\nReceived question name: %s\n", question_name);
	fflush(stdout);
	
	/* add to log file */
	snprintf(log_msg, MSG, "Received question name: %s", question_name);
	SRCCE_server_create_log(log_msg, "admin");
	
	/* connect with SRCCE_database */
	MYSQL *con = my_sql_init();
  
	/* enter the query to take data Question_Bank table */
	if (mysql_query(con, "SELECT * FROM Question_Bank")) 
	{
		/* error */
		SRCCE_server_finish_with_error(con);
	}
  
	/* take the data from the tables and store it in result */
	MYSQL_RES *result = mysql_use_result(con);
	
	/* check for error */
	if (result == NULL) 
	{
    	SRCCE_server_finish_with_error(con);
	}

	MYSQL_ROW row;

	/* fetch the rows until reached the end of table */
	while ((row = mysql_fetch_row(result))) 
	{
		flag = INVALID_QUESTION;
		
		/* if question found in Question Bank */ 
    	if(strcmp(question_name,row[ZERO])== ZERO )
		{
			flag = VALID_QUESTION;
			break;
		}
	}
	
	/* free the result set */
	mysql_free_result(result);
	
	/* if Question found in the Question Bank */
	if(flag == VALID_QUESTION)
	{
		/* Delete question's details from question bank table */
		snprintf(query, MSG, "DELETE FROM Question_Bank WHERE Question_Name = ('%s')", question_name);
		
		/* check for error */
		if (mysql_query(con, query))
		{
			SRCCE_server_finish_with_error(con);
		}
		
		/* send the acknowledgement */
		SRCCE_server_send_message(socket,"\nQuestion Deleted Successfully...\n");
		printf("\nQuestion Name: %s, successfully deleted from Question Bank...\n",question_name);
		fflush(stdout);
		
		/* add to log file */
		snprintf(log_msg, MSG, "Question Name: %s, successfully deleted from Question Bank...\n",question_name);
		SRCCE_server_create_log(log_msg, "admin");
		
		/* delete the question directory in question bank */
		strcpy(command,"rm -rf ");
		strcpy(temp_dir_path, Question_path);
		strcat(temp_dir_path, question_name);
		strcat(command, temp_dir_path);
		system(command);	
	
		/* update question for each user */
		printf("\nUpdating Question_Bank for each user...");
		fflush(stdout);
		
		/* add to log file */
		SRCCE_server_create_log("Updating Question_Bank for each user...", "admin");
		
		if (mysql_query(con, "SELECT * FROM login_credentials")) 
		{
			SRCCE_server_finish_with_error(con);
		}
  
		MYSQL_RES *result = mysql_use_result(con);
		if (result == NULL) 
		{
      			SRCCE_server_finish_with_error(con);
		}

		MYSQL_ROW row;
		while ((row = mysql_fetch_row(result))) 
		{

			if(!strcmp(row[ZERO],"admin"))
			{	
				continue;
			}
			
			/* delete the question bank of each user */
			strcpy(command,"rm -rf ");
			strcpy(temp_dir_path, user_dir_path);
			strcat(temp_dir_path,row[ZERO]);
			strcat(temp_dir_path,"/Question_Bank");			
			strcat(command, temp_dir_path);
			system(command);				
				
			/* Copy the New Question bank into each user directory */
			strcpy(temp_dir_path, user_dir_path);
			strcat(temp_dir_path,row[ZERO]);
			strcat(temp_dir_path,"/");			
			
			strcpy(command,"cp -r ");
			strcat(command, Question_path1);
			strcat(command," ");
			strcat(command, temp_dir_path);
			system(command);
		}
		
		/* free the result set */
		mysql_free_result(result);
		
		/* close the connection with the database and free up memory */
		mysql_close(con);
		mysql_library_end();
		
	}/* end of if 1 == flag */
	else /* flag == 0,  Question not found in Question Bank */
	{
		SRCCE_server_send_message(socket,"\n\t\tQuestion Doesnt exist...\n");
		printf("\nQuestion name: %s, Doesn't exists in the Question Bank...\n",question_name);
		fflush(stdout);
		
		/* add to log file */
		snprintf(log_msg, MSG, "Question name: %s, Doesn't exists in the Question Bank...",question_name);
		SRCCE_server_create_log(log_msg, "admin");
		
		/* close the connection and clean the memory */
		mysql_close(con);
		mysql_library_end();
	} 
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_server_add_remove_question
 *
 * DESCRIPTION: Receives the choice from the user and accordingly call the 
 * add question or remove question functions or exits from this menu.
 * 	
 * ARGUMENTS: socket: socket descriptor. 
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_server_add_remove_question(int socket)
{
	/* 1 for add question 2 for remove question */
	int choice = ZERO;
	
	/* Pointer to store the received message */
	char *recv_msg = NULL;
	
	printf("\nAdmin entered add/remove question...\n");
	fflush(stdout);
	
	/* add to log file */
	SRCCE_server_create_log("Admin entered add/remove question...", "admin");
	
	do
	{
		/* receive choice from admin */
		recv_msg = (char *)malloc(sizeof(char) * MSG);
		
		/* check for error while allocating memory */
		if( NULL == recv_msg)
		{
			printf("\n Failed to allocate the memory to recv_msg \n");
			SRCCE_server_create_log("Failed to allocate the memory to recv_msg" , "admin"); 
			exit(EXIT_FAILURE);
		}
		
		SRCCE_server_receive_message(socket,recv_msg);
		choice = atoi(recv_msg);
		free(recv_msg);
		
		/* select according to admin's choice */ 
		switch(choice)
		{
			case ONE: 	SRCCE_server_add_question(socket);
						break;
			
			case TWO: 	SRCCE_server_remove_question(socket);
						break;
						
			case THREE: SRCCE_server_display_questions(socket);
						break;
			
			case FOUR:  printf("\nAdmin exits from add/remove question...");
						fflush(stdout);
					
						/* add to log file */
						SRCCE_server_create_log("Admin exits from add/remove question...", "admin");
						break;
		}
	}while(FOUR != choice);
}

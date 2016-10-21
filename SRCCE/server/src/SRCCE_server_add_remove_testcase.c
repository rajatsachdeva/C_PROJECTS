/**************************************************************************************
*
*  FILE NAME	: SRCCE_server_add_remove_testcase.c
*
*  DESCRIPTION: This file contains the functions for admin's add remove testcase process.
*
*************************************************************************************/

#include "SRCCE_server_header.h"

/***************************************************************************
 * FUNCTION NAME: SRCCE_server_update_testcase
 *
 * DESCRIPTION: Question name is received and then checked if it is correct.
 * If it is correct then that question's test cases file is sent to the admin.
 * After editing it is received by the server and then corresponding 
 * Expected output file is sent and after editing is received by the server.
 * Now the question bank of each user is updated to reflect the change.
 * 	
 * ARGUMENTS: socket: socket descriptor.
 *
 * RETURNS: Nothing
 * 				
***************************************************************************/
void SRCCE_server_update_testcase(int socket)
{
	/* question name string */
	char question_name[Q_NAME] = {ZERO};
	
	/* question path string to store the question's path in question bank */
	char question_path[PATH] = {ZERO};
	
	/* command string */
	char command[COMMAND_LENGTH] = {ZERO};
	
	/* pointer for send message */
	char *send_msg = NULL;
		
	/* question bank path string */
	char Question_path1[PATH] = "../../Question_Bank";
	
	/* user directory path */
	char user_dir_path[PATH] = "../../users/";
	
	/* temp directory path */
	char temp_dir_path[PATH] = {ZERO};
	
	/* flag to store if question exists or not in question bank */
	int flag = INVALID_QUESTION;
	
	/* Log message string */
	char log_msg[MSG] = {ZERO};
	
	printf("\nAdmin entered update testcase...\n");
	fflush(stdout);
	
	/* add to log file */
	SRCCE_server_create_log("Admin entered update testcase...", "admin");
	
	/* send enter question name message */
	SRCCE_server_send_message(socket, "\n Enter the question name: ");
	
	/* receive the question_name from admin */
	SRCCE_server_receive_message(socket,question_name);
	printf("\nQuestion name is: %s", question_name);
	fflush(stdout);
	
	/* add to log file */
	snprintf(log_msg, MSG, "Question name is: %s", question_name);
	SRCCE_server_create_log(log_msg, "admin");
	
	/* check if the question name exists in the database */	
	/* connect with SRCCE_database */
	MYSQL *con = my_sql_init();
  	
	/* Enter query to take the data from Question_Bank table */
	if (mysql_query(con, "SELECT * FROM Question_Bank")) 
	{
		SRCCE_server_finish_with_error(con);
	}
  
	/* take the rows from table to result set */
	MYSQL_RES *result = mysql_use_result(con);
	
	/* check for error */
	if (result == NULL) 
	{
    	SRCCE_server_finish_with_error(con);
	}
	
	MYSQL_ROW row;
	
	/* Fetch the rows from Question_Bank table till end reaches */
	while ((row = mysql_fetch_row(result))) 
	{
		flag = INVALID_QUESTION;
      
		/* if question name matches */
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
	
	/* free the result set */
	mysql_free_result(result);
	
	if(INVALID_QUESTION == flag) /* Invalid Question name */
	{
		SRCCE_server_send_message(socket,"\nInvalid Question name...\n");
		
		/* add to log file */
		SRCCE_server_create_log("Invalid Question name...", "admin");
		
		/* close the connection with database  and free up memory */
		mysql_close(con);
		mysql_library_end();
	}
	else /* if valid Question name */
	{
		/* edit the test case file */
		SRCCE_server_send_message(socket,"\nValid Question name\n");
		
		/* add to log file */
		SRCCE_server_create_log("Valid Question name...", "admin");
		
		/*find the testcase file path */
		strcpy(question_path,"../../Question_Bank/");
		strcat(question_path,question_name);
		strcat(question_path,"/testcase");
		
		/* Send corresponding test case file */
		SRCCE_server_send_file(socket, question_path);
		
		/* get the edited  testcase file */
		SRCCE_server_receive_file(socket, question_path);
				
		/* send the expected output file */
		strcpy(question_path,"../../Question_Bank/");
		strcat(question_path,question_name);
		strcat(question_path,"/Expected_output");
		
		/* Send corresponding Expected_output file */
		SRCCE_server_send_file(socket, question_path);
		
		/* get the edited Expected_output file */
		SRCCE_server_receive_file(socket, question_path);
				
		/* updating Question bank for all registered user */
		if (mysql_query(con, "SELECT * FROM login_credentials")) 
		{
			SRCCE_server_finish_with_error(con);
		}
  
		MYSQL_RES *result = mysql_store_result(con);
		
		/* check for error */
		if (result == NULL) 
		{
    		SRCCE_server_finish_with_error(con);
		}

		MYSQL_ROW row;
		
		/* fetch the rows from user_credentials table until end of table */
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
		
		/* close the connection with the database and clean up memory */
		mysql_close(con);
		mysql_library_end();
		
		printf("\nQuestion name:%s ,Test Cases updated Successfully by admin...",question_name);
		fflush(stdout);
		
		/* add to log file */
		snprintf(log_msg, MSG, "Question name:%s ,Test Cases updated Successfully by admin...",question_name);
		SRCCE_server_create_log(log_msg, "admin");
	}
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_server_add_remove_testcase
 *
 * DESCRIPTION: Admin's choice is received and accordingly the update testcase 
 * function is called or he can exit from this menu.
 * 	
 * ARGUMENTS: socket: socket descriptor.
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_server_add_remove_testcase(int socket)
{
	
	/* 1 for update testcase 2 for exit */
	int choice = INVALID_CHOICE;
	
	/* Pointer variable to receive message from admin */
	char *recv_msg = NULL;
	
	printf("\nadmin entered add/remove testcase...\n");
	fflush(stdout);
	
	/* add to log file */
	SRCCE_server_create_log("admin entered add/remove testcase...", "admin");
	
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
			case ONE: 	SRCCE_server_update_testcase(socket);
						break;
					
			case TWO: 	printf("\nadmin exits from add/remove testcase.... ");
						fflush(stdout);
						/* add to log file */
						SRCCE_server_create_log("admin exits from add/remove testcase...", "admin");
						break;
					
			default: 	printf("\nInvalid choice entered by admin in add/remove testcase\n");
						fflush(stdout);
						/* add to log file */
						SRCCE_server_create_log("Invalid choice entered by admin in add/remove testcase", "admin");
		}
	}while(TWO != choice); /* End of do while */
}
/**************************************************************************************
*
*  FILE NAME	: SRCCE_server_add_remove_user.c
*
*  DESCRIPTION: This file contains functions for add remove user.
*
*************************************************************************************/

#include "SRCCE_server_header.h"

/***************************************************************************
 * FUNCTION NAME: SRCCE_server_display_user
 *
 * DESCRIPTION: Function to send the user credentials to the admin.
 * 	
 * ARGUMENTS: socket: socket descriptor.
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_server_display_user(int socket)
{
	/* variable to store the total number of users */
	int total_users = ZERO;
	
	/* string to store the user details */
	char userdetails[USER_DETAILS] = {ZERO};
	
	/* Pointer to send message to client */
	char *send_msg = NULL;
		
	printf("\nadmin entered display users...\n");
	fflush(stdout);
	
	/* add to the log file */
	SRCCE_server_create_log("admin entered display users..." , "admin");

	/* find total number of users in database */
	MYSQL *con = my_sql_init();
	
	/* enter the query and check for error */
	if (mysql_query(con, "SELECT COUNT(*) FROM login_credentials")) 
	{
		SRCCE_server_finish_with_error(con);
	}
  
	/* Take the find the first row and store in result set */
	MYSQL_RES *result = mysql_use_result(con);
	
	/* check for error */
	if (result == NULL) 
	{
    	SRCCE_server_finish_with_error(con);
	}
	
	/* fetch the the count of users */
	MYSQL_ROW row = mysql_fetch_row(result);

	/* check for error */
	if (NULL != row)
	{
		/* store the count in total_users */
		total_users = atoi(row[ZERO]);
	}
	
	/* send the total users to admin */
	send_msg = (char *)malloc(sizeof(char) * MSG);
	
	/* check for error while allocating memory */
	if( NULL == send_msg)
	{
		printf("\n Failed to allocate the memory to send_msg \n");
		SRCCE_server_create_log("Failed to allocate the memory to send_msg" , "admin"); 
		exit(EXIT_FAILURE);
	}
	
	snprintf(send_msg,MSG,"%d",total_users);
	SRCCE_server_send_message(socket,send_msg);
	free(send_msg);
	
	/* free the result set */ 
	mysql_free_result(result);
	
	/* if there a no users in data base */
	if(ZERO == total_users)
	{
		printf("\nThere are zero Users in Database...\n");
		fflush(stdout);
		
		/* add to log file */
		SRCCE_server_create_log("There are zero Users in Database..." , "admin");
	
		/* close the connection with database and free up memory */
		mysql_close(con);
		mysql_library_end();
	}
	else /* if total user != 0 */
	{
		/* add to log file */
		SRCCE_server_create_log("Sending user details to the admin..." , "admin");
		
		/* send the user details to admin */
		if (mysql_query(con, "SELECT * FROM login_credentials")) 
		{
			SRCCE_server_finish_with_error(con);
		}
		
		/* store the rows in a result set */
		result = mysql_use_result(con);
		
		/* check for error */
		if (result == NULL) 
		{
			SRCCE_server_finish_with_error(con);
		}
		
		/* fetch until end of table */
		while ((row = mysql_fetch_row(result))) 
		{
			strcpy(userdetails, "\n\t");
			strcat(userdetails, row[ZERO]);
			strcat(userdetails,"\t\t");
			strcat(userdetails,row[ONE]);
			strcat(userdetails,"\t\t");
			strcat(userdetails,row[TWO]);
			strcat(userdetails,"\n");
			SRCCE_server_send_message(socket, userdetails);
		}
		
		/* free the result set */
		mysql_free_result(result);
		
		/* close the connection with database and free up memory */
		mysql_close(con);
		mysql_library_end();
		
	}/* end of while */
}
/***************************************************************************
 * FUNCTION NAME: my_sql_init
 *
 * DESCRIPTION: This function makes the connection with sql and selects
 * database as SRCCE_database.
 *
 * ARGUMENTS: Nothing.
 *
 * RETURNS: An initialized MYSQL* handle. NULL if there was insufficient 
 * memory to allocate a new object.
 *    
***************************************************************************/
MYSQL *my_sql_init()
{
	/* initialize to connect with mysql */
	MYSQL *con = mysql_init(NULL);
  
	/* add to the log file */
	SRCCE_server_create_log("Making connection with the mysql..." , "Server");
	
	/* check for error */
	if (con == NULL)
	{
       	fprintf(stderr, "mysql_init() failed\n");
		
		/* add to the log file */
		SRCCE_server_create_log("Connection with the mysql Failed..." , "Server");
       	
		exit(EXIT_FAILURE);
	}  
  
	/* add to the log file */
	SRCCE_server_create_log("Making connection with the SRCCE_database..." , "Server");
	
	/* connect with SRCCE_database */
	if (mysql_real_connect(con, "localhost", "root", "abc123", "SRCCE_database", ZERO, NULL, ZERO) == NULL) 
	{
		SRCCE_server_finish_with_error(con);
	}   
	
	return con;
}   	
/***************************************************************************
 * FUNCTION NAME: SRCCE_server_add_user
 *
 * DESCRIPTION: Admin can add a user. Server receives the user's name and password
 * for the user. They are validated i.e. do they fit the criteria. If they fit
 * the criteria then the user directory is created and the user credentials 
 * are added to the database and an acknowledgement is sent to the admin.
 * If it do not fit then an acknowledgement is sent to the admin.
 *
 * ARGUMENTS: socket: socket descriptor.
 * 
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_server_add_user(int socket)
{
	/* flag variable to store if user exists
		or not 
		1 if exists
		0 if doesn't exists
	*/
	int flag = INVALID;
	
	/* variable to store if the user name is valid or not */
	int valid_user = INVALID;
	
	/* variable to store if the password is valid or not */
	int valid_pwd = INVALID;
	
	/* string vairable to store the SQL query */
	char query[MSG] = {ZERO};
	
	/* String to store the new user's name */
	char new_user[USERNAME_LEN] = {ZERO};
	
	/* String to store the new user's password */
	char new_pwd[PASSWORD_LENGTH] = {ZERO};
	
	/* String to store the log message */
	char log_msg[MSG] = {ZERO};
	
	/* this contains the path for user directory */
	char user_dir_path[PATH] = "../../users/";
	
	/* String to store the temp directory path */
	char temp_dir_path[PATH] = {ZERO};
	
	/* Command String */
	char command[COMMAND_LENGTH] = {ZERO};
	
	/* String to store the question bank path */
	char Question_path[PATH] = "../../Question_Bank";
	
	printf("\nAdmin entered add user...\n");
	fflush(stdout);
	
	/* add to log file */
	SRCCE_server_create_log("Admin entered add user...\n", "admin");
	
	/* receive the new user's name */
	SRCCE_server_receive_message(socket,new_user);
	printf("\nNew user name is %s", new_user);
	fflush(stdout);
	
	/* add to log file */
	snprintf(log_msg, MSG, "New user name is %s", new_user);
	SRCCE_server_create_log(log_msg, "admin");
	
	/* receive the new user's password */
	SRCCE_server_receive_message(socket,new_pwd);
		
	/* add to log file */
	snprintf(log_msg, MSG, "New user's Password is %s", new_pwd);
	SRCCE_server_create_log(log_msg, "admin");

	/* validate the password */
	valid_pwd = SRCCE_server_validate_password(new_pwd);
	
	/* validate the user name */
	valid_user = SRCCE_server_validate_username(new_user);
	
	printf("\nuser is %d pass is %d\n",valid_user, valid_pwd);
	fflush(stdout);
	
	/* if both user's name and password matches the criteria */
	if(valid_user == VALID && valid_pwd == VALID)
	{
		/* connect to SRCCE_database */
		MYSQL *con = my_sql_init();
  	   	
		/* run query and check for error */
		if (mysql_query(con, "SELECT * FROM login_credentials")) 
		{
			SRCCE_server_finish_with_error(con);
		}
  
		/* store the rows in result set */
		MYSQL_RES *result = mysql_use_result(con);
		
		/* Check for error */
		if (result == NULL) 
		{
      			SRCCE_server_finish_with_error(con);
		}

		MYSQL_ROW row;
		
		/* Fetch the rows until end of table reached */
		while ((row = mysql_fetch_row(result))) 
		{
			flag = INVALID ;
       
     		if(strcmp(new_user,row[ZERO])== ZERO )
			{
				flag = VALID;
				break;
			}
		}
		
		/* free the result set */
		mysql_free_result(result);
		
		/* If user found in database */
		if(flag == VALID)
		{
			SRCCE_server_send_message(socket,"User already exists...");
			
			/* add to log file */
			SRCCE_server_create_log("User already exists...", "admin");
			
			/* close the connection with mysql and free up memory */
			mysql_close(con);
			mysql_library_end();

		}
		else /* If user is not in the database */
		{
			/* enter the details in table */
			snprintf(query, MSG, "INSERT INTO login_credentials(username,password,login_status) VALUES ('%s','%s','%s')", new_user, new_pwd,"0");
			
			/* run the query and check for error */
			if (mysql_query(con, query))
			{
				SRCCE_server_finish_with_error(con);
			}
			
			/* send acknowledgement to admin */
			SRCCE_server_send_message(socket,"\nUser added successfully...");
			
			/* add to log file */
			SRCCE_server_create_log("User added successfully...", "admin");
			SRCCE_server_create_log("Create Directories for the user...", "admin");
			
			/* create directory / folder for this user */
			strcpy(temp_dir_path, user_dir_path);
			strcat(temp_dir_path,new_user);
			strcpy(command,"mkdir ");
			strcat(command, temp_dir_path);
			system(command);
			
			/* add to log file */
			SRCCE_server_create_log("Copying the Question Bank in user's directory...", "admin");
			
			/* code for copying the Question bank into each new user directory */
			strcat(temp_dir_path,"/");
			strcpy(command,"cp -r ");
			strcat(command, Question_path);
			strcat(command," ");
			strcat(command, temp_dir_path);
			system(command);
			
			/* close the connection with mysql and free up memory */
			mysql_close(con);
			mysql_library_end();
		}
		
	} /* end of if both username and password are valid */
	else /* if user's name or password or both doesn't matches the criteria */
	{
		SRCCE_server_send_message(socket,"\nInvalid...\nUsername should have user as prefix, maximum length is 25 and minimum 8.\nPassword should be alphanumeric and length should be atleast 8 and have atleast one upper case ,one lower case & one special character");
		
		/* add to log file */
		SRCCE_server_create_log("Invalid username or Password...", "admin");
	}
}

/***************************************************************************
 * FUNCTION NAME: SRCCE_server_remove_user
 *
 * DESCRIPTION: Server receives the user's name to be removed. It is validated. 
 * If it is correct then the user's directory is removed and user's credentials
 * are removed from the database and an acknowledgement is sent.
 * If it is incorrect then an acknowledgement is sent.
 * 	
 * ARGUMENTS: socket: socket descriptor.
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_server_remove_user(int socket)
{
	/* flag to check if user name exists or not */
	int flag = ZERO;

	/* query string */
	char query[MSG] = {ZERO};
	
	/* String to store user's name to be removed */
	char remove_user[USERNAME_LEN] = {ZERO};
	
	/* log message string */
	char log_msg[MSG] = {ZERO};
	
	/* this contains the path for user directory */
	char user_dir_path[PATH] = "../../users/";
	
	/* this contains the path for temp directory */
	char temp_dir_path[PATH] = {ZERO};
	
	/* Command string */
	char command[COMMAND_LENGTH] = {ZERO};
	
	printf("\nAdmin entered remove user...\n");
	fflush(stdout);
	
	/* add to log file */
	SRCCE_server_create_log("Admin entered remove user...", "admin");

	/* receive the user's name from admin */
	SRCCE_server_receive_message(socket,remove_user);
	
	/* connect to SRCCE_database */
	MYSQL *con = my_sql_init();
   	
	/* run the query and check for error */
	if (mysql_query(con, "SELECT * FROM login_credentials")) 
	{
		SRCCE_server_finish_with_error(con);
	}
  
	/* store in the result set */
	MYSQL_RES *result = mysql_use_result(con);
	
	/* Check for error */
	if (result == NULL) 
	{
      	SRCCE_server_finish_with_error(con);
	}

	MYSQL_ROW row;

	/* Fetch until end of table is reached */
	while ((row = mysql_fetch_row(result))) 
	{
		flag = INVALID ;
       
     	if((ZERO == strcmp(remove_user,row[ZERO])) && (ZERO != strcmp(row[ZERO],"admin")) )
		{
			flag = VALID;
			break;
		}
	}
	
	/* free the result set */
	mysql_free_result(result);
	
	/* if user's name exists */
	if(flag == VALID)
	{
		/*Enter the query */
		snprintf(query, MSG, "DELETE FROM login_credentials WHERE username = ('%s')", remove_user);
		
		/* Run the query and check for error */
		if (mysql_query(con, query))
		{
			SRCCE_server_finish_with_error(con);
		}
		
		/* send the acknowledgement to admin */
		SRCCE_server_send_message(socket,"\nUser deleted successfully...\n");
		printf("\nUser: %s deleted successfully by admin....\n", remove_user);
		
		/* add to log file */
		snprintf(log_msg, MSG, "User: %s deleted successfully by admin....\n", remove_user);
		SRCCE_server_create_log(log_msg, "admin");
		fflush(stdout);
		
		/* add to log file */
		SRCCE_server_create_log("Removing user's directory...", "admin");
		
		/* delete the user directory */
		strcpy(command,"rm -rf ");
		strcpy(temp_dir_path, user_dir_path);
		strcat(temp_dir_path,remove_user);
		strcat(command, temp_dir_path);
		system(command);		
	}
	else
	{
		SRCCE_server_send_message(socket,"\nUser doesnt exist...\n");
		printf("\nUser: %s doesnt exist...\n", remove_user);
		fflush(stdout);
		
		/* add to log file */
		snprintf(log_msg, MSG, "User: %s doesnt exist...", remove_user);
		SRCCE_server_create_log(log_msg, "admin");
	}
	
	/* close the connection and free up memory */
	mysql_close(con);	
	mysql_library_end();
}

/***************************************************************************
 * FUNCTION NAME: SRCCE_server_add_remove_user
 *
 * DESCRIPTION: Server receives the choice to add or remove the user and to
 * exit from this menu.
 * 	
 * ARGUMENTS: socket: socket descriptor.
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_server_add_remove_user(int socket)
{
	/* 1 for add user 2 for remove user */
	int choice = ZERO;
	
	/* string pointer to store the received message */
	char *recv_msg = NULL;
	
	printf("\nAdmin entered add/remove user...\n");
	fflush(stdout);
	
	/* add to log file */
	SRCCE_server_create_log("Admin entered add/remove user...", "admin");
	
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
		
		/* select according to the choice */
		switch(choice)
		{
			case ONE: 	SRCCE_server_add_user(socket);
						break;
			
			case TWO: 	SRCCE_server_remove_user(socket);
						break;
			
			case THREE: SRCCE_server_display_user(socket);
						break;
			
			case FOUR: 	printf("\nAdmin exiting from add/remove user...\n");
						fflush(stdout);
						/* add to log file */
						SRCCE_server_create_log("Admin exiting from add/remove user...", "admin");
						break;
		}
	}while(FOUR != choice); /* End of do while */
}

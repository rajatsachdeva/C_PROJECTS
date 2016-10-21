/**************************************************************************************
*
*  FILE NAME	: SRCCE_server_login.c
*
*  DESCRIPTION: This file contains the files for logging the user in the SRCCE system.
*
*************************************************************************************/

#include "SRCCE_server_header.h"

/***************************************************************************
 * FUNCTION NAME: SRCCE_server_finish_with_error
 *
 * DESCRIPTION: This fucntions prints the error related to sql and exits 
 * the program.
 * 	
 * ARGUMENTS: con: MYSQL initialized handle.
 *
 * RETURNS: Nothing.
 *
***************************************************************************/
void SRCCE_server_finish_with_error(MYSQL *con)
{
	/* log message string */
	char log_msg[MSG] = {ZERO};
	
	fprintf(stderr, "%s\n", mysql_error(con));
  
	/* add to the log file */
	snprintf(log_msg, MSG,"error received: %s",mysql_error(con));
	SRCCE_server_create_log(log_msg , "Server");
  
	/* close the connection and free up memory */
	mysql_close(con);
	mysql_library_end();
  
	exit(EXIT_FAILURE);        
}

/***************************************************************************
 * FUNCTION NAME: SRCCE_server_login
 *
 * DESCRIPTION: This function checks if the user's name and password are
 * coorect or not. If they are correct then it's checked if it's already 
 * logged in or not. If it's not logged in then if user is admin it's checked
 * if users are currently working. If user is a normal user then it's checked 
 * if admin is working. After all contraints are passed then login status of 
 * of the user is changed as logged in.
 *
 * ARGUMENTS: username: name of the user.
 *			  passwd: password of the user.
 *
 * RETURNS: An integer value.
 *			0 inavlid details.  
 *			1 all contstraints passed. 				
 *			2 username(admin/user) username and password valid but already logged in.
 *			3 admin tries while users are logged in.
 *			4 user tries while admin is logeed in.	
 *	   
***************************************************************************/
int SRCCE_server_login(char *username, char *passwd)
{
	/* To take return value mysql_num_fields */
	int num_fields = ZERO;
	
	/* Type of user flag */
	int flag = INVALID_USER;
	
	/* login status string */
	char login_status[TWO] = {ZERO};
	
	/* Query string */
	char query[MSG] = {ZERO};
	
	/* Log message string */
	char log_msg[MSG] = {ZERO};
	
	/* add to the log file */
	SRCCE_server_create_log("logging the user..." , "Server");
	
	/* connect with mysql */	
	MYSQL *con = mysql_init(NULL);
  	
	/* Check for error */
	if (con == NULL)
	{
		fprintf(stderr, "mysql_init() failed\n");
		
		/* add to the log file */
		SRCCE_server_create_log("Connection with the mysql Failed..." , "Server");
		
		exit(EXIT_FAILURE);
	}  
	
	/* Select SRCCE_database */
	if (mysql_real_connect(con, "localhost", "root", "abc123", "SRCCE_database", 0, NULL, 0) == NULL) 
	{
		SRCCE_server_finish_with_error(con);
	} 
	
	/* add to the log file */
	SRCCE_server_create_log("Making connection with the SRCCE_database..." , "Server");
	
  	/*Take users credentials from table */ 
	if (mysql_query(con, "SELECT * FROM login_credentials")) 
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
	
	num_fields = mysql_num_fields(result);

	MYSQL_ROW row;
	
	/* fetch until end of table reached */
	while ((row = mysql_fetch_row(result))) 
  	{ 
		/* invaild details */
		flag = INVALID_USER;
       	
		/* check if the user name and password are correct */
		if(strcmp(username,row[ZERO])== ZERO && strcmp(passwd,row[ONE]) == ZERO )
		{
			/* same user problem */
			/* if user not logged in */
			if(ZERO == strcmp("0", row[TWO]))
			{
				flag = VALID_USER;
				break;
			}
			else/* if already logged in */
			{
				flag = LOGGED_IN;
				break;
			}
		}
		else /* if the details are incorrect */
		{
			flag = INVALID_USER;
		}
	}
	
	/* free the result set */
	mysql_free_result(result);
	
	/* valid login details and not already logged in */
	if(VALID_USER == flag)
	{
		/* if the user is admin */
		if( ZERO == strcmp(username,"admin"))
		{
			if (mysql_query(con, "SELECT * FROM login_credentials")) 
			{
				SRCCE_server_finish_with_error(con);
			}
				
			/* check if any other users are logged in */
			MYSQL_RES *result = mysql_use_result(con);
			
			/* Check for error */
			if (result == NULL) 
			{
				SRCCE_server_finish_with_error(con);
			}
	
			MYSQL_ROW row;
			
			/* fetch until end of table reached */
			while ((row = mysql_fetch_row(result))) 
			{
				if(ZERO == strcmp("admin",row[ZERO]))
				{
					continue;
				}
				else /* other users */
				{
					/* check if any user logged in */
					if(ZERO == strcmp("1",row[TWO]))
					{
						flag = USER_LOGGED_IN;
						break;
					}
					else /* if no user logged in */
					{
						flag = VALID_USER;
					}
				}
			}
			mysql_free_result(result);				
		}/* end of if user is admin */
		else /* if user */
		{
			if (mysql_query(con, "SELECT * FROM login_credentials")) 
			{	
				SRCCE_server_finish_with_error(con);
			}
					
			/* check if admin is logged in */
			MYSQL_RES *result = mysql_use_result(con);
				
			/* check for error */
			if (result == NULL) 
			{
				SRCCE_server_finish_with_error(con);
			}
	
			MYSQL_ROW row;
			
			/* Fetch until end of table reached */
			while ((row = mysql_fetch_row(result))) 
			{
				/* check for admin's login status*/
				if(ZERO == strcmp("admin",row[ZERO]))
				{
					if(ZERO == strcmp("1", row[TWO]))
					{
						flag = ADMIN_LOGGED_IN;
						break;
					}
				}
				else /* other users */
				{
					flag = VALID_USER;
				}
			}
			
			/* free the result */
			mysql_free_result(result);
		}/* end of else if user */
		
	}/* end of if 1 == flag */	
	
	if( VALID_USER == flag)	
	{	
		/* update the login status */
		strcpy(login_status,"1");
		snprintf(query, MSG, "UPDATE login_credentials SET login_status = '%s' WHERE username= '%s'", login_status, username);	
		
		/* add to log file */
		snprintf(log_msg, MSG,"Updating login_status = %s where username= %s ", login_status, username );
		SRCCE_server_create_log(log_msg , "Server");
		
		/*Run the query and check for error */
		if (mysql_query(con, query))
		{
			SRCCE_server_finish_with_error(con);
		}
	
	}
	/* close the connection and free up memory */
	mysql_close(con);
	mysql_library_end();
	
	return flag;
}
/**************************************************************************************
*
*  FILE NAME	: SRCCE_server_admin_process.c
*
*  DESCRIPTION: This file contains function for admin process.
*
*************************************************************************************/

#include "SRCCE_server_header.h"

/***************************************************************************
 * FUNCTION NAME: SRCCE_server_admin_process
 *
 * DESCRIPTION: Server receives the admin's choice and accordingly a function
 * is called.
 *
 * ARGUMENTS: socket: Socket descriptor.
 * 	
 * RETURNS: Nothing
 *	   
***************************************************************************/
void SRCCE_server_admin_process(int socket)
{
	/* String Pointer to store the received message */
	char *recv_msg = NULL;
	
	/* 1. add/remove user 
	   2. add/remove question
	   3. add/remove testcase
	   4. LOGOFF
	*/
	int admin_choice = ZERO;
	
	/* String variable to store the SQL query */
	char query[MSG] = {ZERO};
	
	/* Variable to store the login status of an admin */
	char login_status[TWO] = {ZERO};
	
	/* String varaible to store thread id */
	char thread_id[10] = {ZERO};
	
	/* Connect to the SRCCE_database */
	MYSQL *con = my_sql_init();
	
	/* get the thread id */
	snprintf(thread_id, 10, "%ld", syscall(SYS_gettid));
	
	/* enter the query to add thread id of admin in system */
	snprintf(query, MSG, "UPDATE login_credentials SET thread_id = '%s' WHERE username= '%s'", thread_id, "admin");	
	
	/* run the query and check for error */
	if (mysql_query(con, query))
	{
		SRCCE_server_finish_with_error(con);
	}
	fflush(stdout);
	
	/* close the connection and free up memory */
	mysql_close(con);
	mysql_library_end();
	
	/* Add to the log file */
	SRCCE_server_create_log("Inserting the thread id for admin in database..." , "admin"); 
	
	/* Take the admin to Main menu */
	do
	{
		/* Receive the choice from admin(client) */
		recv_msg = (char *)malloc(sizeof(char) * MSG);
		
		/* check for error while allocating memory */
		if( NULL == recv_msg)
		{
			printf("\n Failed to allocate the memory to recv_msg \n");
			SRCCE_server_create_log("Failed to allocate the memory to recv_msg" , "admin"); 
			exit(EXIT_FAILURE);
		}
		SRCCE_server_receive_message(socket,recv_msg);
		admin_choice = atoi(recv_msg);
		fflush(stdout);
		free(recv_msg);
		
		/* select function according to admin's choice */
		switch(admin_choice)
		{
			case ONE:	 SRCCE_server_add_remove_user(socket);
						break;
					
			case TWO: 	SRCCE_server_add_remove_question(socket);
						break;
						
			case THREE: SRCCE_server_add_remove_testcase(socket);
						break;
					
			case FOUR:	printf("\nAdmin LOGOFF...\n");
						SRCCE_server_create_log("Admin LOGOFF..." , "admin"); 
					
						/* update the login status as Log off */
						MYSQL *con = my_sql_init();
						strcpy(login_status,"0");
						snprintf(query, MSG, "UPDATE login_credentials SET login_status = '%s', thread_id = 'NULL' WHERE username= '%s'", login_status, "admin");	
						if (mysql_query(con, query))
						{
							SRCCE_server_finish_with_error(con);
						}
						fflush(stdout);
						mysql_close(con);
						mysql_library_end();
						sleep(2);
					
						/* add to the log file */
						SRCCE_server_create_log("\nUpdating login status for the admin" , "admin"); 
						break;
					
			default: 	printf("\ninvalid choice by admin...\n ");
						SRCCE_server_create_log("invalid choice by admin..." , "admin"); 
						fflush(stdout);
		}
				
	}while(admin_choice != FOUR);/* End of do while */
}
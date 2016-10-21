/**************************************************************************************
*
*  FILE NAME	: SRCCE_server_utilities.c
*
*  DESCRIPTION: This file contains various utilities functions.
*
*
*************************************************************************************/

#include "SRCCE_server_header.h"

/***************************************************************************
 * FUNCTION NAME: client_handler
 *
 * DESCRIPTION: This is client handler which serves each client's request
 * It accepts the user name and password from the client and calls the 
 * user's process or admin's process according to type of user.
 * 	
 * ARGUMENTS: socket: pointer to socket descriptor.
 *
 * RETURNS: It returns a void pointer.
 *	   
***************************************************************************/
void *SRCCE_client_handler(void *socket)
{
	/* function marks the thread identified by thread
       as detached */
	pthread_detach(pthread_self());
	
	/* string vairable to store the log file message */
	char log_msg[MSG] = {ZERO};
		
	/* sned and receive message string pointers */
	char *recv_msg = NULL;
	char *send_msg = NULL;
	
	/* variable to store if the username is correct 
	and present in the database or not */
	int valid_user = ZERO;
	
	/* 1 if admin 5 if user*/
	int user_type = ZERO;
		
	/* string to store the username and password */
	char username[USERNAME_LEN] = {ZERO};
	char password[PASSWORD_LENGTH] = {ZERO};
	
	/* dereference the socket variable assign to socket_client */
	int socket_client = *(int *)socket;
	
	/* Add in to log file */
	printf("\nThread id is :%ld", syscall(SYS_gettid));
	snprintf(log_msg, MSG, "Thread id is %ld",syscall(SYS_gettid));
	SRCCE_server_create_log( log_msg, "SERVER"); 
	fflush(stdout);
	
	while(TRUE)
	{
		/* send the welcome message to the client */
		SRCCE_server_send_message(socket_client,"\nWelcome to SRCCE System\n");
	
		/* allocate the memory ro recv_msg */
		recv_msg = (char *)malloc(sizeof(char) * MSG);
		
		/* check for error while allocating memory */
		if( NULL == recv_msg)
		{
			printf("\n Failed to allocate the memory to recv_msg \n");
			SRCCE_server_create_log("Failed to allocate the memory to recv_msg" , "SERVER"); 
			exit(EXIT_FAILURE);
		}
		
		/* receive the i got your message */		
		SRCCE_server_receive_message(socket_client,recv_msg);
		printf(" %s ",recv_msg);
		fflush(stdout);
		
		/* add to log file */
		SRCCE_server_create_log( recv_msg, "SERVER");
		free(recv_msg);
		
		/* take user credentials from client */
		do
		{
			/* take user name from client */
			SRCCE_server_send_message(socket_client,"\nEnter username: ");
		
			/* receive and print the user name */
			SRCCE_server_receive_message(socket_client,username);
			printf("\nuser name is :%s\n",username);
			fflush(stdout);
			
			/* add to log file */
			snprintf(log_msg, MSG, "user name is %s\n",username);
			SRCCE_server_create_log( log_msg, "SERVER"); 
			
			/* take password from client */
			SRCCE_server_receive_message(socket_client,password);
						
			/* add to log file */
			snprintf(log_msg, MSG, "password is %s\n",password);
			SRCCE_server_create_log( log_msg, "SERVER"); 
			
			/* check for the type of user */
			valid_user = SRCCE_server_login(username, password);
			printf("\nvalid user is %d",valid_user);
			fflush(stdout);
		
			/* add to log file */
			snprintf(log_msg, MSG, "valid user is %d\n",valid_user);
			SRCCE_server_create_log( log_msg, "SERVER"); 
			
			/* If user has valid details and all constraints passed */
			if( VALID_USER == valid_user)
			{
				if(ZERO == strcmp("admin",username))
				{
					/* admin */
					user_type = IS_ADMIN;
				}
				else
				{	
					/* user */
					user_type = IS_USER;
				}
			}
			
			/* invalid login details */
			else
			{
				user_type = valid_user;
			}
							
			printf("\nuser type is %d",user_type);
			fflush(stdout);
			
			/* add to log file */
			snprintf(log_msg, MSG, "user type is %d\n",user_type);
			SRCCE_server_create_log( log_msg, "SERVER"); 
			
			/* send the user_type to client */
			send_msg = (char *)malloc(sizeof(char) * MSG);
			
			/* check for error while allocating memory */
			if( NULL == send_msg)
			{
				printf("\n Failed to allocate the memory to send_msg \n");
				SRCCE_server_create_log("Failed to allocate the memory to send_msg" , "SERVER"); 
				exit(EXIT_FAILURE);
			}
			
			snprintf(send_msg,MSG,"%d",user_type);
			SRCCE_server_send_message(socket_client,send_msg);
			free(send_msg);
					
		}while(( INVALID_USER == user_type) || (LOGGED_IN == user_type )|| (USER_LOGGED_IN == user_type) || (ADMIN_LOGGED_IN == user_type));
		/* end of do while loop */

		
		/* if admin */
		if( IS_ADMIN == user_type)
		{
			printf("\ncalling admin process...");
			fflush(stdout);
			
			/* add to log file */
			SRCCE_server_create_log("calling admin process..." , "admin"); 
	
			/* call the admin process */
			SRCCE_server_admin_process(socket_client);
			
			/* exit the thread after logging off */
			pthread_exit(NULL);
			
			/* close the socket */
			close(socket_client);
			
		}
		
		/* if user */
		if( IS_USER == user_type)
		{
			printf("\ncalling user process...");
			fflush(stdout);
			
			/* add to log file */
			SRCCE_server_create_log("calling user process..." , username); 
			
			/* call the user function */
			SRCCE_server_user_process(socket_client, username);
			
			/* exit the thread */
			pthread_exit(NULL);
			
			/* close the socket */
			close(socket_client);
		}
	}/* end of while */
}	
/***************************************************************************
 * FUNCTION NAME: SRCCE_server_initialize
 *
 * DESCRIPTION: Takes the server's IP address and port number from a config 
 *	file.
 * 
 * ARGURMENTS: ip: string to store the IP address.
 *			   port: string to store the port number.
 * 
 * RETURNS: Nothing
 *			
***************************************************************************/
void SRCCE_server_initialize(char* ip, char *port)
{
	/* creating file pointer */
	FILE *fp_config = NULL;
	
	/* open the file in read mode */
	
	fp_config = fopen("../config/config.ini","r");
	
	/* check for error */
	if(NULL == fp_config)
	{
		printf("Failed to open the config file\n");
		
		exit(EXIT_FAILURE);
	}
	
	/* reading IP address */
	fscanf(fp_config, "%*[^0123456789]%[^\n]", ip);
	
	/* reading port number */
	fscanf(fp_config, "%*[^0123456789]%[^\n]", port);
	
	/* close the file */
	fclose(fp_config);
	
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_server_sigint_handler
 *
 * DESCRIPTION: This functions detects if the code program was exited using
 * CTRL + C or CTRL + Z.
 *
 * ARGUMENTS: signum: signal number.
 * 
 * RETURNS: Nothing
 *			
***************************************************************************/
void SRCCE_server_sigint_handler(int sig_num)
{
    /* Reset handler to catch SIGINT. */
	if( sig_num == SIGINT )
   	{
		printf("\nTerminated using Ctrl+C \n");
	}
    /* Reset handler to catch SIGINT. */
	else
	{
   		printf("\nStopped using Ctrl+Z \n");
	}
	fflush(stdout);
	exit(EXIT_SUCCESS);
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_server_get_date_time
 *
 * DESCRIPTION: This function finds the current date and time of the system.
 * 	
 * ARGUMENTS: Nothing.
 *
 * RETURNS: Pointer to date time string.
 *
***************************************************************************/
char* SRCCE_server_get_date_time()
{
	/* buffer to store date and time */
	char *datetime = (char *)malloc(sizeof(char) * MSG);
	
	/* check for error while allocating memory */
	if( NULL == datetime)
	{
		printf("\n Failed to allocate the memory to datetime \n");
		exit(EXIT_FAILURE);
	}
	
	/* get current time */
	time_t t = time(NULL);

	/* convert time into localtime */
	struct tm *time = localtime(&t);

	/* format time according to RFC */
	strftime(datetime, MSG, "%a, %d %b %Y %T %z", time);
	
	return datetime;
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_server_create_log
 *
 * DESCRIPTION: This function adds the given message to the log file.
 *
 * ARGUMENTS: message: Message to be added to the log file.
 *			  username: Name of the user.
 * 
 * RETURNS: Nothing.
 *
***************************************************************************/
void SRCCE_server_create_log( char *message, char *username) 
{
	/* logging file */
	char log_file_path[MSG];
	
	FILE *p_log_fp = NULL;
	
	char *datetime = NULL;
	
	char *log = (char *)malloc(sizeof(char) * MSG);
	
	/* check for error while allocating memory */
	if( NULL == log)
	{
		printf("\nFailed to allocate the memory to log\n");
		exit(EXIT_FAILURE);
	}
	
	strcpy(log_file_path,"../log/");
	strcat(log_file_path,"server_log_file.txt");
	
	p_log_fp = fopen(log_file_path,"a");
	
	if (NULL == p_log_fp)			
	{
		fprintf(stderr,"Error in Opening Log File...\n");
		exit(EXIT_FAILURE);
	}

	/* log data */
    datetime = SRCCE_server_get_date_time();
	
	strcpy(log, datetime);
	strcat(log,"  username: ");
	strcat(log, username);
	strcat(log," MESSAGE: ");
	strcat(log, message);
	strcat(log," \n");

	/* writes the data into log file */
    fprintf(p_log_fp, "%s", log);
    fflush(p_log_fp);
	free(datetime);
	free(log);
	
    /* close the file */
    if (EOF == fclose(p_log_fp))
	{
       		fprintf(stderr,"Error in Closing Server log file...\n");
       		exit(EXIT_FAILURE);
   	}
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_server_validate_password
 *
 * DESCRIPTION: This functions checks whether the entered password string 
 *	follows the required conditions or not. 
 * 	
 * ARGUMENTS: password: password string to be checked.
 * 
 * RETURNS: An integer value
 *			0 if it’s an invalid password
 *			1 if it’s a valid password
 *
***************************************************************************/
int SRCCE_server_validate_password(char *password)
{
	int valid_pwd = ZERO;
	int pass_len = ZERO;
	int Upper_Case = ZERO;
	int Lower_Case = ZERO; 
	int Numeric_Character = ZERO; 
	int Special_Character = ZERO;
	int index = ZERO;
	
	/* get the length of the password */
	pass_len = strlen(password);
	
	/* validate the password */
	if( pass_len < 8 )
	{
		valid_pwd = INVALID;
	}
	else 
	{
		for(index = ZERO; index < pass_len; index++)
		{
			if( 'A' <= password[index] && 'Z' >=  password[index] )
			{
				Upper_Case = VALID;
			}
			else if( 'a' <= password[index] && 'z' >= password[index] )
			{
				Lower_Case = VALID;
			}
			else if( '0' <= password[index] && '9' >=  password[index] )
			{
				Numeric_Character = VALID;
			}
			else
			{
				Special_Character = VALID;
			}
		}
			
		if( Upper_Case && Lower_Case && Numeric_Character && Special_Character)
		{
			valid_pwd = VALID;		
		}
		else
		{
			valid_pwd = INVALID;	
		}
	}
	return valid_pwd;
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_server_validate_username
 *
 * DESCRIPTION: This function checks if the user's name follows a certain
 * criteria or not.
 * 
 * ARGUMENTS: username: user name string to be checked.
 * 	
 * RETURNS: 1 if user name is valid.
 *	   		0 if user name is invalid.
 *
***************************************************************************/
int SRCCE_server_validate_username(char *username)
{
	char user_prefix[] = "user"; 
	int valid_user = ZERO;
	int index = ZERO;
	
	if(strlen(username) > 4) 
	{ 
		for(index = ZERO; index < 4; index++) 
        { 
          	if(username[index] != user_prefix[index]) 
			{				
				valid_user = INVALID;
				break; 
			}
			valid_user = VALID;
        } 
    } 
    else 
    { 
        valid_user = INVALID; 
    }
	
    if(VALID == valid_user)
	{
		while('\0' != username[index] )
		{
			if(ZERO == isdigit(username[index]))
			{
				valid_user = INVALID;
				break;
			}
			index++;
		}
	}
	return valid_user;
}


/***************************************************************************
 * FUNCTION NAME: SRCCE_server_encrypt
 *
 * DESCRIPTION: Takes an input file and creates an encrypted version of the
 * input file.
 * 	
 * ARGUMENTS: filename: Input file name.
 *			  new_filename: Encrypted file name.
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_server_encrypt( char *filename, char *new_filename)
{    
	FILE *fp1 = NULL;
	FILE *fp2 = NULL;
	int key = CIPHER;
	char val = ZERO;
	char ch = ZERO;
	
	char log_msg[MSG] = {ZERO};

	/* open the given file in read mode */
	fp1 = fopen(filename, "r");
	
	/* error handling */
	if (NULL == fp1) 
	{
		printf("\nUnable to open the file: %s\n",filename);
		fflush(stdout);
		
		/* add to log file */
		snprintf(log_msg, MSG, "Unable to open the file: %s",filename);
		SRCCE_server_create_log(log_msg , "user"); 
		
		exit(EXIT_FAILURE);
	}

	/* open the temporary file in write mode */
	fp2 = fopen(new_filename, "w");
	
	/* error handling */
	if (NULL ==fp2) 
	{
		printf("\nUnable to open the file: %s\n",new_filename);
		fflush(stdout);
		
		/* add to log file */
		snprintf(log_msg, MSG, "Unable to open the file: %s",new_filename);
		SRCCE_server_create_log(log_msg , "user"); 
		
		exit(EXIT_FAILURE);
    }

	/* converting plain text to cipher text */
	while ((ch = fgetc(fp1)) != EOF) 
	{
		/* adding key to the ascii value of the fetched char */
		val = ch + key;
		fputc(val,fp2);
	}

	/* closing all opened files */
	fclose(fp1);
	fclose(fp2);
}

/***************************************************************************
 * FUNCTION NAME: SRCCE_server_decrypt
 *
 * DESCRIPTION: Takes an input encrypted file and decrypts the file and create
 * a new file without any encryption.
 * 	
 * ARGUMENTS: filename: Input file name.
 * 			  decrypt_file: Output decrypted file name.
 * 
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_server_decrypt( char *filename, char *decrypt_file)
{
	int key = CIPHER;
	char ch = ZERO;
	char val = ZERO;
	FILE *fp1 = NULL;
	FILE *fp2 = NULL;
	
	char log_msg[MSG] = {ZERO};
	
	/* open the file containing cipher text in read mode */
	fp1 = fopen(filename, "r");
	
	/* error handling */
	if (NULL == fp1) 
	{
		printf("\nUnable to open the file: %s\n",filename);
		fflush(stdout);
		
		/* add to log file */
		snprintf(log_msg, MSG, "Unable to open the file: %s",filename);
		SRCCE_server_create_log(log_msg , "user"); 
		
		exit(EXIT_FAILURE);
	}
		
	fp2 = fopen(decrypt_file, "w");

	/* error handling */
	if (NULL == fp2) 
	{
		printf("\nUnable to open the file: %s\n",decrypt_file);
		fflush(stdout);
		
		/* add to log file */
		snprintf(log_msg, MSG, "Unable to open the file: %s",decrypt_file);
		SRCCE_server_create_log(log_msg , "user"); 
		
		exit(EXIT_FAILURE);
	}
	
    while ((ch = fgetc(fp1)) != EOF) 
	{
        if (!feof(fp1))
		{
            val = ch - key;
			
			fputc(val,fp2);
		}
	}
    
    /* close all opened files */
    fclose(fp1);
	fclose(fp2);
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_server_create_socket
 *
 * DESCRIPTION: Creates a socket according to the family , type and protocol. 
 *
 * ARGUEMENTS: socket_in: Socket descriptor.
 *			   family: Socket family.
 *			   type: Type of socket.
 * 			   protocol: Type of protocol.
 * 				
 * RETURNS: On successful creation of the socket it returns the socket descriptor.
 *	   
***************************************************************************/
/* wrapper function for create socket command */
int SRCCE_server_create_socket(int socket_in, int family, int type, int protocol)
{
	/* create socket */
	socket_in = socket(family, type, protocol);
	
	/* check for error */
	if(NEG == socket_in)
	{
		printf("\nFailed to create the socket\n");
		SRCCE_server_create_log("Failed to create the socket", "SERVER"); 
		exit(EXIT_FAILURE);
	}
	
	printf("\nSocket created...\n");
	SRCCE_server_create_log("Socket created...", "SERVER"); 
	fflush(stdout);	
	
	return socket_in;
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_server_check_filesize
 *
 * DESCRIPTION: Checks if the file size if less than 64KB or not.
 * 
 * ARGUEMENTS: filename: name of the file.
 *
 * RETURNS: It returns an interger value.
 * 			0  if greater than 64KB
 * 			1  if less than or equal to 64KB
 *	   
***************************************************************************/
int SRCCE_server_check_filesize(char *filename)
{
	/* declare file pointer */
	FILE *fp = NULL;
	
	/* size flag */
	int flag  = ZERO;
	
	/* variable to store the size of the file */
	int file_size = ZERO;
		
	char log_msg[MSG] = {ZERO};
	
	/* Open the required file in read mode */
	fp = fopen(filename, "r");
	
	/* check for error */
	if( NULL == fp)
	{
		printf("\n Unable to open the file: %s", filename);
		
		/* add to log file */
		snprintf(log_msg, MSG, "Unable to open the file: %s",filename);
		SRCCE_server_create_log(log_msg , "SERVER");
		
		return flag;
	}
	
	/* Get the size of the file */
	fseek(fp, ZERO, SEEK_END);
	file_size = ftell(fp);
	printf("\nFILE SIZE is %d",file_size);
	fflush(stdout);
	
	/* add to log file */
	snprintf(log_msg, MSG, "FILE SIZE is %d",file_size);
	SRCCE_server_create_log(log_msg , "SERVER");
	
	/* close the file */
	fclose(fp);
	
	/* check if file_size is less than equal to FILESIZE */
	if( file_size <= FILESIZE)
	{
		/* valid file size */
		flag = VALID;
	}
	else
	{
		/* invalid file size */
		flag = INVALID;
	}
	
	return flag;	
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_server_send_message
 *
 * DESCRIPTION: It sends the required string message over the socket. 
 * 	
 * ARGUEMENTS: socket: Socket descriptor.
 *			   message: Message to string to send.
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_server_send_message(int socket, char *message)
{
	int length = ZERO;
	int temp = ZERO;
	
	int n = ZERO;
	
	char log_msg[MSG] = {ZERO};
	
	/* find the length of the message */
	length = strlen(message);
	
	/* convert the length to network byte order */
	temp = htonl(length);
		
	/* send length of the message */
	n = write(socket, &temp, INT_LEN);
		
	/* check for error */
	if( n < ZERO )
	{
		printf("\n %d: length not send \n",length);
			
		/* add to log file */
		snprintf(log_msg, MSG, "%d: message not sent \n",length);
		SRCCE_server_create_log(log_msg , "SERVER");
			
		exit(EXIT_FAILURE);
	}
		
	/* send the message */
	n = write(socket, message, length);
	
	/* check for error */
	if( (n != length) || (n < ZERO))
	{
		printf("\n %s: message not send \n",message);
		
		/* add to log file */
		snprintf(log_msg, MSG, "%s: message not sent \n",message);
		SRCCE_server_create_log(log_msg , "SERVER");
			
		exit(EXIT_FAILURE);
	}
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_server_receive_message
 *
 * DESCRIPTION: Receives the message from the socket and stores it in a string
 * buffer.
 *
 * ARGUEMENTS: socket: Socket descriptor.
 *			   message: String pointer to store the received message.
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_server_receive_message(int socket, char *message)
{
	int temp = ZERO;
	int size = ZERO;
	int n = ZERO;
	char thread_id[10] = {ZERO};
	char query[MSG] = {ZERO};
	char username[USERNAME_LEN] = {ZERO};
	char log_msg[MSG] = {ZERO};
	
	/* read length of the message */
	n = read(socket, &temp, sizeof(temp));
	size = ntohl(temp);
	
	/* check for error */
	if( n < ZERO )
	{
		printf("\nlength not received successfully\n");
		
		SRCCE_server_create_log( "length not received successfully", "SERVER");
		
		exit(EXIT_FAILURE);
	}
		
		
	/* read the message string */
	n = read(socket, message, size);
	message[size] = '\0';
	
	/* check for error */
	if( n < ZERO )
	{
		printf("\nmessage not received successfully\n");
		
		SRCCE_server_create_log( "message not received successfully", "SERVER");
		
		exit(EXIT_FAILURE);
	}
	
	if( n == ZERO)
	{
		
		MYSQL *con = my_sql_init();
		
		snprintf(thread_id, 10, "%ld", syscall(SYS_gettid));
		
		snprintf(query, MSG, "SELECT username FROM login_credentials WHERE thread_id= '%s'", thread_id);	
		if (mysql_query(con, query))
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
			strcpy(username, row[ZERO]);
		}
		
		mysql_free_result(result);
		
		snprintf(query, MSG, "UPDATE login_credentials SET login_status = '0', thread_id = 'NULL' WHERE thread_id= '%s'", thread_id);	
		
		if (mysql_query(con, query))
		{
			SRCCE_server_finish_with_error(con);
		}
		
		
		printf("\n%s was shutdown...", username);
		fflush(stdout);
		
		/* add to log file */
		snprintf(log_msg, MSG, "%s was shutdown...", username);
		SRCCE_server_create_log( log_msg, "SERVER");
		
		mysql_close(con);
		mysql_library_end();
		
		pthread_exit(NULL);
	}
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_server_send_file
 *
 * DESCRIPTION: Encrypt and send a file over the socket. 
 *
 * ARGUEMENTS:  socket: Socket descriptor.
 *				filename: naem of the file to send. 
 * 	
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_server_send_file(int socket, char *filename)		
{	
	int fd = ZERO;
	int filesize = ZERO;
	int temp = ZERO;
	int bytesread = ZERO;
	int n = ZERO;
	char send_buffer[5*FILESIZE] = {ZERO};
	char log_msg[MSG] ={ZERO};
	
	/* Declare file pointer */
	FILE *fp = NULL;
	
	SRCCE_server_encrypt(filename,"encrypted.txt");
	
	
	/* open the file using file descriptor */
	fd = open("encrypted.txt", O_RDONLY);
	
	/* check for error */
	if (fd < ZERO )
	{
		printf("\nError opening file %s\n",filename);
		
		/* add to log file */
		snprintf(log_msg, MSG, "Error opening file %s",filename);
		SRCCE_server_create_log(log_msg , "SERVER");
		
		exit(EXIT_FAILURE);			
	}
	
	/* open the file using file pointer */
	fp = fopen("encrypted.txt","r");
	
	/* check for error */
	if(NULL == fp)
	{
		printf("\nError opening file %s\n","encrypted.txt");
		
		/* add to log file */
		snprintf(log_msg, MSG, "Error opening file %s","encrypted.txt");
		SRCCE_server_create_log(log_msg , "SERVER");
		
		exit(EXIT_FAILURE);			
	}
	
	/* find the size of file */
	fseek(fp, ZERO, SEEK_END);
	filesize = ftell(fp);
	printf("Sending File of size: %d\n",filesize);
	fflush(stdout);
	
	/* add to log file */
	snprintf(log_msg, MSG, "Sending File of size: %d\n",filesize);
	SRCCE_server_create_log(log_msg , "SERVER");
	
	/* Now point to start of the file */
	rewind(fp);
	temp = htonl(filesize);
	
	/*send file size */
	n = write(socket, &temp, INT_LEN);
	
	/* check for error */
	if( n < ZERO )
	{
		printf("\nfailed to send the file size\n");
		
		/* add to log file */
		SRCCE_server_create_log("failed to send the file size" , "SERVER");
		
		exit(EXIT_FAILURE);
	}
	
	/* send the file over the socket */
	while ( (bytesread=read(fd , send_buffer , filesize)) > ZERO )
	{
		n = write(socket , send_buffer , bytesread);
		
		/* check for error */
		if(n != bytesread)
		{
			printf("\nfailed to send whole file\n");
			
			/* add to log file */
			SRCCE_server_create_log("failed to send whole file" , "SERVER");
			
			exit(EXIT_FAILURE);
		}
	}
	
	/* remove the Encrypted file */
	system("rm ./encrypted.txt &> error.txt");
	
	/* close the file */
	fclose(fp);
	close(fd);	
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_server_receive_file
 *
 * DESCRIPTION: Receives a file and SRCCE_server_decrypt it.
 * 	
 * ARGUMENTS: socket: Socket descriptor.
 * 			  filename: name of the to file to receive.
 *
 * RETURNS: An integer value
 * 			0 if FAILED
 * 			1 if SUCCESS
 *	   
***************************************************************************/
int SRCCE_server_receive_file(int socket, char *filename)		
{	
	char buffer[FILESIZE] = {ZERO};
	int bytesread = ZERO;
	int flag = ZERO;
	int n = ZERO;
	int temp = ZERO;
	int size = ZERO;
	int fd = ZERO;
	int total = ZERO;
	
	/* open file using file descriptor */
	fd = open("temp_encrypted.c", O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
	fd = open("temp_encrypted.c", O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
		
	/* check for error */
	if (fd < ZERO )
	{
		printf("\nError opening file %s\n","temp_encrypted.c");
		
		/* add to log file */
		SRCCE_server_create_log("Error opening file: temp_encrypted.c" , "SERVER");
		
		exit(EXIT_FAILURE);			
	}
	
	/* read length of file from socket */
	n = read(socket, &temp, sizeof(temp));
	
	/* check for error */
	if (n < ZERO) 
	{
		printf("ERROR reading from socket");
		fflush(stdout);
		
		/* add to log file */
		SRCCE_server_create_log("ERROR reading from socket" , "SERVER");
		
		return flag;
	}	
	
	size = ntohl(temp);
	
	if( ZERO == size)
	{
		printf("\nFILE SIZE is 0");
		fflush(stdout);
		
		return flag;
	}
	
	/* read from socket and write in file */
	while ( total < size )
	{
		bytesread = read(socket , buffer , 255 );
		
		/*write the contents of buffer in file */
		n = write(fd, buffer , bytesread );
		
		/* check for error */
		if (n != bytesread)
		{
			printf("File write Error \n");
			fflush(stdout);
			
			/* add to log file */
			SRCCE_server_create_log("File write Error" , "SERVER");
			
			return flag;
		}
		total = total + bytesread;
		if (total == size)
		{
			printf ("File written successfully \n");
			fflush(stdout);
			
			/* add to log file */
			SRCCE_server_create_log("File written successfully" , "SERVER");
			
			flag = VALID;
			break;
		}
	}

	close(fd);
	SRCCE_server_decrypt("temp_encrypted.c",filename);
	
	/* remove the temp file */
	system("rm temp_encrypted.c &> error.txt");
	return flag;
}

/***************************************************************************
 * FUNCTION NAME: SRCCE_server_send_file_user
 *
 * DESCRIPTION: Encrypt and send a file over the socket.  
 * 	
 * ARGUMENTS: socket: Socket descriptor.
 * 			  filename: file's name to send.
 *			  username: name of the user whose file is sent.
 *  		  question: name of question whose file is sent.
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_server_send_file_user(int socket, char *filename, char *username, char *question)		
{	
	int fd = ZERO;
	int filesize = ZERO;
	int temp = ZERO;
	int bytesread = ZERO;
	int n = ZERO;
	char send_buffer[5 * FILESIZE] = {ZERO};
	char log_msg[MSG] = {ZERO};
	
	/* Declare file pointer */
	FILE *fp = NULL;
	
	char user_dir_path[PATH] = "../../users/";
	char temp_dir_path[PATH] = {ZERO};
	char encrypt_file_path[PATH] = {ZERO};
	
	/* find the question folder in corresponding user */
	strcat(user_dir_path, username);
	strcat(user_dir_path,"/Question_Bank/");
	strcat(user_dir_path,question);
	strcat(user_dir_path,"/");
	
	/* find the input file path for user */
	strcpy(temp_dir_path, user_dir_path);
	strcat(temp_dir_path,filename);
	
	/* create the encrypted file path for user */
	strcpy(encrypt_file_path,user_dir_path);
	strcat(encrypt_file_path,"encrypted.txt");
	
	SRCCE_server_encrypt(temp_dir_path,encrypt_file_path);
	
	/* open the file using file descriptor */
	fd = open(encrypt_file_path, O_RDONLY);
	
	/* check for error */
	if (fd < ZERO )
	{
		printf("\nError opening file %s\n",encrypt_file_path);
		fflush(stdout);
		
		/* add to log file */
		snprintf(log_msg, MSG, "Error opening file %s\n",encrypt_file_path);
		SRCCE_server_create_log(log_msg , "SERVER");
		
		exit(EXIT_FAILURE);			
	}
	
	/* open the file using file pointer */
	fp = fopen(encrypt_file_path,"r");

	/* check for error */
	if(NULL == fp)
	{
		printf("\n Error opening file %s\n",encrypt_file_path);
		fflush(stdout);
		
		/* add to log file */
		snprintf(log_msg, MSG, "Error opening file %s\n",encrypt_file_path);
		SRCCE_server_create_log(log_msg , "SERVER");	
			
		exit(EXIT_FAILURE);			
	}
	
	/* find the size of file */
	fseek(fp, ZERO, SEEK_END);
	filesize = ftell(fp);
	printf("Sending File of size: %d\n",filesize);
	fflush(stdout);
	
	/* add to log file */
	snprintf(log_msg, MSG, "Sending File of size: %d\n",filesize);
	SRCCE_server_create_log(log_msg , "SERVER");
	
	/* Now point to start of the file */
	rewind(fp);
	temp = htonl(filesize);
	
	/*send file size */
	n = write(socket, &temp, INT_LEN);
	
	/* check for error */
	if( n < ZERO )
	{
		printf("\nfailed to send the file size\n");
		fflush(stdout);
	
		/* add to log file */
		SRCCE_server_create_log("failed to send the file size" , "SERVER");
		exit(EXIT_FAILURE);
	}
	
	/* send the file over the socket */
	while ( (bytesread=read(fd , send_buffer , filesize)) > ZERO )
	{
		n = write(socket , send_buffer , bytesread);
		
		/* check for error */
		if(n != bytesread)
		{
			printf("\nfailed to send whole file\n");
			fflush(stdout);
	
			/* add to log file */
			SRCCE_server_create_log("failed to send whole file" , "SERVER");
			
			exit(EXIT_FAILURE);
		}
	}
	
	/* close the file */
	fclose(fp);
	close(fd);	
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_server_receive_file_user
 *
 * DESCRIPTION: Receives a file and SRCCE_server_decrypt it.
 * 	
 * ARGUMENTS: socket: Socket descriptor.
 *			  filename: file's name to receive.
 * 			  username: name of the user whose file is received.
 * 			  question: question name whose file is received.
 * 
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_server_receive_file_user(int socket,	char *filename, char *username, char *question)		
{	
	char buffer[FILESIZE] = {ZERO};
	int bytesread = ZERO;
	int n = ZERO;
	int temp = ZERO;
	int size = ZERO;
	int fd = ZERO;
	int total = ZERO;
	
	char user_dir_path[PATH] = "../../users/";
	char temp_dir_path[PATH] = {ZERO};
	char encrypt_file_path[PATH] = {ZERO};
	char log_msg[MSG] = {ZERO};
		
	/* find the question folder in corresponding user */
	strcat(user_dir_path, username);
	strcat(user_dir_path,"/Question_Bank/");
	strcat(user_dir_path,question);
	strcat(user_dir_path,"/");
	
	/* find the final output file path for user */
	strcpy(temp_dir_path, user_dir_path);
	strcat(temp_dir_path,filename);
	
	/* create the encrypted file path for user */
	strcpy(encrypt_file_path,user_dir_path);
	strcat(encrypt_file_path,"recv_encrypted.txt");
	
	
	/* open file using file descriptor */
	fd = open(encrypt_file_path, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
	
	/* check for error */
	if (fd < ZERO )
	{
		printf("\n Error opening file %s\n",encrypt_file_path);
		fflush(stdout);
		
		/* add to log file */
		snprintf(log_msg, MSG, "Error opening file %s\n",encrypt_file_path);
		SRCCE_server_create_log(log_msg , "SERVER");
		
		exit(EXIT_FAILURE);			
	}
	
	/* read length of file from socket */
	n = read(socket, &temp, sizeof(temp));
	
	/* check for error */
	if (n < ZERO) 
	{
		printf("ERROR reading from socket");
		exit(EXIT_FAILURE);
	}	
	
	size = ntohl(temp);
	
	/* read from socket and write in file */
	while ( total < size  )
	{
		bytesread = read(socket , buffer , 255 );
		
		/*write the contents of buffer in file */
		n = write(fd, buffer , bytesread );
		
		/* check for error */
		if (n != bytesread)
		{
			printf("File write Error \n");
			fflush(stdout);
		
			/* add to log file */
			SRCCE_server_create_log("File write Error" , "SERVER");
			
			exit(EXIT_FAILURE);
		}
		total = total + bytesread;
		
		if (total == size)
		{
			printf ("File written successfully \n");
			fflush(stdout);
			
			/* add to log file */
			SRCCE_server_create_log("File written successfully" , "SERVER");
			break;
		}
	}
	close(fd);
	SRCCE_server_decrypt(encrypt_file_path,temp_dir_path);
}
/**************************************************************************************
*
*  FILE NAME	: SRCCE_client_utilities.c
*
*  DESCRIPTION: This file contains the utilities functions.
*
*************************************************************************************/

#include "SRCCE_client_header.h"

/***************************************************************************
 * FUNCTION NAME: SRCCE_client_initialize
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
void SRCCE_client_initialize(char* ip,char *port)
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
 * FUNCTION NAME: SRCCE_client_get_date_time
 *
 * DESCRIPTION: This function finds the current date and time of the system.
 * 	
 * ARGUMENTS: Nothing.
 *
 * RETURNS: Pointer to date time string.
 *
***************************************************************************/
char* SRCCE_client_get_date_time()
{
	/* buffer to store date and time */
	char *datetime = (char *)malloc(sizeof(char) * MSG);

	/* get current time */
	time_t t = time(NULL);

	/* convert time into localtime */
	struct tm *time = localtime(&t);

	/* format time according to RFC */
	strftime(datetime, MSG, "%a, %d %b %Y %T %z", time);
	
	return datetime;
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_client_create_log
 *
 * DESCRIPTION: This function adds the given message to the log file.
 *
 * ARGUMENTS: message: Message to be added to the log file.
 *			  username: Name of the user.
 * 
 * RETURNS: Nothing.
 *
***************************************************************************/
void SRCCE_client_create_log( char *message, char *username) 
{
	/* logging file */
	char log_file_path[MSG];
	
	FILE *p_log_fp = NULL;
	
	char *datetime = NULL;
	
	char *log = (char *)malloc(sizeof(char) * MSG);
	
	strcpy(log_file_path,"../log/");
	strcat(log_file_path,"client_log_file.txt");
	
	p_log_fp = fopen(log_file_path,"a");
	
	if (NULL == p_log_fp)			
	{
		fprintf(stderr,"Error in Opening Log File...\n");
		exit(EXIT_FAILURE);
	}

	/* log data */
    datetime = SRCCE_client_get_date_time();
	
	strcpy(log, datetime);
	strcat(log,"  username: ");
	strcat(log, username);
	strcat(log," ");
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
 * FUNCTION NAME: SRCCE_client_receive_file_compile
 *
 * DESCRIPTION: Receives the compile file and decrypt it.
 * 	
 * ARGUEMENTS: socket_in: Socket descriptor.
 *			   filename: Name of file.
 *
 * RETURNS: An integer value
 * 			0 if FAILED
 * 			1 if SUCCESS
 *	   
***************************************************************************/
int SRCCE_client_receive_file_compile(int socket, char *filename)		
{	
	char buffer[FILESIZE] = {ZERO};
	int bytesread = ZERO;
	char log_msg[MSG] = {ZERO};
	int flag = ZERO;
	int n = ZERO;
	int temp = ZERO;
	int size = ZERO;
	int fd = ZERO;
	int total = ZERO;

	/* open file using file descriptor */
	fd = open("temp_encrypted.c", O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);	
	/* check for error */
	if (fd < ZERO )
	{
		printf("\n Error opening file %s\n","temp_encrypted.c");
		n = snprintf(log_msg, MSG, "Error opening file %s","temp_encrypted.c");
		SRCCE_client_create_log( log_msg, "CLIENT"); 
		exit(EXIT_FAILURE);			
	}
	
	/* read length of file from socket */
	n = read(socket, &temp, sizeof(temp));
	
	/* check for error */
	if (n < ZERO) 
	{
		printf("ERROR reading from socket");
		SRCCE_client_create_log( "ERROR reading from socket", "CLIENT"); 
		return flag;
	}	
	
	size = ntohl(temp);
	if( ZERO == size)
	{
		flag = ONE;
		close(fd);
	}
	else
	{
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
				SRCCE_client_create_log( "File write Error", "CLIENT");
				return flag;
			}
			total = total + bytesread;
			if (total == size)
			{
				printf ("File written successfully \n");
				SRCCE_client_create_log( "File written successfully", "CLIENT");
				flag = ONE;
				break;
			}
		}

		close(fd);
		SRCCE_client_decrypt("temp_encrypted.c",filename);
		/* remove the temp file */
		system("rm temp_encrypted.c &> error.txt");
	}
	return flag;
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_client_encrypt
 *
 * DESCRIPTION: Takes an input file and creates an encrypted version of the
 * input file.
 * 	
 * ARGUMENTS: filename: Name of input file.
 *			  new_filename: Name of encrypted file.
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_client_encrypt( char *filename, char *new_filename)
{    
	FILE *fp1 = NULL;
	FILE *fp2 = NULL;
	int key = CIPHER;
	char val = ZERO;
	char ch = ZERO;

	/* open the given file in read mode */
	fp1 = fopen(filename, "r");
	
	/* error handling */
	if (NULL == fp1) 
	{
		printf("Unable to open the input file!!\n");
		SRCCE_client_create_log( "Unable to open the input file!!", "CLIENT");
		exit(EXIT_FAILURE);
	}

	/* open the temporary file in write mode */
	fp2 = fopen(new_filename, "w");
	
	/* error handling */
	if (NULL ==fp2) 
	{
		printf("Unable to open the temporary file!!\n");
		SRCCE_client_create_log( "Unable to open the temporary file!!", "CLIENT");
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
 * FUNCTION NAME: SRCCE_client_decrypt
 *
 * DESCRIPTION: Takes an input encrypted file and decrypts the file and create
 * a new file without any encryption.
 * 	
 * ARGUMENTS: filename: Name of input encrypted file.
 *			  decrypt_file: Name of decrypted file.
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_client_decrypt( char *filename, char *decrypt_file)
{
	int key = CIPHER;
	char val = ZERO;
	char ch = ZERO;
	FILE *fp1 = NULL;
	FILE *fp2 = NULL;

	/* open the file containint cipher text in read mode */
	fp1 = fopen(filename, "r");
	if (NULL == fp1) 
	{
		printf("Unable to open file!!\n");
		SRCCE_client_create_log( "Unable to open file!!", "CLIENT");
		exit(EXIT_FAILURE);
	}
	fp2 = fopen(decrypt_file, "w");
	if (NULL ==fp2) 
	{
		printf("Unable to open file!!\n");
		SRCCE_client_create_log( "Unable to open file!!", "CLIENT");
		exit(EXIT_FAILURE);
    	}

    /*
    * converting the cipher text to plain
    * text and printing it to console
    */
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
 * FUNCTION NAME: SRCCE_client_password_mask
 *
 * DESCRIPTION: It masks the user's entered password with '*' and send to the
 * server.
 *
 * ARGUMENTS: Nothing.
 *
 * RETURNS: Pointer to the password string.
 *	   
***************************************************************************/ 
char *SRCCE_client_password_mask()
{
	char passwd[MSG] = {ZERO};
	char *in = passwd;
	struct termios  tty_orig;
	char c = ZERO;
	
	puts("\n Enter password:");
			
	tcgetattr( STDIN_FILENO, &tty_orig );
	struct termios  tty_work = tty_orig;
	tty_work.c_lflag &= ~( ECHO | ICANON ); 
	tty_work.c_cc[ VMIN ]  = ONE;
	tty_work.c_cc[ VTIME ] = ZERO;
	tcsetattr( STDIN_FILENO, TCSAFLUSH, &tty_work );
		
	while (TRUE) 
	{
		if (read(STDIN_FILENO, &c, sizeof c) > ZERO) 
		{
			if ('\n' == c) 
			{
				break;
			}
			*in++ = c;
			write(STDOUT_FILENO, "*", ONE);
		}
	}

	tcsetattr( STDIN_FILENO, TCSAFLUSH, &tty_orig );

	*in = '\0';
	fputc('\n', stdout);
	return strdup(passwd);
}

/***************************************************************************
 * FUNCTION NAME: SRCCE_client_check_filesize
 *
 * DESCRIPTION: Checks if the file size if less than 64KB or not.
 * 
 * ARGUMENTS: filename: Name of input file.
 *
 * RETURNS: It returns an interger value.
 * 			0  if greater than 64KB
 * 			1  if less than or equal to 64KB
 *	   
***************************************************************************/
int SRCCE_client_check_filesize(char *filename)
{
	/* declare file pointer */
	FILE *fp = NULL;
	
	/* size flag */
	int flag  = INVALID_FILESIZE;
	
	/* variable to store the size of the file */
	int file_size = INVALID_FILESIZE;
	
	/* Open the required file in read mode */
	fp = fopen(filename, "r");
	
	/* check for error */
	if( NULL == fp)
	{
		printf("\n Unable to open the file: %s", filename);
		SRCCE_client_create_log( "Unable to open file!!", "CLIENT");
		return flag;
	}
	
	/* Get the size of the file */
	fseek(fp, ZERO, SEEK_END);
	file_size = ftell(fp);
	printf("\nFILE SIZE is %d",file_size);
	fflush(stdout);
	
	/* close the file */
	fclose(fp);
	
	if( file_size == INVALID_FILESIZE)
	{
		/* 0 file size */
		return flag;
	}
	
	/* check if file_size is less than equal to FILESIZE */
	if( file_size <= FILESIZE)
	{
		/* valid file size */
		flag = VALID_FILESIZE;
	}
	else
	{
		/* invalid file size */
		flag = INVALID_FILESIZE;
	}
	
	return flag;	
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_client_validate_string_length
 *
 * DESCRIPTION: Validates if the string entered by the user is less than the 
 * maximum allowed length or not. 
 * 
 * ARGUMENTS: Input_string: Input string.
 *			  max_length: Maximum allowed length of string.
 *
 * RETURNS: It returns an interger value.
 * 			0  if not a valid length.
 * 			1  if a valid length.
 *	   
***************************************************************************/
int SRCCE_client_validate_string_length(char *Input_string, int max_length)
{
	int len  = ZERO;
	int flag = INVALID_LEN;
	
	/* find the length of the string */
	len = strlen(Input_string);
	
	/* check the if less than max_length */
	if( len <= max_length )
	{
		flag = VALID_LEN;
	}
	else
	{
		flag = INVALID_LEN;
	}
	
	return flag;
}

/***************************************************************************
 * FUNCTION NAME: SRCCE_client_validate_choice
 *
 * DESCRIPTION: Validates if the choice entered by the user is correct or
 * not. 
 * 
 * ARGUMENTS: choice: Input choice string.
 *
 * RETURNS: It returns an interger value.
 * 			0  if not a valid choice.
 * 			1  if a valid choice is a digit.
 *			2  if a valid choice is a character(y/Y/n/N). 
 *	   
***************************************************************************/
int SRCCE_client_validate_choice(char *choice)
{
	int len = ZERO;	
	int valid = ZERO;
	
	/* check the length of the choice */
	len = strlen(choice);
		
	if( len > ONE)
	{
		valid = INVALID_CHOICE;
	}
	else
	{
		/* now check if it is a digit or not */
		if(ZERO == isdigit(choice[ZERO]))
		{
			if( ZERO == strcmp(choice,"y") || ZERO == strcmp(choice,"Y") || ZERO == strcmp(choice,"n") || ZERO == strcmp(choice,"N"))
			{
				valid = CHAR_CHOICE;
			}
		}
		else /* if it is a digit then it is a valid choice */
		{
			valid = DIGIT_CHOICE;
		}
	}
	
	return valid;
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_client_create_socket
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
int SRCCE_client_create_socket(int socket_in, int family, int type, int protocol)
{
	/* create socket */
	socket_in = socket(family, type, protocol);
	
	/* check for error */
	if(NEG == socket_in)
	{
		printf("\n Failed to create the socket\n");
		SRCCE_client_create_log( "Failed to create the socket", "CLIENT");
		/* do error handling here return this value -1 */
		exit(EXIT_FAILURE);
	}
	
	return socket_in;
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_client_send_message
 *
 * DESCRIPTION: It sends the required string message over the socket. 
 * 
 * ARGUEMENTS: socket_in: Socket descriptor.
 *			   message: Message string.
 * 
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_client_send_message(int socket, char *message)
{
	int length = ZERO;
	int temp = ZERO;
	int n = ZERO;
	
	length = strlen(message);
	temp = htonl(length);
		
	/* send length of the message */
	n = write(socket, &temp, INT_LEN);
		
	/* check for error */
	if( n < ZERO )
	{
			printf("\n message not send \n");
			SRCCE_client_create_log( "message not send", "CLIENT");
			/* return this value do no exit */
			exit(EXIT_FAILURE);
	}
	/* sending the message */
	n = write(socket, message, length);
	
	/* check for error */
	if( (n != length) || (n < ZERO))
	{
			printf("\n message not send \n");
			SRCCE_client_create_log( "message not send", "CLIENT");
			/* return this value do no exit */
			exit(EXIT_FAILURE);
	}
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_client_receive_message
 *
 * DESCRIPTION: Receives the message from the socket and stores it in a string
 * buffer.
 *
 * ARGUEMENTS: socket_in: Socket descriptor.
 *			   message: Message buffer to store the received message.
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_client_receive_message(int socket, char *message)
{
	int temp = ZERO;
	int size = ZERO;
	int n = ZERO;
	
	/* read length of the message */
	n = read(socket, &temp, sizeof(temp));
	size = ntohl(temp);
	
	/* check for error */
	if( n < ZERO )
	{
		printf("\n message not received successfully\n");
		SRCCE_client_create_log( "message not  received successfully", "CLIENT");
		exit(EXIT_FAILURE);
	}
		
		
	/* read the message string */
	n = read(socket, message, size);
	message[size] = '\0';
	
	/* check for error */
	if( n < ZERO )
	{
		printf("\n message not received successfully\n");
		SRCCE_client_create_log( "message not  received successfully", "CLIENT");
		exit(EXIT_FAILURE);
	}
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_client_send_file
 *
 * DESCRIPTION: Encrypt and send a file over the socket.  
 * 	
 * ARGUEMENTS: socket_in: Socket descriptor.
 *			   filename: Name of file.
 *			   
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_client_send_file(int socket, char *filename)		
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
	
	SRCCE_client_encrypt(filename,"encrypted.txt");
	
	/* open the file using file descriptor */
	fd = open("encrypted.txt", O_RDONLY);
	/* check for error */
	if (fd < ZERO )
	{
		printf("\n Error opening file %s\n",filename);
		exit(EXIT_FAILURE);			
	}
	
	/* open the file using file pointer */
	fp = fopen("encrypted.txt","r");
	/* check for error */
	if(NULL == fp)
	{
		printf("\n Error opening file %s\n","encrypted.txt");
		n = snprintf(log_msg, MSG, "Error opening file %s\n","encrypted.txt");
		SRCCE_client_create_log( log_msg, "CLIENT"); 
		exit(EXIT_FAILURE);			
	}
	
	/* find the size of file */
	fseek(fp, ZERO, SEEK_END);
	filesize = ftell(fp);
	printf("Sending File of size: %d\n",filesize);
	fflush(stdout);
	
	/* Now point to start of the file */
	rewind(fp);
	temp = htonl(filesize);
	
	/*send file size */
	n = write(socket, &temp, INT_LEN);
	
	/* check for error */
	if( n < ZERO )
	{
		printf("\n failed to send the file size\n");
		SRCCE_client_create_log( " failed to send the file size", "CLIENT");
		exit(EXIT_FAILURE);
	}
	
	/* send the file over the socket */
	while ( (bytesread=read(fd , send_buffer , filesize)) > ZERO )
	{
		n = write(socket , send_buffer , bytesread);
		
		/* check for error */
		if(n != bytesread)
		{
			printf("\n failed to send whole file\n");
			SRCCE_client_create_log( " failed to send whole file", "CLIENT");
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
 * FUNCTION NAME: SRCCE_client_receive_file
 *
 * DESCRIPTION: Receives a file and SRCCE_client_decrypt it.
 *
 * ARGUEMENTS: socket_in: Socket descriptor.
 *			   filename: Name of file.
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_client_receive_file(int socket, char *filename)		
{	
	char buffer[FILESIZE] = {ZERO};
	int bytesread = ZERO;
	int n = ZERO;
	int temp = ZERO;
	int size = ZERO;
	int fd = ZERO;
	int total = ZERO;
	char log_msg[MSG] = {ZERO};
	
	/* open file using file descriptor */
	fd = open("temp_encrypted.c", O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);	
	
	/* check for error */
	if (fd < ZERO )
	{
		printf("\n Error opening file %s\n","temp_encrypted.c");
		n = snprintf(log_msg, MSG, "Error opening file %s\n","temp_encrypted.txt");
		SRCCE_client_create_log( log_msg, "CLIENT");
		exit(EXIT_FAILURE);			
	}
	
	/* read length of file from socket */
	n = read(socket, &temp, sizeof(temp));
	
	/* check for error */
	if (n < ZERO) 
	{
		printf("ERROR reading from socket");
		SRCCE_client_create_log( "ERROR reading from socket", "CLIENT");
		exit(EXIT_FAILURE);
	}	
	
	size = ntohl(temp);
	
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
			SRCCE_client_create_log( "File write Error", "CLIENT");
			exit(EXIT_FAILURE);

		}
		total = total + bytesread;
		if (total == size)
		{
			printf ("File written successfully \n");
			SRCCE_client_create_log( "File written successfully", "CLIENT");
			break;
		}
	}
	
	close(fd);
	SRCCE_client_decrypt("temp_encrypted.c", filename);
	
	/* remove the temp file */
	system("rm temp_encrypted.c &> error.txt");	
}

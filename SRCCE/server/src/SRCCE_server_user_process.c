/**************************************************************************************
*
*  FILE NAME	: SRCCE_server_user_process.c
*
*  DESCRIPTION: This file contains functions required for the user's process.
*
*************************************************************************************/

#include "SRCCE_server_header.h"

/***************************************************************************
 * FUNCTION NAME: SRCCE_server_run_code
 *
 * DESCRIPTION: This function runs the executable of the selected question 
 * for the user. It creates the output file for the user.
 * 	
 * ARGUEMENTS: socket: Socket descriptor.
 *			   username: Name of the user.
 *			   question: Name of the question.
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_server_run_code(int socket, char *username, char *question)
{
	/* Declare file pointers */
	FILE *fp = NULL;
	FILE *fp1 = NULL;
		
	/* find the question file for user */
	char user_dir_path[PATH] = "../../users/";
	char temp_file_path[PATH] = {ZERO};
	char output_file_path[PATH] = {ZERO};
	char test_file_path[PATH] = {ZERO};
	char executable_file_path[PATH] = {ZERO};
	char command[COMMAND_LENGTH] = {ZERO};
	char string[MSG] = {ZERO};
	
	/* log message string */
	char log_msg[MSG] = {ZERO};
	
	/* find the question folder in corresponding user */
	strcat(user_dir_path, username);
	strcat(user_dir_path,"/Question_Bank/");
	strcat(user_dir_path,question);
	strcat(user_dir_path,"/");
	
	/* find the executable path */
	strcpy(executable_file_path, user_dir_path);
	strcat(executable_file_path,username);
		
	/* find the actual output file path for user */
	strcpy(output_file_path, user_dir_path);
	strcat(output_file_path,"actual_output");
		
	/* find the testcase file path for user */
	strcpy(test_file_path, user_dir_path);
	strcat(test_file_path,"testcase");
	
	/* open the test case file in read mode */
	fp = fopen(test_file_path, "r");
	
	/* check for error */
	if( NULL == fp)
	{
		printf("\nfailed to open file: %s", test_file_path);
		
		/* add to log file */
		snprintf(log_msg, MSG, "failed to open file: %s", test_file_path);
		SRCCE_server_create_log(log_msg, username); 
		
		exit(EXIT_FAILURE);
	}
	
	/* path for the temporary test case file */
	strcpy(temp_file_path, user_dir_path);
	strcat(temp_file_path,"temp_testcase");
	
	/* truncate the actual output file */
	fp1 = fopen(output_file_path, "w");
	
	/* check for error */
	if( NULL == fp1)
	{
		printf("\nfailed to open file: %s", output_file_path);
		
		/* add to log file */
		snprintf(log_msg, MSG, "failed to open file: %s", output_file_path);
		SRCCE_server_create_log(log_msg, username); 
		
		exit(EXIT_FAILURE);
	}
	
	/* close the file */
	fclose(fp1);
	
	/* add to log file */
	SRCCE_server_create_log("running the code...", username); 
		
	/* create the actual_output file */
	while(TRUE)
	{
		if( NULL == fgets(string, MSG, fp))
		{
			break;
		}
		/* open the temp testcase file */
		fp1 = fopen(temp_file_path,"w");
		
		/* write one test case in this temporary file */
		fprintf(fp1,"%s",string);
		
		/* close the file */
		fclose(fp1);
		
		/* 
		./"executable file path" < "temporary testcase file path" >> "actual output file path"
		*/
		strcpy(command, "./");
		strcat(command, executable_file_path);
		strcat(command, " < ");
		strcat(command, temp_file_path);
		strcat(command, " >> ");
		strcat(command, output_file_path);
		system(command);
	} 
	
	/* send the output file to the user */
	SRCCE_server_send_file_user(socket, "actual_output", username, question);
	
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_server_edit_question
 *
 * DESCRIPTION: This allows the user to receive a question's answer file and
 * edit and send it back to the server. 
 *
 * ARGUEMENTS: socket: Socket descriptor.
 *			   username: name of the user.
 *			   question: name of the question.
 * 
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_server_edit_question(int socket, char *username, char *question)
{
	/* log message string */
	char log_msg[MSG] = {ZERO};
	
	printf("\n%s entered edit question...\n",username);
	fflush(stdout);
	
	/* add to log file */
	snprintf(log_msg, MSG, "%s entered edit question...",username);
	SRCCE_server_create_log(log_msg, username); 
	
	SRCCE_server_send_message(socket,"\nentering edit question...\n");	
	
	/* send the answer file(main.c) to the user */
	SRCCE_server_send_file_user(socket, "main.c", username, question);
	
	/* add to log file */
	SRCCE_server_create_log("Sending the answer file(main.c) to user", username); 
	
	/* receive the answer file from user */
	SRCCE_server_receive_file_user(socket, "main.c", username, question);
	
	/* add to log file */
	SRCCE_server_create_log("receive the answer file from user", username); 
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_server_check_warning_error
 *
 * DESCRIPTION:  This finds if there are any errors/warnings at compile time.
 * 
 * ARGUEMENTS: filename: name of the file.
 *			   username: name of the user.
 * 			   question; name of the question.
 *
 * RETURNS: An integer value.
 *			1 if compilation successful.
 *	   		0 if compilatin failed.
***************************************************************************/
int SRCCE_server_check_warning_error(char *filename, char *username , char *question)
{
	/* Declare file pointer */
	FILE *fp = NULL;
	
	/* Variable to store size of file */
	int size = ZERO;
	
	/* Flag to store if error/warnings present in the compilation result */
	int error_flag = ZERO;
		
	/* log message string */
	char log_msg[MSG] = {ZERO};
	
	printf("\n%s: Checking warnings or error in compilation result in question:%s\n",username, question);
	fflush(stdout);
	
	/* add to log file */
	snprintf(log_msg, MSG, "%s: Checking warnings or error in compilation result in question:%s",username, question);
	SRCCE_server_create_log(log_msg, username);
	
	/* Find the compile file path */
	char compile_file_path[PATH] = "../../users/";
	strcat(compile_file_path, username);
	strcat(compile_file_path,"/Question_Bank/");
	strcat(compile_file_path,question);
	strcat(compile_file_path,"/");
	strcat(compile_file_path,filename);
	
	/* Open the compile file in read mode */
	fp = fopen(compile_file_path, "r");
	
	/* check for error */
	if( NULL == fp)
	{
		printf("\nError opening file: %s\n", compile_file_path);
		
		/* add to log file */
		snprintf(log_msg, MSG, "failed to open file: %s", compile_file_path);
		SRCCE_server_create_log(log_msg, username);
		
		return error_flag;
	}

	/* get the size of file */
	fseek(fp, ZERO, SEEK_END);
	size = ftell(fp);
	
	/* close the file */
	fclose(fp);
	
	/* compilation successful if file empty / size = 0 */
	if( ZERO == size )
	{
		error_flag = ONE;
		return error_flag;
	}
	else
	{
		return error_flag;
	}
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_server_compile_run
 *
 * DESCRIPTION:  This allows the user to compile and run the code. It displays
 * all the warnings and errors to the user if there are any. In case of successful
 * compilation it displays the output of the user's code.
 * In case of unsuccessful compilation it only shows the warnings and errors.
 * 	
 * ARGUEMENTS: socket: Socket descriptor.
 *			   username: name of the user.
 * 			   question: name of the question.
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_server_compile_run(int socket, char *username, char *question)
{	
	/* log message string */
	char log_msg[MSG] = {ZERO};
	
	/* pointer to store send and receive message */
	char *send_msg = NULL;
	char *recv_msg = NULL;
	
	/* declare file pointer */
	FILE *fp = NULL;
	
	/* Flag to check if there are errors/warnings or not */
	int error_flag = ZERO;
	
	/* to check if compile file size is less than 64KB or not */
	int compile_flag = ZERO;
	
	/* receive file acknowledgement */
	int ack_file = ZERO;
	
	/* find the question file for user */
	char user_dir_path[PATH] = "../../users/";
	char temp_dir_path[PATH] = {ZERO};
	char compile_file_path[PATH] = {ZERO};
	char output_file_path[PATH] = {ZERO};
	char executable_file_path[PATH] = {ZERO};
	char command[COMMAND_LENGTH] = {ZERO};
	
	printf("\n%s entered compile/run...\n",username);
	fflush(stdout);
	
	/* add to log file */
	snprintf(log_msg, MSG, "%s entered compile/run...",username);
	SRCCE_server_create_log(log_msg, username);
	
	SRCCE_server_send_message(socket,"\nentering compile/run....\n");
			
	/* find the question folder in corresponding user */
	strcat(user_dir_path, username);
	strcat(user_dir_path,"/Question_Bank/");
	strcat(user_dir_path,question);
	strcat(user_dir_path,"/");
	
	/* find the main.c file path for user */
	strcpy(temp_dir_path, user_dir_path);
	strcat(temp_dir_path,"main.c");
	
	/* find the executable file path for user */
	strcpy(executable_file_path, user_dir_path);
	strcat(executable_file_path,username);
	
	/* gcc -Wall -o "executable_file_path" "file path" &> "compile file path" */
	strcpy(command,"gcc -Wall -o ");
	strcat(command, executable_file_path);
	strcat(command," ");
	strcat(command, temp_dir_path);
	strcat(command, " &> ");
	
	/* compilation file path */
	strcpy(compile_file_path, user_dir_path);
	strcat(compile_file_path,"compile.txt");
	
	strcat(command, compile_file_path);
	system(command);
	
	/* check the compile size if it is greater than 64KB */
	compile_flag = SRCCE_server_check_filesize(temp_dir_path);
	
	/* send the compile_flag */
	send_msg = (char *)malloc(sizeof(char) * MSG);
	
	/* check for error while allocating memory */
	if( NULL == send_msg)
	{
		printf("\n Failed to allocate the memory to send_msg \n");
		SRCCE_server_create_log("Failed to allocate the memory to send_msg" , username); 
		exit(EXIT_FAILURE);
	}
	snprintf(send_msg,MSG,"%d",compile_flag);
	SRCCE_server_send_message(socket,send_msg);
	free(send_msg);
	
	/* compile file size greater than 64 KB */
	if( ZERO == compile_flag)
	{
		printf("\nCompile file size very large too many errors...\n");
		fflush(stdout);
		
		/* add to log file */
		SRCCE_server_create_log("Compile file size very large too many errors...", username);
	}
	else
	{
		while( RECEIVE_FAIL == ack_file)
		{
		
			/* send the compilation result to the user */
			SRCCE_server_send_file_user(socket, "compile.txt", username, question );
			
			/* receive the file receive acknowledgement */
			recv_msg = (char *)malloc(sizeof(char) * MSG);
			
			/* check for error while allocating memory */
			if( NULL == recv_msg)
			{
				printf("\n Failed to allocate the memory to recv_msg \n");
				SRCCE_server_create_log("Failed to allocate the memory to recv_msg" , username); 
				exit(EXIT_FAILURE);
			}
			
			SRCCE_server_receive_message(socket,recv_msg);
			ack_file = atoi(recv_msg);
			free(recv_msg);
			
			if( RECEIVE_SUCCESS == ack_file)
			{
				printf("\n%s: compile file received by the user...",username);
				fflush(stdout);
				
				/* add to log file */
				SRCCE_server_create_log("compile file received by the user...", username);
				
				break;
			}
			else
			{
				printf("\n%s: compile file not received by the user...",username);
				fflush(stdout);
				
				/* add to log file */
				SRCCE_server_create_log("compile file not received by the user...", username);
			}
			
		}
	
	}
		
	/* check if there are any warnings or errors in compile.txt */
	error_flag = SRCCE_server_check_warning_error("compile.txt", username, question);
	
	/* send error_flag to the user */
	send_msg = (char *)malloc(sizeof(char) * MSG);
	
	/* check for error while allocating memory */
	if( NULL == send_msg)
	{
		printf("\n Failed to allocate the memory to send_msg \n");
		SRCCE_server_create_log("Failed to allocate the memory to send_msg" , username); 
		exit(EXIT_FAILURE);
	}
	
	snprintf(send_msg,MSG,"%d",error_flag);
	SRCCE_server_send_message(socket,send_msg);
	free(send_msg);
		
	/* No warnings  */
	if( ONE == error_flag)
	{
		printf("\n%s: 0 Errors and 0 Warnings...\n", username);
		fflush(stdout);
		
		/* add to log file */
		SRCCE_server_create_log("0 Errors and 0 Warnings...", username);
		
		SRCCE_server_run_code(socket, username, question);
	}
	else
	{
		printf("\nWarnings in user: %s, question : %s\n", username, question);
		fflush(stdout);
		
		/* add to log file */
		snprintf(log_msg, MSG, "Warnings in user: %s, question : %s ", username, question);
		SRCCE_server_create_log(log_msg, username);
		
		/* find the actual output file path for user */
		strcpy(output_file_path, user_dir_path);
		strcat(output_file_path,"actual_output");
		
		/* truncate the actual file path */
		fp = fopen(output_file_path, "w");
		fclose(fp);
	}
		
	/* remove the executable after execution */
	strcpy(command, "rm -rf ");
	strcat(command,executable_file_path);
	strcat(command," &> error.txt");
	system(command);
	
	/* remove the compilation file after execution */
	strcpy(command, "rm -rf ");
	strcat(command,compile_file_path);
	strcat(command," &> error.txt");
	system(command);
}

/***************************************************************************
 * FUNCTION NAME: SRCCE_server_see_result
 *
 * DESCRIPTION:  Shows that which of the test cases passed and which failed.
 * It also shows how many of them passed.
 * 
 * ARGUEMENTS: socket: Socket descriptor.
 *			   username: name of the user.
 *			   question: name of the question.
 *
 * RETURNS: Nothing.
 *
 ****************************************************************************/
void SRCCE_server_see_result(int socket, char *username, char *question)
{
	FILE *fp_actual = NULL;
	FILE *fp_expected = NULL;
	FILE *fp_result = NULL;
	FILE *fp_test = NULL;
	
	int size_of_file = ZERO;
	int total_test_case = ZERO;
	int pass_test_case = ZERO;
	int len  = ZERO;
	
	printf("\n%s entered see result...\n",username);
	fflush(stdout);
	
	SRCCE_server_create_log("entered see result...", username);
	
	SRCCE_server_send_message(socket,"\nentering see result....\n");
	
	/* find the question file for user */
	char user_dir_path[PATH] = "../../users/";
	char result_file_path[PATH] = {ZERO};
	char test_file_path[PATH] = {ZERO};
	char actual_output_file_path[PATH] = {ZERO};
	char expected_output_file_path[PATH] = {ZERO};
	char command[COMMAND_LENGTH] = {ZERO};
	
	char test_case[MSG] = {ZERO};
	char actual_output[MSG] = {ZERO};
	char expected_output[MSG] = {ZERO};
	char log_msg[MSG] = {ZERO};
	
	char *send_msg = NULL;
	
	/* find the question folder in corresponding user */
	strcat(user_dir_path, username);
	strcat(user_dir_path,"/Question_Bank/");
	strcat(user_dir_path,question);
	strcat(user_dir_path,"/");
	
	/* find the actual output file path for user */
	strcpy(actual_output_file_path, user_dir_path);
	strcat(actual_output_file_path,"actual_output");
	strcpy(command,"touch ");
	strcat(command, actual_output_file_path);
	system(command);
	
	/* find the expected output file path for user */
	strcpy(expected_output_file_path, user_dir_path);
	strcat(expected_output_file_path,"Expected_output");
	
	/* find the result file path for user */
	strcpy(result_file_path, user_dir_path);
	strcat(result_file_path,"Result");
	
	/* find the result file path for user */
	strcpy(test_file_path, user_dir_path);
	strcat(test_file_path,"testcase");
		
	/* open the files in read mode */
	fp_actual = fopen(actual_output_file_path, "r");
	
	/* check for error */
	if( NULL == fp_actual)
	{
		printf("\nUnable to open the file: %s\n", actual_output_file_path);
		
		/* add to log file */
		snprintf(log_msg, MSG, "Unable to open the file: %s", actual_output_file_path);
		SRCCE_server_create_log(log_msg, username);
		
		exit(EXIT_FAILURE);
	}
	
	/* open the files in read mode */
	fp_expected = fopen(expected_output_file_path, "r");
	
	/* check for error */
	if( NULL == fp_expected)
	{
		printf("\nUnable to open the file: %s\n",expected_output_file_path);
		
		/* add to log file */
		snprintf(log_msg, MSG, "Unable to open the file: %s", expected_output_file_path);
		SRCCE_server_create_log(log_msg, username);
		
		exit(EXIT_FAILURE);
	}
	
	/* open the files in read mode */
	fp_test = fopen(test_file_path, "r");
	
	/* check for error */
	if( NULL == fp_test)
	{
		printf("\nUnable to open the file: %s\n",test_file_path);
		
		/* add to log file */
		snprintf(log_msg, MSG, "Unable to open the file: %s\n",test_file_path);
		SRCCE_server_create_log(log_msg, username);
		
		exit(EXIT_FAILURE);
	}
	
	/* check file size of actual output file */
	fseek(fp_actual, ZERO, SEEK_END);
	size_of_file = ftell(fp_actual);
	rewind(fp_actual);
	
	/* send the actual output file size to the user */
	send_msg = (char *)malloc(sizeof(char) * MSG);
	
	/* check for error while allocating memory */
	if( NULL == send_msg)
	{
		printf("\n Failed to allocate the memory to send_msg \n");
		SRCCE_server_create_log("Failed to allocate the memory to send_msg" , username); 
		exit(EXIT_FAILURE);
	}
	
	snprintf(send_msg,MSG,"%d",size_of_file);
	SRCCE_server_send_message(socket,send_msg);
	free(send_msg);
	
	if( ZERO == size_of_file )
	{
		printf("\n0 test cases passed \n User :%s Question:%s", username, question);
		fflush(stdout);
			
		/* add to log file */
		snprintf(log_msg, MSG, "0 test cases passed \n User :%s Question:%s", username, question);
		SRCCE_server_create_log(log_msg, username);
	}
	else
	{
		/* create the result file */
		fp_result = fopen(result_file_path, "w");
		
		/* check for error */
		if( NULL == fp_result)
		{	
			printf("\nUnable to open the file: %s\n",result_file_path);
			
			/* add to log file */
			snprintf(log_msg, MSG, "Unable to open the file: %s\n",result_file_path);
			SRCCE_server_create_log(log_msg, username);
			
			exit(EXIT_FAILURE);
		}
		
		fprintf(fp_result, "%s %s %s %s\n", "Test_Case", "Expected_Output", "Actual_Output", "Result");
		
		while(TRUE)
		{
			/* check for end of file */
			if( (NULL == fgets(test_case, MSG, fp_test)) || (NULL == fgets(actual_output, MSG, fp_actual)) || (NULL == fgets(expected_output, MSG, fp_expected)) )
			{
				break;
			}
			
			total_test_case++;
			
			/* remove the new line character */
			len = strlen(actual_output) - 1;
			if('\n' == actual_output[len])
			{
				actual_output[len] = '\0';
			}
			
			len = strlen(expected_output) - 1;
			if('\n' == expected_output[len])
			{
				expected_output[len] = '\0';
			}
			
			len = strlen(test_case) - 1;
			if('\n' == test_case[len])
			{
				test_case[len] = '\0';
			}
			
			/* now compare the two output files */
			if(ZERO == strcmp(actual_output, expected_output))
			{
				fprintf(fp_result, "%s		%s		%s		%s\n", test_case, expected_output, actual_output, "PASS");
				pass_test_case++;
			}
			else
			{
				fprintf(fp_result, "%s		%s		%s		%s\n", test_case, expected_output, actual_output, "FAIL");
			}
		}
		fprintf(fp_result,"%s\n" ,"**************************************************");
		fprintf(fp_result,"%s %d %s %d\n" ,"Result : ", pass_test_case,"/",total_test_case);
		fclose(fp_result);
		
		/* send the result file to the user */
		SRCCE_server_send_file_user(socket, "Result", username, question);
		
	}
	/* close the files */
	fclose(fp_actual);
	fclose(fp_expected);
	fclose(fp_test);
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_server_display_questions
 *
 * DESCRIPTION: Displays the questions name and their description to user.
 * 	
 * ARGUEMENTS: socket: Socket descriptor.
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_server_display_questions(int socket)
{
	int total_questions = ZERO;
	char question[Q_DESCRIPTION] = {ZERO};
	char *send_msg = NULL;
	
	printf("\nentered display questions...\n");
	fflush(stdout);
	
	/* add to log file */
	SRCCE_server_create_log("entered display questions...", "user");
		
	/* find total number of questions in database */
	MYSQL *con = my_sql_init();
	
	if (mysql_query(con, "SELECT COUNT(*) FROM Question_Bank")) 
	{
		SRCCE_server_finish_with_error(con);
	}
  
	MYSQL_RES *result = mysql_use_result(con);
	if (result == NULL) 
	{
    	SRCCE_server_finish_with_error(con);
	}
	
	MYSQL_ROW row = mysql_fetch_row(result);
	if (NULL != row)
	{
		total_questions = atoi(row[ZERO]);
	}
	mysql_free_result(result);
	
	/* send the total questions to user */
	send_msg = (char *)malloc(sizeof(char) * MSG);
	
	/* check for error while allocating memory */
	if( NULL == send_msg)
	{
		printf("\n Failed to allocate the memory to send_msg \n");
		SRCCE_server_create_log("Failed to allocate the memory to send_msg" , "user"); 
		exit(EXIT_FAILURE);
	}
	
	snprintf(send_msg,MSG,"%d",total_questions);
	SRCCE_server_send_message(socket,send_msg);
	free(send_msg);
	
	if(ZERO == total_questions)
	{
		printf("\nThere are zero Questions in Question_Bank...\nComing Soon..");
		fflush(stdout);
		
		/* add to log file */
		SRCCE_server_create_log("There are zero Questions in Question_Bank...Coming Soon..", "user");
		
		mysql_close(con);	
		mysql_library_end();
	}
	else
	{
		/* send the questions to user */
		if (mysql_query(con, "SELECT * FROM Question_Bank")) 
		{
			SRCCE_server_finish_with_error(con);
		}
  
		result = mysql_store_result(con);
		if (result == NULL) 
		{
			SRCCE_server_finish_with_error(con);
		}
		while ((row = mysql_fetch_row(result))) 
		{
			strcpy(question, row[ZERO]);
			strcat(question,"\t\t");
			strcat(question,row[ONE]);
			SRCCE_server_send_message(socket, question);
		}
		mysql_free_result(result);
		mysql_close(con);	
		mysql_library_end();
	}
}

/***************************************************************************
 * FUNCTION NAME: SRCCE_server_select_question
 *
 * DESCRIPTION: It displays the questions to the user and allows him to select
 * the question. After a question is selected he is presented with a set of 
 * options that he can perform on the question.
 *
 * ARGUEMENTS: socket: Socket descriptor.
 *			   username: name of the user.
 * 
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_server_select_question(int socket, char *username)
{
	char *recv_msg = NULL;
	char *send_msg = NULL;
	 
	char question[Q_NAME] = {ZERO};
	char log_msg[MSG] = {ZERO};
	
	int valid_flag = INVALID;
	
	printf("\n%s entered select question...\n",username);
	fflush(stdout);
	
	/* add to log file */
	snprintf(log_msg, MSG, "%s entered select question...",username);
	SRCCE_server_create_log(log_msg, username);
	
	SRCCE_server_display_questions(socket);
	
	/* receive the question name from user */
	SRCCE_server_receive_message(socket,question);
	
	if( ZERO == strcasecmp(question, "exit"))
	{
		
		printf("\n%s: Going to prevoius menu\n",username);
		fflush(stdout);
		
		/* add to log file */
		snprintf(log_msg, MSG, "%s: Going to prevoius menu\n",username);
		SRCCE_server_create_log(log_msg, username);
		
		return;
	}
	
	/* check if question name is valid or not */
	MYSQL *con = my_sql_init();
  	   	
	if (mysql_query(con, "SELECT * FROM Question_Bank")) 
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
		valid_flag = INVALID ;
      
		/* if question name found in database */
    	if(strcmp(question,row[ZERO]) == ZERO )
		{
			valid_flag = VALID;
			break;
		}
	}
	
	/* send flag if question name exists or not */
	send_msg = (char *)malloc(sizeof(char) * MSG);
	
	/* check for error while allocating memory */
	if( NULL == send_msg)
	{
		printf("\n Failed to allocate the memory to send_msg \n");
		SRCCE_server_create_log("Failed to allocate the memory to send_msg" , username); 
		exit(EXIT_FAILURE);
	}
	
	snprintf(send_msg,MSG,"%d",valid_flag);
	SRCCE_server_send_message(socket,send_msg);
	free(send_msg);
	
	/* if question name is valid */
	if( VALID == valid_flag)
	{
		
		/* 1. Answer Question
	       2. Compile/run
     	   3. See Results
	       4. EXIT
	   */
	
		int user_choice = INVALID;
		
		do
		{
			/* Receive the choice from user */
			recv_msg = (char *)malloc(sizeof(char) * MSG);
			
			/* check for error while allocating memory */
			if( NULL == recv_msg)
			{
				printf("\n Failed to allocate the memory to recv_msg \n");
				SRCCE_server_create_log("Failed to allocate the memory to recv_msg" , username); 
				exit(EXIT_FAILURE);
			}
			
			SRCCE_server_receive_message(socket,recv_msg);
			user_choice = atoi(recv_msg);
			free(recv_msg);
		
			/* select function according to user's choice */
			switch(user_choice)
			{
				case ONE: 	SRCCE_server_edit_question(socket, username, question);
							break;
						
				case TWO: 	SRCCE_server_compile_run(socket, username, question);
							break;
						
				case THREE:	SRCCE_server_see_result(socket, username, question);
							break;
							
				case FOUR: 	printf("\n%s exiting...\n",username);
							fflush(stdout);
						
							/* add to log file */
							snprintf(log_msg, MSG, "%s exiting...\n",username);
							SRCCE_server_create_log(log_msg, username);
						
							break;					
							
				default: 	printf("\ninvalid choice by %s user\n ", username);
						
							/* add to log file */
							snprintf(log_msg, MSG, "invalid choice by %s user", username);
							SRCCE_server_create_log(log_msg, username);
			}
				
		}while(FOUR != user_choice);
	}

	mysql_free_result(result);
	mysql_close(con);	
	mysql_library_end();
	
}
/***************************************************************************
 * FUNCTION NAME: SRCCE_server_user_process
 *
 * DESCRIPTION:  This allows the user to either go into the question menu or
 * log off from the system.
 * 	
 * ARGUEMENTS: socket: Socket descriptor.
 *			   username: name of the user.
 *
 * RETURNS: Nothing.
 *	   
***************************************************************************/
void SRCCE_server_user_process(int socket, char *username)
{
	char *recv_msg = NULL;
	
	/* 1. Select Question
	   2. LOGOFF
	*/
	int user_choice = ZERO;
	char query[MSG] = {ZERO};
	char login_status[TWO] = {ZERO};
	char thread_id[10] = {ZERO};
	
	char log_msg[MSG] = {ZERO};
	
	printf("\n%s entered user process...\n",username);
	fflush(stdout);
	
	/* add to log file */
	snprintf(log_msg, MSG, "%s entered user process...",username);
	SRCCE_server_create_log(log_msg, username);
	
	MYSQL *con = my_sql_init();
	snprintf(thread_id, 10, "%ld", syscall(SYS_gettid));
	snprintf(query, MSG, "UPDATE login_credentials SET thread_id = '%s' WHERE username= '%s'", thread_id, username);	
	if (mysql_query(con, query))
	{
		SRCCE_server_finish_with_error(con);
	}
	
	mysql_close(con);
	mysql_library_end();
	
	do
	{
		/* Receive the choice from user */
		recv_msg = (char *)malloc(sizeof(char) * MSG);
		
		/* check for error while allocating memory */
		if( NULL == recv_msg)
		{
			printf("\n Failed to allocate the memory to recv_msg \n");
			SRCCE_server_create_log("Failed to allocate the memory to recv_msg" , username); 
			exit(EXIT_FAILURE);
		}
				
		SRCCE_server_receive_message(socket,recv_msg);
		user_choice = atoi(recv_msg);
		free(recv_msg);
		
		/* select function according to user's choice */
		switch(user_choice)
		{
			case ONE: 	SRCCE_server_select_question(socket, username);
						break;

			case TWO: 	printf("\n%s LOGGED OFF\n",username);
						fflush(stdout);
					
						/* add to log file */
						snprintf(log_msg, MSG, "%s LOGGED OFF\n",username);
						SRCCE_server_create_log(log_msg, username);
					
						MYSQL *con = my_sql_init();
						strcpy(login_status,"0");
						snprintf(query, MSG, "UPDATE login_credentials SET login_status = '%s', thread_id = 'NULL' WHERE username= '%s'", login_status, username);	
						if (mysql_query(con, query))
						{
							SRCCE_server_finish_with_error(con);
						}
						mysql_close(con);
						mysql_library_end();
						break;
					
			default: 	printf("\nInvalid choice by %s user\n ", username);
						/* add to log file */
						snprintf(log_msg, MSG, "Invalid choice by %s user\n ", username);
						SRCCE_server_create_log(log_msg, username);
		}
				
	}while(TWO != user_choice);
}
/**************************************************************************************
*
*  FILE NAME	: SRCCE_server_header.h
*
*  DESCRIPTION: This File contains the required function prototypes required for SRCCE
*  system. It also contains the required header files. It contains the macros.
*
*************************************************************************************/

#ifndef SRCCE_SERVER_HEADER_H 
#define SRCCE_SERVER_HEADER_H 

/* Header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <mysql/mysql.h>
#include<pthread.h>
#include <termios.h>
#include <dirent.h>
#include <syscall.h>
#include <time.h>
#include <signal.h>
#include <ctype.h>

/* Macros used */
#define MAX_CLIENTS 40
#define MSG 2000
#define INT_LEN 4
#define CIPHER 5
#define USERNAME_LEN 25
#define PASSWORD_LENGTH 25
#define PATH 250
#define COMMAND_LENGTH 200
#define Q_NAME 8
#define Q_DESCRIPTION 125
#define FILESIZE 64000
#define IS_ADMIN 1
#define IS_USER 5
#define INVALID_USER 0
#define VALID_USER 1
#define LOGGED_IN 2
#define USER_LOGGED_IN 3
#define ADMIN_LOGGED_IN 4
#define PORT 6
#define IP 16
#define INVALID_QUESTION 0
#define VALID_QUESTION 1
#define RECEIVE_FAIL 0
#define RECEIVE_SUCCESS 1
#define INVALID_CHOICE 0
#define TRUE 1
#define USER_DETAILS 52
#define INVALID 0
#define VALID 1
#define NEG -1
#define ZERO 0
#define ONE 1
#define TWO 2
#define THREE 3
#define FOUR 4

/* Function prototypes */
void SRCCE_server_sigint_handler(int );
int SRCCE_server_create_socket(int , int , int , int );
void SRCCE_server_initialize(char *,char *);
void *SRCCE_client_handler(void *);

int SRCCE_server_check_filesize(char *);

void SRCCE_server_send_message(int , char *);
void SRCCE_server_receive_message(int , char *);

void SRCCE_server_send_file(int , char *);
int SRCCE_server_receive_file(int ,	char *);

void SRCCE_server_send_file_user(int , char *, char *, char *);
void SRCCE_server_receive_file_user(int , char *, char *, char *);	

int SRCCE_server_validate_password(char *);
int SRCCE_server_validate_username(char *);

void SRCCE_server_encrypt( char *, char *);
void SRCCE_server_decrypt( char *, char *);

void SRCCE_server_finish_with_error(MYSQL *);
int SRCCE_server_login(char *, char *);

void SRCCE_server_admin_process(int );

void SRCCE_server_add_remove_user(int );
void SRCCE_server_remove_user(int );
void SRCCE_server_add_user(int );
void SRCCE_server_display_user(int );

void SRCCE_server_add_remove_question(int );
void SRCCE_server_add_question(int );
void SRCCE_server_remove_question(int );

void SRCCE_server_add_remove_testcase(int );
void SRCCE_server_update_testcase(int );

void SRCCE_server_user_process(int , char *);
void SRCCE_server_select_question(int , char *);
void SRCCE_server_display_questions(int );
void SRCCE_server_see_result(int , char *, char *);
void SRCCE_server_compile_run(int , char *, char *);
void SRCCE_server_edit_question(int , char *, char *);
void SRCCE_server_run_code(int , char *, char *);
int SRCCE_server_check_warning_error(char *, char *, char *);

void SRCCE_server_create_log( char *, char *);
char* SRCCE_server_get_date_time();

MYSQL *my_sql_init();


#endif /* SRCCE_SERVER_HEADER_H */ 
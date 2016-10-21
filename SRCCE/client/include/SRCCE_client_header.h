/**************************************************************************************
*
*  FILE NAME	: SRCCE_client_header.h
*
*  DESCRIPTION: This File contains the required function prototypes required for SRCCE
*  system. It also contains the required header files. It contains the macros.
*
*************************************************************************************/
#ifndef SRCCE_CLIENT_HEADER_H 
#define SRCCE_CLIENT_HEADER_H 

/* Header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <ctype.h>
#include <time.h>

/* Macros used */
#define MSG 2000
#define INT_LEN 4
#define CIPHER 5
#define USERNAME_LEN 25
#define PASSWORD_LENGTH 25
#define CHOICE_SIZE 1000
#define Q_NAME 8
#define Q_DESCRIPTION 125
#define FILESIZE 64000
#define INVALID_USER 0
#define IS_ADMIN 1
#define LOGGED_IN 2
#define USER_LOGGED_IN 3
#define ADMIN_LOGGED_IN 4
#define IS_USER 5
#define INVALID_LEN 0
#define VALID_LEN 1
#define INVALID_FILESIZE 0
#define VALID_FILESIZE 1
#define INVALID_CHOICE 0
#define DIGIT_CHOICE 1
#define CHAR_CHOICE 2
#define CONNECT_ERROR -1
#define PORT 6
#define IP 16
#define INVALID_QUESTION 0
#define VALID_QUESTION 1
#define RECEIVE_FAIL 0
#define RECEIVE_SUCCESS 1
#define TRUE 1
#define NEG -1
#define ZERO 0
#define ONE 1
#define TWO 2
#define THREE 3
#define FOUR 4
#define FIVE 5

/* Function prototypes */
int SRCCE_client_create_socket(int , int , int , int );

int SRCCE_client_check_filesize(char *);
int SRCCE_client_validate_choice(char *);
int SRCCE_client_validate_string_length(char *, int );

void SRCCE_client_send_message(int , char *);
void SRCCE_client_receive_message(int , char *);

void SRCCE_client_send_file(int , char *);
void SRCCE_client_receive_file(int , char *);
int SRCCE_client_receive_file_compile(int ,	char *);

void SRCCE_client_encrypt( char *, char *);
void SRCCE_client_decrypt( char *, char *);

char *SRCCE_client_password_mask();

void SRCCE_client_admin_process(int );
void SRCCE_client_display_admin_menu();

void SRCCE_client_add_remove_user(int );
void SRCCE_client_remove_user(int );
void SRCCE_client_add_user(int );
void SRCCE_client_display_user(int );

void SRCCE_client_add_remove_question(int );
void SRCCE_client_add_question(int );
void SRCCE_client_remove_question(int );

void SRCCE_client_add_remove_testcase(int );
void SRCCE_client_update_testcase(int );

void SRCCE_client_user_process(int , char *);
void SRCCE_client_select_question(int );
void SRCCE_client_display_questions(int );
void SRCCE_client_see_result(int );
void SRCCE_client_compile_run(int );
void SRCCE_client_edit_question(int );

void SRCCE_client_create_log( char *, char *);
char* SRCCE_client_get_date_time();
void SRCCE_client_initialize(char *,char *);

	
#endif /* SRCCE_CLIENT_HEADER_H */ 
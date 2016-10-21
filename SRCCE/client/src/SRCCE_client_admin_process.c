/**************************************************************************************
*
*  FILE NAME	: SRCCE_client_admin_process.c
*
*  DESCRIPTION: This file contains the function for admin porcess.
*
*************************************************************************************/

#include "SRCCE_client_header.h"

/***************************************************************************
 * FUNCTION NAME: SRCCE_client_display_admin_menu
 *
 * DESCRIPTION: Displays or prints the menu to the admin.
 * 	
 * ARGUMENTS: socket: socket descriptor.
 *
 * RETURNS: Nothing
 *	   
***************************************************************************/
void SRCCE_client_display_admin_menu()
{
	printf("\n --------------------MAIN MENU-------------------\n");
	printf("\n	1.ADD/REMOVE USER");
	printf("\n	2.ADD/REMOVE QUESTION");
	printf("\n	3.ADD/REMOVE TEST CASE");
	printf("\n	4.LOGOFF\n");
	printf("\n enter choice:");	
	fflush(stdout);
	SRCCE_client_create_log(" --------------------MAIN MENU-------------------", "admin"); 
	SRCCE_client_create_log( "	1.ADD/REMOVE USER", "admin"); 
	SRCCE_client_create_log("	2.ADD/REMOVE QUESTION", "admin"); 
	SRCCE_client_create_log( "	3.ADD/REMOVE TEST CASE", "admin"); 
	SRCCE_client_create_log("	4.LOGOFF\n", "admin"); 
	SRCCE_client_create_log(" enter choice:", "admin"); 

}
/***************************************************************************
 * FUNCTION NAME: SRCCE_client_admin_process
 *
 * DESCRIPTION: Gives the main menu to the admin where he can choose among
 * the various options provided to him.
 *
 * ARGUMENTS: socket: socket descriptor.
 * 	
 * RETURNS: Nothing
 *	   
***************************************************************************/
void SRCCE_client_admin_process(int socket)
{
	char admin_choice[CHOICE_SIZE] = {ZERO};
		
	int valid = INVALID_CHOICE;
	
	printf("\n-------------------WELCOME ADMIN------------------------\n");
	fflush(stdout);
	do
	{
		SRCCE_client_display_admin_menu();
		/* get the choice from the admin*/
 
		scanf(" %[^\n]",admin_choice);
		
		valid = SRCCE_client_validate_choice(admin_choice);
		
		if( INVALID_CHOICE == valid || CHAR_CHOICE == valid )
		{
			printf("\n Invalid choice \n Please re-enter \n");
			fflush(stdout);
			SRCCE_client_create_log(" Invalid choice...Please re-enter ", "admin");
			sleep(1);
		}
		else
		{	
			/* send admin choice to server */
			SRCCE_client_send_message(socket,admin_choice);
			
			switch(atoi(admin_choice))
			{
				case ONE: SRCCE_client_add_remove_user(socket);
						break;
				case TWO: SRCCE_client_add_remove_question(socket);
						break;
				case THREE: SRCCE_client_add_remove_testcase(socket);
						break;
				case FOUR: printf("\nLOGOFF\n");
						fflush(stdout);
						SRCCE_client_create_log(" LOGOFF", "admin");
						break;
						
				default: printf("\nInvalid choice\n");
						fflush(stdout);
						SRCCE_client_create_log(" Invalid choice", "admin");
			}
									
		}
		system("clear");
					
	}while(FOUR != atoi(admin_choice));
}
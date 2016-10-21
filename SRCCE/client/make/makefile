########################################################################
#       _________________________________________________________ 	  	#
#       |                                                       |       #
#       |                                                       |       #
#       |               Make File for SRCCE_CLIENT 				|       #
#       |                                                       |       #
#       |_______________________________________________________|       #
#                                                                       #
#########################################################################

CC=gcc
CFLAGS=-c -Wall -g 
FLAGS=-o
VFLAGS=--tool=memcheck --leak-check=yes --show-reachable=yes -v --max-stackframe=10242080
INCLUDE=-I../include/
HEADER=../include/
OBJ=../obj/
SRC=../src/
OUTPUT=../bin/



$(OUTPUT)client: $(OBJ)SRCCE_client_main.o $(OBJ)SRCCE_client_utilities.o $(OBJ)SRCCE_client_admin_process.o $(OBJ)SRCCE_client_add_remove_user.o $(OBJ)SRCCE_client_add_remove_question.o $(OBJ)SRCCE_client_add_remove_testcase.o $(OBJ)SRCCE_client_user_process.o
	$(CC) $(FLAGS) client $(OBJ)SRCCE_client_main.o $(OBJ)SRCCE_client_utilities.o $(OBJ)SRCCE_client_admin_process.o $(OBJ)SRCCE_client_add_remove_user.o $(OBJ)SRCCE_client_add_remove_question.o $(OBJ)SRCCE_client_add_remove_testcase.o $(OBJ)SRCCE_client_user_process.o
	mv client ../bin/

$(OBJ)SRCCE_client_main.o: $(SRC)SRCCE_client_main.c $(HEADER)SRCCE_client_header.h
	$(CC)  -DTRACING $(INCLUDE) $(CFLAGS) $(SRC)SRCCE_client_main.c
	mv SRCCE_client_main.o ../obj/

$(OBJ)SRCCE_client_utilities.o: $(SRC)SRCCE_client_utilities.c $(HEADER)SRCCE_client_header.h 
	$(CC) -DTRACING $(INCLUDE) $(CFLAGS) $(SRC)SRCCE_client_utilities.c
	mv SRCCE_client_utilities.o ../obj/


$(OBJ)SRCCE_client_admin_process.o: $(SRC)SRCCE_client_admin_process.c $(HEADER)SRCCE_client_header.h 
	$(CC) -DTRACING $(INCLUDE) $(CFLAGS) $(SRC)SRCCE_client_admin_process.c
	mv SRCCE_client_admin_process.o ../obj/

$(OBJ)SRCCE_client_add_remove_user.o: $(SRC)SRCCE_client_add_remove_user.c $(HEADER)SRCCE_client_header.h 
	$(CC) -DTRACING $(INCLUDE) $(CFLAGS) $(SRC)SRCCE_client_add_remove_user.c
	mv SRCCE_client_add_remove_user.o ../obj/	

$(OBJ)SRCCE_client_add_remove_question.o: $(SRC)SRCCE_client_add_remove_question.c $(HEADER)SRCCE_client_header.h 
	$(CC) -DTRACING $(INCLUDE) $(CFLAGS) $(SRC)SRCCE_client_add_remove_question.c
	mv SRCCE_client_add_remove_question.o ../obj/

$(OBJ)SRCCE_client_add_remove_testcase.o: $(SRC)SRCCE_client_add_remove_testcase.c $(HEADER)SRCCE_client_header.h 
	$(CC) -DTRACING $(INCLUDE) $(CFLAGS) $(SRC)SRCCE_client_add_remove_testcase.c
	mv SRCCE_client_add_remove_testcase.o ../obj/

$(OBJ)SRCCE_client_user_process.o: $(SRC)SRCCE_client_user_process.c $(HEADER)SRCCE_client_header.h 
	$(CC) -DTRACING $(INCLUDE) $(CFLAGS) $(SRC)SRCCE_client_user_process.c
	mv SRCCE_client_user_process.o ../obj/
	
valgrind: $(OUTPUT)client
	G_SLICE=always-malloc valgrind $(VFLAGS) ./$(OUTPUT)client

splint: $(SRC)SRCCE_client_main.c $(SRC)SRCCE_client_utilities.c $(SRC)SRCCE_client_admin_process.c $(SRC)SRCCE_client_add_remove_user.c $(SRC)SRCCE_client_add_remove_question.c $(SRC)SRCCE_client_add_remove_testcase.c $(SRC)SRCCE_client_user_process.c
	splint $(SRC)SRCCE_client_main.c $(SRC)SRCCE_client_utilities.c $(SRC)SRCCE_client_admin_process.c $(SRC)SRCCE_client_add_remove_user.c $(SRC)SRCCE_client_add_remove_question.c $(SRC)SRCCE_client_add_remove_testcase.c $(SRC)SRCCE_client_user_process.c $(INCLUDE)

gcov_compile:
	$(CC) -g $(FLAGS) $(OUTPUT)client -fprofile-arcs -ftest-coverage $(SRC)SRCCE_client_main.c $(SRC)SRCCE_client_utilities.c $(SRC)SRCCE_client_admin_process.c $(SRC)SRCCE_client_add_remove_user.c $(SRC)SRCCE_client_add_remove_question.c $(SRC)SRCCE_client_add_remove_testcase.c $(SRC)SRCCE_client_user_process.c $(INCLUDE)

gcov_run:
	$(OUTPUT)client

gcov:
	gcov $(SRC)SRCCE_client_main.c
	gcov $(SRC)SRCCE_client_utilities.c
	gcov $(SRC)SRCCE_client_admin_process.c
	gcov $(SRC)SRCCE_client_add_remove_user.c
	gcov $(SRC)SRCCE_client_add_remove_question.c
	gcov $(SRC)SRCCE_client_add_remove_testcase.c
	gcov $(SRC)SRCCE_client_user_process.c

gcov_clean:
	rm *.gcda
	rm *.gcno
	rm *.gcov
	
clean:
	rm $(OBJ)*.o
	rm $(OUTPUT)client
########################################################################
#       _________________________________________________________ 	  	#
#       |                                                       |       #
#       |                                                       |       #
#       |               Make File for SRCCE_SERVER 				|       #
#       |                                                       |       #
#       |_______________________________________________________|       #
#                                                                       #
#########################################################################

CC=gcc
CFLAGS=-c -Wall -g -lpthread 
FLAGS=-o
VFLAGS=--tool=memcheck --leak-check=yes --show-reachable=yes -v --max-stackframe=10242080
INCLUDE=-I../include/
HEADER=../include/
OBJ=../obj/
SRC=../src/
OUTPUT=../bin/
SQL_CFLAGS= $(mysql_config --cflags)
SQL_LIBS=$(mysql_config --libs)

$(OUTPUT)server: $(OBJ)SRCCE_server_main.o $(OBJ)SRCCE_server_utilities.o $(OBJ)SRCCE_server_login.o $(OBJ)SRCCE_server_admin_process.o $(OBJ)SRCCE_server_add_remove_user.o $(OBJ)SRCCE_server_add_remove_question.o $(OBJ)SRCCE_server_add_remove_testcase.o $(OBJ)SRCCE_server_user_process.o
	$(CC) $(FLAGS) server -lmysqlclient $(OBJ)SRCCE_server_main.o $(OBJ)SRCCE_server_utilities.o $(OBJ)SRCCE_server_login.o $(OBJ)SRCCE_server_admin_process.o $(OBJ)SRCCE_server_add_remove_user.o $(OBJ)SRCCE_server_add_remove_question.o $(OBJ)SRCCE_server_add_remove_testcase.o $(OBJ)SRCCE_server_user_process.o `mysql_config --cflags --libs`
	mv server ../bin/

$(OBJ)SRCCE_server_main.o: $(SRC)SRCCE_server_main.c $(HEADER)SRCCE_server_header.h
	$(CC)  -DTRACING $(INCLUDE) $(CFLAGS) $(SRC)SRCCE_server_main.c
	mv SRCCE_server_main.o ../obj/

$(OBJ)SRCCE_server_utilities.o: $(SRC)SRCCE_server_utilities.c $(HEADER)SRCCE_server_header.h 
	$(CC) -DTRACING $(INCLUDE) $(CFLAGS) $(SRC)SRCCE_server_utilities.c
	mv SRCCE_server_utilities.o ../obj/

$(OBJ)SRCCE_server_login.o: $(SRC)SRCCE_server_login.c $(HEADER)SRCCE_server_header.h 
	$(CC) -DTRACING $(INCLUDE) $(CFLAGS) $(SRC)SRCCE_server_login.c
	mv SRCCE_server_login.o ../obj/
	
$(OBJ)SRCCE_server_admin_process.o: $(SRC)SRCCE_server_admin_process.c $(HEADER)SRCCE_server_header.h 
	$(CC) -DTRACING $(INCLUDE) $(CFLAGS) $(SRC)SRCCE_server_admin_process.c
	mv SRCCE_server_admin_process.o ../obj/

$(OBJ)SRCCE_server_add_remove_user.o: $(SRC)SRCCE_server_add_remove_user.c $(HEADER)SRCCE_server_header.h 
	$(CC) -DTRACING $(INCLUDE) $(CFLAGS) $(SRC)SRCCE_server_add_remove_user.c
	mv SRCCE_server_add_remove_user.o ../obj/	

$(OBJ)SRCCE_server_add_remove_question.o: $(SRC)SRCCE_server_add_remove_question.c $(HEADER)SRCCE_server_header.h 
	$(CC) -DTRACING $(INCLUDE) $(CFLAGS) $(SRC)SRCCE_server_add_remove_question.c
	mv SRCCE_server_add_remove_question.o ../obj/

$(OBJ)SRCCE_server_add_remove_testcase.o: $(SRC)SRCCE_server_add_remove_testcase.c $(HEADER)SRCCE_server_header.h 
	$(CC) -DTRACING $(INCLUDE) $(CFLAGS) $(SRC)SRCCE_server_add_remove_testcase.c
	mv SRCCE_server_add_remove_testcase.o ../obj/

$(OBJ)SRCCE_server_user_process.o: $(SRC)SRCCE_server_user_process.c $(HEADER)SRCCE_server_header.h 
	$(CC) -DTRACING $(INCLUDE) $(CFLAGS) $(SRC)SRCCE_server_user_process.c
	mv SRCCE_server_user_process.o ../obj/
	
valgrind: $(OUTPUT)server
	G_SLICE=always-malloc valgrind $(VFLAGS) ./$(OUTPUT)server
	
splint:
	splint $(SRC)SRCCE_server_main.c $(SRC)SRCCE_server_utilities.c $(SRC)SRCCE_server_login.c $(SRC)SRCCE_server_admin_process.c $(SRC)SRCCE_server_add_remove_user.c $(SRC)SRCCE_server_add_remove_question.c $(SRC)SRCCE_server_add_remove_testcase.c $(SRC)SRCCE_server_user_process.c $(INCLUDE)

gcov_compile:
	$(CC) $(FLAGS) $(OUTPUT)server -fprofile-arcs -ftest-coverage $(SRC)SRCCE_server_main.c $(SRC)SRCCE_server_utilities.c $(SRC)SRCCE_server_login.c $(SRC)SRCCE_server_admin_process.c $(SRC)SRCCE_server_add_remove_user.c $(SRC)SRCCE_server_add_remove_question.c $(SRC)SRCCE_server_add_remove_testcase.c $(SRC)SRCCE_server_user_process.c $(INCLUDE) `mysql_config --cflags --libs`

gcov_run:
	$(OUTPUT)server

gcov:
	gcov $(SRC)SRCCE_server_main.c
	gcov $(SRC)SRCCE_server_utilities.c
	gcov $(SRC)SRCCE_server_login.c
	gcov $(SRC)SRCCE_server_admin_process.c
	gcov $(SRC)SRCCE_server_add_remove_user.c
	gcov $(SRC)SRCCE_server_add_remove_question.c
	gcov $(SRC)SRCCE_server_add_remove_testcase.c
	gcov $(SRC)SRCCE_server_user_process.c

gcov_clean:
	rm *.gcda
	rm *.gcno
	rm *.gcov

clean:
	rm $(OBJ)*.o
	rm $(OUTPUT)server
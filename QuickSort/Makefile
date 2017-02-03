#----------------------------------------------------------------------
# CS415/515 Assignment 1
#----------------------------------------------------------------------

FLAGS = -std=c99 -pthread -g -DVERBOSE
CC = gcc

hw1:
	$(CC) $(FLAGS) -o qsort-pthd qsort-pthd.c task-queue.c

debug:
	$(CC) $(FLAGS) -D DEBUG -o qsort-pthd qsort-pthd.c task-queue.c


%: task-queue.h task-queue.c %.c 
	$(CC) $(FLAGS) -o $@ task-queue.c $@.c
	$(CC) $(FLAGS) -o $@ qsort-pthd.c $@.c


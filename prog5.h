/*
 *	File:		prog5.h
 *	Programmer:	Waber, Vaan Wulfgang
 *	Course:		CSCI 4727-001
 *	Assignment:	Homework 5
 *	Date:		April 18, 2012
 *	Purpose:	
 */

//Standard includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
using std::cin;

//Project specific includes
#include <pthread.h>
#include <semaphore.h>
#include <queue>
using std::queue;

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <unistd.h>
#include <sys/wait.h>

//Function definitions
char* getCLFlagParam(int, char**, char*);
bool isnumeric(char* str);

//Struct definitions
struct inputStruct
	{
	char str[256];
	int len;
	int flag;
	};

struct tokenStruct
	{
	char token[256];
	int tokenType;
	int flag;
	};
	
struct resourceStruct
	{
	int connection;
	
	queue<inputStruct> qIS0;
	queue<tokenStruct> qTS0;
	queue<tokenStruct> qTS1;
	queue<tokenStruct> qTS2;
	
	pthread_mutex_t mu_qIS0;
	pthread_mutex_t mu_qTS0;
	pthread_mutex_t mu_qTS1;
	pthread_mutex_t mu_qTS2;
	
	sem_t sem_qIS0;
	sem_t sem_qTS0;
	sem_t sem_qTS1;
	sem_t sem_qTS2;
	};

//Macros to simplify the Pushing and Popping from the Queues
//Bulids in necessary mutex and semaphor functionality
#define PUSH(Q, MU, SM, ST)\
	pthread_mutex_lock(&MU);\
		Q.push(ST);\
	pthread_mutex_unlock(&MU);\
	sem_post(&SM);

#define POP(Q, MU, SM, ST)\
	sem_wait(&SM);\
	pthread_mutex_lock(&MU);\
		ST = Q.front();\
		Q.pop();\
	pthread_mutex_unlock(&MU);


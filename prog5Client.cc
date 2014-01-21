/*
 *	File:		prog5Client.cc
 *	Programmer:	Waber, Vaan Wulfgang
 *	Course:		CSCI 4727-001
 *	Assignment:	Homework 5
 *	Date:		April 18, 2012
 *	Purpose:	
 */

#include "prog5.h"

//Gets lines of input from user and places inputStrucst on queue
void* fnChild0(void*);
//Sends inputStructs to Server
void* fnChild1(void*);
//Recieves processed tokenStructs from Server
void* fnChild2(void*);

//Initialization of Queues,
queue<inputStruct> qIS0;

//Initialization of mutexes 
pthread_mutex_t mu_qIS0;

//Declaration of semaphores
sem_t sem_qIS0;


int main(int argc, char** argv)
	{
	struct addrinfo* myinfo;
	int sockdesc;
	char hostname[81];
	char portnum[81];
	char buffer[81];
	int connection;

	strcpy(hostname, "0.0.0.0");
	strcpy(portnum, "1024");

	//Checks for portnumber parameter, default otherwise
	char strFlag[] = "-p";
	char* strTMP = getCLFlagParam(argc, argv, strFlag);
        if(strTMP)
                {
                strcpy(portnum, strTMP);
                }

	//Checks for server address as parameter, default otherwise
	strcpy(strFlag, "-s");
	strTMP = getCLFlagParam(argc, argv, strFlag);
        if(strTMP)
                {
                strcpy(hostname, strTMP);
                }

	sockdesc = socket(AF_INET, SOCK_STREAM, 0);
	if(sockdesc < 0)
		{
		printf("%s\n", "Error creating socket!");
		exit(0);
		}
	
	if(getaddrinfo(hostname, portnum, NULL, &myinfo))
		{
		printf("%s\n", "Error getting address!");
		exit(0);
		}

	connection = connect(sockdesc, myinfo->ai_addr, myinfo->ai_addrlen);
	if(connection < 0)
		{
		printf("%s\n", "Error in connect!");
		exit(0);
		}

	//Initialization of mutex(s)
	pthread_mutex_init(&mu_qIS0, NULL);

	//Initialization of semaphores
	sem_init(&sem_qIS0, 0, 0);	

	//Declaration of thread referece variables
	pthread_t child0, child1, child2, child3;

	//Thread creation
	pthread_create(&child0, NULL, fnChild0, NULL);
	pthread_create(&child1, NULL, fnChild1, (void*)&sockdesc);
	pthread_create(&child2, NULL, fnChild2, (void*)&sockdesc);

	//Waits for each thread to die
	pthread_join(child0, NULL);
	pthread_join(child1, NULL);
	pthread_join(child2, NULL);

	return(0);
	}

void* fnChild0(void*)
	{
	char strBuffer[256];
	
	struct inputStruct sInput;

	//Continues getting lines of input from std while a blank line has not been input
	while(cin.getline(strBuffer, 256) && cin.gcount() > 1)
		{	
		strcpy(sInput.str, strBuffer);
		sInput.len = strlen(strBuffer);
		sInput.flag = 0;

		//Pushes the struct containg a line of input to a queue to pass to a sibling
		PUSH(qIS0, mu_qIS0, sem_qIS0, sInput);

		}

	sInput.flag = 1;

	//Pushes struct signifying end of program
	PUSH(qIS0, mu_qIS0, sem_qIS0, sInput);

	//Kills the thread
	pthread_exit(0);
	}

void* fnChild1(void* arg)
	{
	int sockdesc = *((int*)arg);

	struct inputStruct sInput;
	
	POP(qIS0, mu_qIS0, sem_qIS0, sInput);
	
	while(!sInput.flag)
		{
		write(sockdesc, (char*)&sInput, sizeof(sInput));
		POP(qIS0, mu_qIS0, sem_qIS0, sInput);
		}
	
	write(sockdesc, (char*)&sInput, sizeof(sInput));
	pthread_exit(0);
	}

void* fnChild2(void* arg)
	{
	
	int sockdesc = *((int*)arg);	

	struct tokenStruct sToken;

	read(sockdesc, (char*)&sToken, sizeof(sToken));	
	
	while(!sToken.flag)
		{				
		printf("%i: %s\n", sToken.tokenType, sToken.token);
		
		read(sockdesc, (char*)&sToken, sizeof(sToken));		
		}
	
	pthread_exit(0);
	}

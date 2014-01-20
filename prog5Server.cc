/*
 *	File:		prog5Server.cc
 *	Programmer:	Waber, Vaan Wulfgang
 *	Course:		CSCI 4727-001
 *	Assignment:	Homework 5
 *	Date:		April 18, 2012
 *	Purpose:	
 */

#include "prog5.h"

//Recieves inputStruct from client and places on queue
void* fnChild0(void*);
//Dequeues inputStructs, tokenizes, and places tokenStructs on queue
void* fnChild1(void*);
//Categorises and populates inputStruct.tokenType, places on next queue
void* fnChild2(void*);
//Sends tokenStructs to client, places on additional queue
void* fnChild3(void*);
//Counts tokens on final queue
void* fnChild4(void*);

int main(int argc, char** argv)
	{
	int sockdesc;
	struct addrinfo* myinfo;
	char strPortNum[81];
	int iPortNum = 1024;
	char buffer[40];
	int connection;

	//Checks for portnumber as parameter, replaces default 1024 if found
	char strFlag[] = "-p";
	char* strTMP = getCLFlagParam(argc, argv, strFlag);
	if(strTMP)
		{
		sscanf(strTMP, "%i", &iPortNum);
		}

	//Increments ports while trying to register a socket, breaks upon success
	for(; iPortNum < 49151; iPortNum++)
		{
		sprintf(strPortNum, "%i", iPortNum);

		sockdesc = socket(AF_INET, SOCK_STREAM, 0);
		if(sockdesc < 0)
			{
			printf("%s\n", "Error creating socket!");
			exit(0);
			}

		if(getaddrinfo("0.0.0.0", strPortNum, NULL, &myinfo))
			{
			printf("%s\n", "Error getting address!");
			exit(0);		
			}

		if(bind(sockdesc, myinfo->ai_addr, myinfo->ai_addrlen) >= 0)
			{
			printf("%s%i\n", "Successfully registered on Port ", iPortNum);
			break;
			}
		}

	//Attempts to listen at socket
	if(listen(sockdesc, 1) < 0)
		{
		printf("%s\n", "Error in listen");
		exit(0);		
		}

	//Infinite loop to handle clients heard by listening
	while(1)
		{
		connection = accept(sockdesc, NULL, NULL);
		if(connection < 0)
			{
			printf("%s\n", "Error getting address!");
			exit(0);		
			}
		//Resources allocated and threads spawns to handle incomming connection
		else
			{
			printf("%s\n", "CONNECTION!!!");
			
			struct resourceStruct* sR = new resourceStruct();
			
			sR->connection = connection;
			
			sem_init(&sR->sem_qIS0, 0, 0);
			sem_init(&sR->sem_qTS0, 0, 0);
			sem_init(&sR->sem_qTS1, 0, 0);
			sem_init(&sR->sem_qTS2, 0, 0);
				
			pthread_mutex_init(&sR->mu_qIS0, NULL);
			pthread_mutex_init(&sR->mu_qTS0, NULL);
			pthread_mutex_init(&sR->mu_qTS1, NULL);
			pthread_mutex_init(&sR->mu_qTS2, NULL);
			
			pthread_t child0, child1, child2, child3, child4;
			
			pthread_create(&child0, NULL, fnChild0, (void*)sR);
			pthread_create(&child1, NULL, fnChild1, (void*)sR);
			pthread_create(&child2, NULL, fnChild2, (void*)sR);
			pthread_create(&child3, NULL, fnChild3, (void*)sR);
			pthread_create(&child3, NULL, fnChild4, (void*)sR);
			}
		}

	exit(0);
	}

void* fnChild0(void* arg)
	{
	pthread_detach(pthread_self());
	
	struct resourceStruct* sR = (resourceStruct*)arg;

	struct inputStruct sInput;

	read(sR->connection, (char*)&sInput, sizeof(sInput));
	
	
	while(!sInput.flag)
		{
		PUSH(sR->qIS0, sR->mu_qIS0, sR->sem_qIS0, sInput);
		read(sR->connection, (char*)&sInput, sizeof(sInput));
		}

	
	PUSH(sR->qIS0, sR->mu_qIS0, sR->sem_qIS0, sInput);
	pthread_exit(0);
	}	
	
void* fnChild1(void* arg)
	{
	pthread_detach(pthread_self());
	
	struct resourceStruct* sR = (resourceStruct*)arg;	

	struct inputStruct sInput;
	struct tokenStruct sToken;

	//Pops a struck from the queue to be processed
	POP(sR->qIS0, sR->mu_qIS0, sR->sem_qIS0, sInput);

	char* strToken;
	char strDelims[] = " \t\f\n\r\v";

	//Processes structs until end struct is found
	while(!sInput.flag)
		{
	
		//Splits string into tokens and processes each token
		strToken = strtok(sInput.str, strDelims);
		while(strToken != NULL)
			{
			//Populates struct to pass to Child1
			strcpy(sToken.token, strToken);
			sToken.flag = 0;
			
			//Writes struct to queue for a sibling to read
			PUSH(sR->qTS0, sR->mu_qTS0, sR->sem_qTS0, sToken);
			
			strToken = strtok(NULL, strDelims);
			}

		//Reads in next struct from parent
		POP(sR->qIS0, sR->mu_qIS0, sR->sem_qIS0, sInput);
		}
		
	//Pushes a struct signaling end of input
	sToken.flag = 1;	
	PUSH(sR->qTS0, sR->mu_qTS0, sR->sem_qTS0, sToken);

	//Kills the thread
	pthread_exit(0);

	}

void* fnChild2(void* arg)
	{
	pthread_detach(pthread_self());
	
	struct resourceStruct* sR = (resourceStruct*)arg;	

	struct tokenStruct sToken;
	char strType0[] = ";)(";
	char strType1[] = "*+-/";

	//Reads in first struct for processing
	POP(sR->qTS0, sR->mu_qTS0, sR->sem_qTS0, sToken);
	
	//Processes structs until end struct is found
	while(!sToken.flag)
		{		

		//Block to categorize the current token	
		if(isnumeric(sToken.token))
			{
			sToken.tokenType = 2;		
			}
		else if(sToken.token[1] == '\0'
			&& strchr(strType0, sToken.token[0]))
			{
			sToken.tokenType = 0;
			}	
		else if(sToken.token[1] == '\0'
			&& strchr(strType1, sToken.token[0]))
			{
			sToken.tokenType = 1;
			}
		else
			{
			sToken.tokenType = 3;
			}

		//Pushes token struct to the queue for a sibling to process
		PUSH(sR->qTS1, sR->mu_qTS1, sR->sem_qTS1, sToken);

		//Reads in next struct
		POP(sR->qTS0, sR->mu_qTS0, sR->sem_qTS0, sToken);
		}
	
	//Pushes a struct signaling end of input
	PUSH(sR->qTS1, sR->mu_qTS1, sR->sem_qTS1, sToken);

	//Kills the thread
	pthread_exit(0);	
	}
	
void* fnChild3(void* arg)
	{
	pthread_detach(pthread_self());

	struct resourceStruct* sR = (resourceStruct*)arg;
	
	struct tokenStruct sToken;
	
	POP(sR->qTS1, sR->mu_qTS1, sR->sem_qTS1, sToken);
	
	while(!sToken.flag)
		{
		write(sR->connection, (char*)&sToken, sizeof(sToken));
		PUSH(sR->qTS2, sR->mu_qTS2, sR->sem_qTS2, sToken);
		POP(sR->qTS1, sR->mu_qTS1, sR->sem_qTS1, sToken);
		}
	
	write(sR->connection, (char*)&sToken, sizeof(sToken));	
	PUSH(sR->qTS2, sR->mu_qTS2, sR->sem_qTS2, sToken);

	//Kills the thread
	pthread_exit(0);	
	}
	
void* fnChild4(void* arg)
	{
	pthread_detach(pthread_self());

	struct resourceStruct* sR = (resourceStruct*)arg;		

	int tCount0 = 0;
	int tCount1 = 0;
	int tCount2 = 0;
	int tCount3 = 0;

	struct tokenStruct sToken;

	//Pops struct for processing
	POP(sR->qTS2, sR->mu_qTS2, sR->sem_qTS2, sToken);

	//Sums the amount of each token type
	while(!sToken.flag)
		{
		switch(sToken.tokenType)
			{
			case 0:
				tCount0 ++;
				break;
			case 1:
				tCount1 ++;
				break;
			case 2: 
				tCount2 ++;
				break;
			case 3: 
				tCount3 ++;
			}

		//Pops next struct for processing
		POP(sR->qTS2, sR->mu_qTS2, sR->sem_qTS2, sToken);
		}

	//Final output displaying the total amount of each token type
	printf("T0: %i\nT1: %i\nT2: %i\nT3: %i\n\n", tCount0, tCount1, tCount2, tCount3);

	//Kills the thread
	pthread_exit(0);	
	}
	
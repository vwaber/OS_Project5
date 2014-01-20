/*
 *  File:       library.cc
 * 	Programmer:	Waber, Vaan Wulfgang
 * 	Course:    	CSCI 4727-001
 * 	Assignment:	Homework 5
 *  Date:		April 18, 2012
 *	Purpose:
*/

#include <string.h>

char* getCLFlagParam(int argc, char** argv, char* param)
        {
        for(int i = 1; i < argc; i++)
                {
                if(strcmp(argv[i-1], param) == 0)
                        {
                        return(argv[i]);
                        break;
                        }
                }
        return(NULL);
        }

bool isnumeric(char* str)
        {
        if(!*str)return(0);

        while(*str)
                {
                if(*str < '0' || *str > '9')
                        {
                        return(0);
                        }
                str++;
                }
        return(1);
        }


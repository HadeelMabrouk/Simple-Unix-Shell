#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

int commandParser(char *, char **, int,int*,int*,int*,int*);

#define MAX_LINE		80 /* 80 chars per line, per command */


//a function to parse the user input and tokenize it
int commandParser(char* command, char** arguments,int argNo, int* conF,int* inpF, int* outpF, int* pipeF)
{
	//history support implementation
	if(command[0]=='!' && command[1]=='!')
	{
		
		if (command[2]!='\n')
		{
			printf("Please, insert a correct command.\n");
			return -2;
		}
		else if(argNo==-2) //argNo = -2 is the default value at the beginning of the program start
		{
			printf("No commands in history.\n");
			return -2;
		}
		else
		{	//to echo the last command when a !! command is typed
			printf("osh>");
			for(int i=0;i<argNo;i++)
			{
				printf("%s ",arguments[i]);
			}
			printf("\n");
			return argNo;
		}
	}
	int i=0;
	int c=0;
	int a=0;
	char temp[MAX_LINE];
	while (command[i]!='\n' && command[i]!=0)
	{
		if((command[i]==' ' || command[i]=='\t') && command[i+1]!=' ' && command[i+1]!='\t') //to store the value of the argument when a space is read
		{
			arguments[a]= (char*) malloc((c+2)*sizeof(char));
			temp[c]=0;
			strcpy(arguments[a],temp);
			a++;
			c=0;
		}
		else if (command[i]!=' ' && command[i]!='\t')
		{
			temp[c++]=command[i]; //store the characters of the arguments in the temp variable
		}
		i++;
	}
	arguments[a]= (char*) malloc((c+2)*sizeof(char)); //to store the last argument
	temp[c]='\0';
	strcpy(arguments[a],temp);
	a++;
	arguments[a]=NULL; //to store the last argument to be equal to NULL
	int x = checkForFlags(arguments,a,conF,inpF,outpF,pipeF);
	return a;
}

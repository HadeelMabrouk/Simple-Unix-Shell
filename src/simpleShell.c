#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

int checkForFlags(char** ,int, int* ,int* , int* , int* );
int removeFlags(char**,int);
int commandParser(char *, char **, int,int*,int*,int*,int*);
int pipeComm(char **, int, int);


#define MAX_LINE		80 /* 80 chars per line, per command */

int pipeComm(char ** args, int argsNumber, int pipeF)
{
	int pid;
	int fd[2];

	if(pipe(fd)==-1)
	{
		printf("Error: Pipe Failed.\n");
		return 1;
	}
	pid= fork();
	if(pid<0)
	{
		printf("Error: Forking Failed.\n");
		return 1;
	}
	else if (pid==0) //child
	{
		close(fd[0]);
		int stdout = dup(STDOUT_FILENO);
		dup2(fd[1],STDOUT_FILENO);
		char* firstArgs[MAX_LINE/2+1]; //to store only the arguments of the child before the | symbol to be executed
		for(int i = 0; i < MAX_LINE/2+1; i++)
		{
			firstArgs[i]=NULL;
		}
		for(int i = 0; i < pipeF; i++)
		{
			firstArgs[i] = (char*) malloc(sizeof(args[i]));
			strcpy(firstArgs[i],args[i]);
		}
		//firstArgs[pipeF]=NULL;
		execvp(firstArgs[0], firstArgs);

		dup2(stdout,STDOUT_FILENO);
		printf("Command Failed.\n");
		return 1;
	}
	else //parent
	{
		close(fd[1]);
		dup2(fd[0],STDIN_FILENO);
		waitpid(pid, NULL, 0);

		char* secondArgs[MAX_LINE/2+1];//to store only the arguments of the parent after the | symbol to be executed
		for(int i = 0; i< MAX_LINE/2+1; i++)
		{
			secondArgs[i] = NULL;
		}
		for(int i = 0; i< argsNumber-pipeF-1; i++)
		{
			secondArgs[i] = (char*) malloc(sizeof(args[pipeF+i+1]));
			strcpy(secondArgs[i],args[pipeF + i+1]);
		}
		//secondArgs[argsNumber-pipeF]=NULL;
		execvp(secondArgs[0], secondArgs);
		printf("Command Failed.\n");
		return 1;
	}
	return 0;
}

int removeFlags(char** args,int argsNo) //to remove the flags from the arguments
{
	char *argsN[argsNo];
	for (int i=0;i<argsNo;i++)
	{
		argsN[i]=NULL;
	}
	int j=0;
	for(int i=0;i<argsNo;i++) //this loop to store only the arugments other than the flags
	{
		if(strcmp(args[i],"&")==0)
		{
			continue;
		}
		else if (strcmp(args[i],">")==0)
		{
			i++;
			continue;
		}
		else if (strcmp(args[i],"<")==0)
		{
			i++;
			continue;
		}
		else if(strcmp(args[i],"|")==0)
		{
			argsN[j++]=0;
			continue;
		}
		else
		{
			argsN[j]= (char*) malloc(sizeof(args[i])); //memory allocation
			strcpy(argsN[j++],args[i]);
		}
	}
	for (int i=0;i<argsNo;i++) //to free the allocated memory
	{
		if(args[i])
			free(args[i]);
		args[i]=NULL;
	}
	for(int i=0;i<j;i++) //to set the values of the argument array to be equal to the new created array
	{
		args[i]=(char*) malloc(sizeof(argsN[i]));
		strcpy(args[i],argsN[i]);
	}
}

int checkForFlags(char** arguments,int a, int* conF,int* inpF, int* outpF, int* pipeF) //to set the values of the flags
{
	for (int i=0;i<a;i++)
	{	
		if(strcmp(arguments[i],"&")==0)
		{
			*conF=i; //the concurrency flag
		}
		else if (strcmp(arguments[i],">")==0)
		{
			*outpF=i; //to output redirection operator
		}
		else if (strcmp(arguments[i],"<")==0)
		{
			*inpF=i; //the input redirection operator
		}
		else if(strcmp(arguments[i],"|")==0)
		{
			*pipeF=i; //the pipe communication operator
		}
	}
	return 0;
}


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

/* spshell - minimalistic Linux shell written in C */
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <limits.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>


#define MAXCOMMANDS 100
#define MAX_PROMPT_LENGTH PATH_MAX+HOST_NAME_MAX+LOGIN_NAME_MAX+5

#define clear() printf("\033[H")

/* greeting screen */
void init_shell();
/*  function to get line input and display prompt */
int get_input(char *str);
/* function to break the input into tokens */
int tokenize(char *line, char** target);
/*   function to execute the commands  */
int run(char **cmds);
/* cd built-in command function */
int cd(char* path);
/* function to evaluate file paths such as ~/Downloads etc. */
int evaluate_file_path(char* path);


int main(void)
{
        char input[MAX_INPUT];
	char *commands[MAXCOMMANDS];
	bool receivedEOF = false;
	init_shell();
	while (!receivedEOF)
	{
        	if(get_input(input) == -2)
		{
			receivedEOF = true;
			continue;
		}
		if(tokenize(input, commands)) continue;
		if(!strcmp(commands[0],"cd"))
		{
			if(cd(commands[1]))
				fprintf(stderr, "cd: \'%s\' directory doesn't exist\n",commands[1]);
		}
		else
			run(commands);
	}
        
        return EXIT_SUCCESS;
}

// Greeting screen during startup
void init_shell()
{
        puts("******************************************************");
        puts("          SPSHELL - MINIMALISTIC LINUX SHELL          ");
        puts("             MEANT TO INTERACTIVE USE ONLY            ");
        puts("                 USE AT YOUR OWN RISK                 ");
        puts("******************************************************");
        printf("THE USER IS: %s\n",getenv("USER"));
	putchar('\n');
}
// Function to get input from the user
int get_input(char *str)
{
        char *buf;
        char prompt[MAX_PROMPT_LENGTH] = "[ ";
        char *username = getenv("USER");
	char hostname[HOST_NAME_MAX] = "";
	gethostname(hostname,HOST_NAME_MAX);
        char dir[PATH_MAX];
        getcwd(dir,sizeof(dir));
	strcat(prompt,username);
        strcat(prompt,"@");
        strcat(prompt,hostname);
	strcat(prompt," ");
        strcat(prompt,dir);
	if (strcmp(username,"root")) 
        	strcat(prompt," ] $ ");
	else strcat(prompt, " ] # ");
        buf = readline(prompt);
	if (buf == NULL || !strcmp(buf,"exit")) {
		puts("Goodbye!");
		free(buf);
		return -2;
	}
        if(strlen(buf) != 0) 
        {
                add_history(buf);
                strcpy(str,buf);
		free(buf);
                return 0;
        }
        return -1;
}

int tokenize(char *line, char** target)
{
	int i;
	// char *temp = line;
	for(i = 0; i < MAXCOMMANDS; i++)
	{
		if(line && (*line == '\"' ||  *line == '\''))
		{
			char *end = strchr(line+1,'\"');
			*line = '\0';
			if(end)
			{
				*end  = '\0';
				target[i] = line+1;
			}
			else 
			{
				fprintf(stderr,"spsh: Missing closing \'\"\' character\n");
				return -1;
			}
		}
		else target[i] = strsep(&line," ");
		if(target[i] == NULL) break;
	}
	return 0;
}
int run(char **cmds)
{
	pid_t pid;
	pid = fork();
	if (pid == 0)
	{
		if(execvp(cmds[0],cmds) == -1)
		{
			fprintf(stderr, "spsh: Failed to execute command \'%s\'\n",cmds[0]);
		}
	}
	else if(pid == -1)
	{
		fprintf(stderr, "spsh: Failed to start new process\n");
		return -1;
	}
	else
	{
		wait(NULL);
	}
	return 0;
}
int cd(char* path)
{
	if(path == NULL || !strcmp(path,""))
		return chdir(getenv("HOME"));
	evaluate_file_path(path);
	return chdir(path);
}

int evaluate_file_path(char* path)
{
	if(path[0] == '~')
	{
		char new_path[PATH_MAX];
		strcpy(new_path,getenv("HOME"));
		strcat(new_path,path+1);
		strcpy(path,new_path);
		
	}

	return 0;
}


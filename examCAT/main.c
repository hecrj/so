#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

int pid = -1;
char buff[80];

/**
 * Waits for the child with pid set in the global variable and
 * outputs an info message when DEBUG constant is defined.
 */
void wait_child(void)
{
	int status;
	waitpid(pid, &status, 0);

	#ifdef DEBUG
	sprintf(buff, "Terminated process with PID %d and status %d\n", pid, status);
	write(1, buff, strlen(buff));
	#endif

	pid = -1;
}

/**
 * Kills the main process and its child, if it's not dead already.
 * @param s Signal identifier
 */
void kill_all(int s)
{
	sprintf(buff, "Time is over\n");
	write(1, buff, strlen(buff));

	if(pid != -1)
	{
		kill(pid, SIGKILL);
		wait_child();
	}

	exit(0);
}

/**
 * Prints contents of the given file using cat command.
 * @param file File to print
 */
void print_content(char *file)
{
	pid = fork();

	if(pid == 0)
	{
		execlp("cat", "cat", file, (char*)NULL);
		perror("Error when executing cat command.");
		exit(1);
	}
	else if(pid < 0)
	{
		perror("Error when forking the process.");
		exit(1);
	}
	else
	{
		#ifdef DEBUG
		sprintf(buff, "New process with PID: %d\n", pid);
		write(1, buff, strlen(buff));
		#endif

		getchar();
		wait_child();
	}
}

/**
 * Prints instructions about how to use the tool.
 */
void Usage(void)
{
	sprintf(buff, "Usage: examCAT file1, file2, ...\n\n");
	write(1, buff, strlen(buff));

	sprintf(buff, "Description:\nPrints the content of every given file as a parameter.\n");
	write(1, buff, strlen(buff));

	exit(0);
}

/**
 * Prints the content of every given file as a parameter.
 * @param argc Number of arguments recieved
 * @param argv Array of arguments recieved
 */
void main(int argc, char *argv[])
{
	if(argc < 2)
		Usage();

	signal(SIGALRM, kill_all);
	alarm(10);

	int i;
	for(i = 1; i < argc; ++i)
		print_content(argv[i]);
}

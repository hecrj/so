#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

#define COUNTER_MAX 5
#define PS_USER "hector"

int alarm_recieved;
int counter;
int pid;
char buff[80];

/**
 * Prints the number of times that ps has been executed.
 * @param s Signal identifier
 */
void print_counter(int s)
{
	sprintf(buff, "The command ps has been executed: %d time", counter);
	write(1, buff, strlen(buff));

	if(counter != 1)
		write(1, "s", 1);

	write(1, "\n", 1);
}

/**
 * Prints the main process pid.
 * @param s Signal identifier
 */
void print_pid(int s)
{
	sprintf(buff, "The main process PID is: %d\n", pid);
	write(1, buff, strlen(buff));
}

/**
 * Executes the ps command for the user PS_USER with format
 * pid, parent pid and executable name.
 * @param s Signal identifier
 */
void exec_ps()
{
	int cpid = fork();

	if(cpid == 0)
	{
		execlp("ps", "ps", "-u", PS_USER, "-o", "pid,ppid,comm", (char*)NULL);
		perror("Error when executing ps command.");
		exit(1);
	}
	else if(cpid < 0)
	{
		perror("Error when forking the process.");
		exit(1);
	}
	else
	{
		int status;
		waitpid(cpid, &status, 0);
	}
}

void set_alarm(int s)
{
	alarm_recieved = 1;
}

/**
 * Prints info about the usage of the tool.
 */
void Usage(void)
{
	sprintf(buff, "Usage: examPS seconds\n\n");
	write(1, buff, strlen(buff));

	sprintf(buff, "Description:\nExecutes ps every 'seconds' periodically, 5 times.\n");
	write(1, buff, strlen(buff));

	exit(0);
}

/**
 * Executes the ps command periodically depending from the argument
 * given to the app, and a maximum number of COUNTER_MAX times.
 * @param argc Number of arguments recieved
 * @param argv Array of arguments recieved
 */
void main(int argc, char *argv[])
{
	if(argc != 2)
		Usage();

	signal(SIGALRM, set_alarm);
	signal(SIGUSR1, print_pid);
	signal(SIGUSR2, print_counter);

	pid = getpid();

	int interval = atoi(argv[1]);

	for(counter = 0; counter < COUNTER_MAX; ++counter)
	{
		alarm_recieved = 0;
		alarm(interval);

		while(alarm_recieved == 0)
			pause();

		exec_ps();
	}
}

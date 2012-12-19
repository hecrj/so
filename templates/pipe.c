#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Minimum number of arguments.
 */
#define MIN_ARGS 1

/**
 * Writes string s to fd stream.
 * @param fd A stream identifier
 * @param s  The string to write
 */
void put(int fd, char *s)
{
	write(fd, s, strlen(s));
}

/**
 * Throws an error and finishes the execution.
 * @param error_msg Error message to throw
 */
void runtime_error(char *error_msg)
{
	perror(error_msg);
	exit(1);
}

/**
 * Prints the usage and description of the program.
 */
void Usage()
{
	put(1, "Usage: name [params]\n\n");

	put(1, "Description:\n");
	put(1, "[Program description]");

	exit(1);
}

/**
 * [Parent function]
 * @param pipefd A pipe file descriptors.
 */
void parent(int pipefd[2])
{
	close(pipefd[0]);

	// Do something serious
	// For example:

	put(pipefd[1], "Chunky\n");
	close(pipefd[1]);

	int exit_status;
	waitpid(-1, &exit_status, 0);

	put(1, "Bacon\n");
}

/**
 * [Child function]
 * @param pipefd A pipe file descriptors.
 */
void child(int pipefd[2])
{
	close(pipefd[1]);

	dup2(pipefd[0], 0);
	close(pipefd[0]);

	execlp("cat", "cat", (char*)NULL);

	// Process arrives here -> Error when muting!
	runtime_error("Error on execlp.");
}

/**
 * [Main function]
 * @param argc Number of arguments received
 * @param argv Array of arguments
 */
void main(int argc, char *argv[])
{
	if(argc < MIN_ARGS)
		Usage();

	int pipefd[2];

	if(pipe(pipefd) == -1)
		runtime_error("Error creating a pipe.");

	int pid = fork();

	if(pid == -1)
		runtime_error("Error on fork.");

	else if(pid == 0)
		child(pipefd);

	else
		parent(pipefd);
}

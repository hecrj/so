#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	int fd[2];

	if(pipe(fd) < 0)
		perror("Error creating pipe.");


	int pid = fork();

	if(pid < 0)
		perror("Error when forking the process.");

	else if(pid == 0)
	{
		// Close pipe read stream
		close(fd[0]);

		// Close default write stream and assign pipe write stream
		dup2(fd[1], 1);

		// Close pipe write stream
		close(fd[1]);

		// CAT! CAT! CAT! Miau
		execlp("cat", "cat", (char *) 0);
	}

	else
	{
		// Close pipe write stream
		close(fd[1]);

		int num = 0;
		char c;

		// While we can read the pipe
		while(read(fd[0], &c, sizeof(c)) > 0)
			num++;

		// Output message
		char buff[80];
		sprintf(buff, "%d characters received.\n", num);
		write(1, buff, strlen(buff));
		waitpid(-1, NULL, 0);
	}
}

#include"kernel/types.h"
#include"user/user.h"

int main()
{
	int parent_fd[2], child_fd[2];
	char buf[14];
	int pid;
	//int n;

	pipe(parent_fd);
	pipe(child_fd);
	pid = fork();

	if(pid == 0)
	{
		read(parent_fd[0], buf, sizeof(buf));		
		printf("%d: %s", getpid(), buf);
		//write(1, buf, n);
		write(child_fd[1], "received pong\n", 14);
	}

	else
	{
		write(parent_fd[1], "received ping\n", 14);
		read(child_fd[0], buf, sizeof(buf));
		printf("%d: %s", getpid(), buf);
		//write(1, buf, n);
	}

exit();

}

#include"kernel/types.h"
#include"user/user.h"

void source(int pd[2]);
void cull(int pd[2], int n, int o);
void sink(int pd[2]);

int main()
{
	int pid, pd[2];
	pipe(pd);
	pid =fork();

	if(pid<0)
	{
		printf("fork failed\n");
		exit();
	}

	if(pipe(pd)<0)
	{
		printf("pipe failed\n");
	}

	if(pid == 0)
	{
		source(pd);
	}
	else
	{
		sink(pd);
	}

exit();
}

void source(int pd[2])//close output of pipe and feeds 2 to 35
{
	close(pd[0]);
	for(int i=2; i<36; i++)
	{
		write(pd[1], &i, sizeof(i));
	}
	close(pd[1]);
	return;

}

void cull(int pd[2], int n, int o)
{
	int a;
	close(pd[0]);
	for(;;)
	{
		if(read(o, &a, sizeof(a))==0)
		{
			close(pd[1]);
			return;
		}
		if((a%n==0))
		{
			write(pd[1], &a, sizeof(a));
		}
}


void sink(int pd[2])//read from pipe and 
{
	close(pd[1]);
	int n, pid, newpd[2];
	
	n = read(pd[0], &n, sizeof(n));
	if(n==0)
	{
		printf("pipe empty\n");
	}
	else
	{
		printf("prime: %d", n);
		
		if(pipe(pd)<0)
		{
			printf("sink pipe failed\n");
			exit();
		}
		else
		{
			pid=fork();
			if(pid==0)
			{
				cull(n, pd[0], newpd);
			}
			else
			{
				sink(newpd);
			}
		}

	
}

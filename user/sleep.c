#include"kernel/types.h"
#include"user/user.h"

int main(int argc, char*argv[])
{
	int i;

	if(argc<2 || argc>2)
	{
	printf("enter 2 arguments\n");
	}

	i = atoi(argv[1]);
	sleep (i);
	exit();
}

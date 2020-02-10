#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void ls(char *path,char *file)
{
	char buf[512],*p;
	int fd;
	struct dirent de;
	struct stat st;
	struct stat st1;

	if(path==file)
	{
		printf("%s",path);
		return;
	}

	if((fd=open(path,0))<0)
	{
		fprintf(2,"ls: cannot open %s\n",path);
		return;
	}

	if(fstat(fd,&st)<0)
	{
		fprintf(2,"ls: cannot stat %s\n",path);
		close(fd);
		return;
	}

	switch(st.type)
	{
		case T_FILE:
			return;

		case T_DIR:
			strcpy(buf,path);
			p=buf+strlen(buf);
			*p++='/';

			while(read(fd, &de, sizeof(de)) == sizeof(de)){
      			if(de.inum == 0||strcmp(de.name,".")==0||strcmp(de.name,"..")==0)
        		{	
        			continue;
        		}
				memmove(p, de.name, DIRSIZ);
				p[DIRSIZ]=0;
				if(stat(buf,&st1)<0)
				{
					printf("ls: cannot stat %s\n",buf);
					continue;
				}

				if(st1.type==T_DIR)
				{
					if (strcmp(de.name,file)==0)
					{
						printf("%s\n",buf);

					}
					else{
						ls(buf,file);
					}
					
				}
				else if(st1.type==T_FILE && strcmp(de.name,file)==0)
				{
					printf("%s\n",buf);
					
				}
			}

	}


}

void main(int argc, char *argv[])
{
	if(argc != 3)
	{
		printf("argument missing in the command\n");
		exit();
	}
	else
	{
		ls(argv[1],argv[2]);
		exit();
	}
}

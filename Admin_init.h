#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "structures.h"

long int findSize()
{
    struct stat st; 
    
    char *file_name = "Admin.txt"; 
     
    if(stat(file_name,&st)==0)
        return (st.st_size);
    else
        return -1;
}

int addAdmin(struct customer cust) 
{
	int temp = 1000;
	int fd=open("Admin.txt", O_CREAT | O_RDWR | O_APPEND, 0766);
	write(fd, &cust, sizeof(cust));
	write(fd,&temp,sizeof(temp)); //initial id for customers
	close(fd);
}

int lastid(int type)	//0-> fetch !=0-> store
{	
	int temp_buf;
	if(type==0)
	{
		int fd = open("Admin.txt", O_RDONLY, 0766);
		lseek(fd,2012,SEEK_SET);
		read(fd, &temp_buf, sizeof(int));
		return temp_buf;
	}
	else
	{
		int fd = open("Admin.txt", O_WRONLY, 0766);
		lseek(fd,2012,SEEK_SET);
		write(fd, &type, sizeof(int));
		return 1;
	}
}

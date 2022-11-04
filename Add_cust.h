#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "Admin_init.h"


int addUser(struct customer cust) 
{	
	int fd=open("cust.txt", O_CREAT | O_RDWR | O_APPEND, 0766);
	struct customer c;

	int flag1=0;
	while(read(fd, &c, sizeof(c))>0)
	{
		if( strcmp(c.username, cust.username)==0)
		{
			flag1=1;
		}
	}
	if(flag1!=1)		// username doesnt exist
	{
		struct flock lock;
		lock.l_type = F_WRLCK;	
		lock.l_whence = SEEK_CUR;
		lock.l_start = 0;
		lock.l_len = 0;
		lock.l_pid = getpid();
		fcntl(fd, F_SETLKW, &lock);		// locking.  -> here on critical section.
		write(fd, &cust, sizeof(cust));
		lock.l_type = F_UNLCK;			// unlocking.
		fcntl(fd, F_SETLK, &lock);		// unlocking.
		close(fd);
		
		// -----------locking and writing into customer file-------------------
		
		int fd1=open("Account.txt", O_CREAT | O_RDWR | O_APPEND, 0766);
		struct account a,a1;
		int flag1=0;
		
		while(read(fd1, &a, sizeof(a)) > 0)
		{
			if( (cust.accno-a.accno) == 0)
				flag1=1;
				
		}
		
		if(flag1!=1)
		{
			a1.accno=cust.accno;
			a1.balance = 0;
			a1.acc_status=1;
			write(fd1, &a1, sizeof(a1));	
		}
		
		close(fd1);
		
		return 1;
	}
	else		// username exists.
		return 0;
	close(fd);
}

int addJointUser(struct jointcustomer cust) 
{
	
	int fd=open("Joint.txt", O_CREAT | O_RDWR | O_APPEND, 0766);
	struct jointcustomer c;
	
	int flag1=0;
	while(read(fd, &c, sizeof(c))>0)
	{
		if( ((strcmp(c.username1, cust.username1)==0) && (strcmp(c.username2, cust.username2)==0)) || ((strcmp(c.username1, cust.username2)==0) && (strcmp(c.username2, cust.username1)==0)))
		{
			flag1=1;
		}
	}
	if(flag1!=1)		// username doesnt exist
	{
		struct flock lock;
		lock.l_type = F_WRLCK;	
		lock.l_whence = SEEK_CUR;
		lock.l_start = 0;
		lock.l_len = 0;
		lock.l_pid = getpid();
		fcntl(fd, F_SETLKW, &lock);		// locking.  -> here on critical section.
		write(fd, &cust, sizeof(cust));
		lock.l_type = F_UNLCK;			// unlocking.
		fcntl(fd, F_SETLK, &lock);		// unlocking.
		
		close(fd);
		
		// -----------locking and writing into customer file-------------------
		
		int fd1=open("Account.txt", O_CREAT | O_RDWR | O_APPEND, 0766);
		struct account a,a1;
		int flag1=0;
		
		while(read(fd1, &a, sizeof(a)) > 0)
		{
			if( (cust.accno-a.accno) == 0)
				flag1=1;
		}
		if(flag1!=1)
		{
			a1.accno=cust.accno;
			a1.balance = 0;
			a1.acc_status=1;
			write(fd1, &a1, sizeof(a1));	
		}
		close(fd1);
		
		return 1;
	}
	else
		return 0;
	close(fd);
}

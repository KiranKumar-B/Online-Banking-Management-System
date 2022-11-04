#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#define size 1000
#include "Add_cust.h"
int withdraw(int accno, int amount);
void updateTransactionFile(int accno, int amount, int total_bal);

int isUserValid(char *name, int ch)		// choice - 0 -> user - 2 -> admin.
{
	int fd;
	if(ch==1)
		fd=open("cust.txt", O_RDONLY, 0766);
	else if(ch==3)
		fd=open("Admin.txt", O_RDONLY, 0766);	
	struct customer c1;
		
	while(read(fd, &c1, sizeof(c1))>0)
	{		
		if(strcmp(c1.username, name) == 0 && c1.user_status!=0)
			return 1;
	}
	close(fd);
	return (-1);
}

int isJointUserValid(char *name)		// choice - 0 -> user - 2 -> admin.
{
	int fd=open("Joint.txt", O_RDONLY, 0766);
		
	struct jointcustomer c1;
		
	while(read(fd, &c1, sizeof(c1))>0)
	{		
		if(((strcmp(c1.username1, name) == 0) || (strcmp(c1.username2, name) == 0)) && (c1.user_status!=0))
			return 1;
	}
	close(fd);
	return (-1);
}


int isPasswordValid(char *name, char *pass, int ch)
{
	int fd;
	if(ch==1)
		fd=open("cust.txt", O_RDONLY, 0766);
	else if(ch==3)
		fd=open("Admin.txt", O_RDONLY, 0766);
	int value;
	
	struct customer c1;
	
	while(read(fd, &c1, sizeof(c1))>0)
	{		
		if( strcmp(c1.username,name)==0 && strcmp(c1.password, pass)==0)
		{
			value = c1.type;
			
			return value;
		}
	}
	close(fd);
	return (-1);
}

int isJointPasswordValid(char *name, char *pass)
{
	int fd=open("Joint.txt", O_RDONLY, 0766);
	int value;
	
	struct jointcustomer c1;
	
	while(read(fd, &c1, sizeof(c1))>0)
	{		
		if( ((strcmp(c1.username1,name)==0) && (strcmp(c1.password1, pass)==0)) || ((strcmp(c1.username2,name) == 0) && (strcmp(c1.password2, pass)==0)))
		{
			value = c1.type;
			return value;
		}
	}
	close(fd);
	return (-1);
}

int isjointaccvalid(int accno, char* uname)
{
	int fd=open("Joint.txt", O_RDONLY, 0766);
	int retval=1;		//not found the account
	
	struct jointcustomer c1;
	
	while(read(fd,&c1,sizeof(c1))>0)
	{
		if((c1.accno-accno)==0 && (strcmp(c1.username1,uname)==0 || strcmp(c1.username2,uname)==0))
		{
			retval=0;	//found the account
			return retval;
		}
	}
	return retval;
}		

struct account searchDetails(int acc_no)
{
	int fd=open("Account.txt", O_RDONLY, 0766);
	
	struct flock lock;
	lock.l_type = F_RDLCK;	
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;
	lock.l_pid = getpid();
	fcntl(fd, F_SETLKW, &lock);		// locking on critical section.
	
	
	struct account a1, a2;
	int flag=0;
	while(read(fd, &a1, sizeof(a1))>0)
	{		
		if(a1.accno == acc_no)
		{
			flag=1;
			break;
		}
	}
	lock.l_type = F_UNLCK;			// unlocking
	fcntl(fd, F_SETLK, &lock);		// unlocking
	
	if(flag==1)
		return a1;
	else
	{
		a1.acc_status = -10;
		return a1;			
	}

	close(fd);
}

struct account searchDetailsByName(char *name)
{
	int fd=open("cust.txt", O_RDONLY, 0766);
	
	struct flock lock;
	lock.l_type = F_RDLCK;	
	lock.l_whence = SEEK_CUR;
	lock.l_start = 0;
	lock.l_len = 0;
	lock.l_pid = getpid();
	fcntl(fd, F_SETLKW, &lock);		// locking.  -> here on critical section.
	
	int accountno=0;
	struct jointcustomer c1;
	
	while(read(fd, &c1, sizeof(c1))>0)
	{		
		if(strcmp(c1.username1, name) == 0)
		{
			accountno=c1.accno;
			break;
		}
	}
	
	lock.l_type = F_UNLCK;			// unlocking.
	fcntl(fd, F_SETLK, &lock);		// unlocking.
	
	close(fd);

	int fd1=open("Account.txt", O_RDONLY, 0766);
	
	struct account a1;
	
	while(read(fd1, &a1, sizeof(a1))>0)
	{		
		if(a1.accno == accountno)
		{
			return a1;
		}
	}
	
	return a1;
	close(fd1);
}

struct customer viewDetailsByName(char *name)
{
	int fd=open("cust.txt", O_RDONLY, 0766);
	
	struct flock lock;
	lock.l_type = F_RDLCK;	
	lock.l_whence = SEEK_CUR;
	lock.l_start = 0;
	lock.l_len = 0;
	lock.l_pid = getpid();
	fcntl(fd, F_SETLKW, &lock);		// locking.  -> here on critical section.
	
	
	struct customer c1;
	while(read(fd, &c1, sizeof(c1))>0)
	{		
		if(strcmp(c1.username, name) == 0)
		{
			
			lock.l_type = F_UNLCK;			// unlocking.
			fcntl(fd, F_SETLK, &lock);		// unlocking.
			return c1;
		}
	}
	lock.l_type = F_UNLCK;			// unlocking.
	fcntl(fd, F_SETLK, &lock);		// unlocking.
	
	close(fd);
}

struct jointcustomer viewJointDetailsByName(char *name)
{
	int fd=open("Joint.txt", O_RDONLY, 0766);
	
	struct flock lock;
	lock.l_type = F_RDLCK;	
	lock.l_whence = SEEK_CUR;
	lock.l_start = 0;
	lock.l_len = 0;
	lock.l_pid = getpid();
	fcntl(fd, F_SETLKW, &lock);		// locking.  -> here on critical section.
	
	
	struct jointcustomer c1;
	while(read(fd, &c1, sizeof(c1))>0)
	{		
		if(strcmp(c1.username1, name) == 0 || strcmp(c1.username2,name) == 0)
		{
			
			lock.l_type = F_UNLCK;			// unlocking.
			fcntl(fd, F_SETLK, &lock);		// unlocking.
			return c1;
		}
	}
	lock.l_type = F_UNLCK;			// unlocking.
	fcntl(fd, F_SETLK, &lock);		// unlocking.
	
	close(fd);
}

//--------------------------------------------Delete functionality in Admin -----------------------------------------------------

int deleteAccount(int acc_num)
{
	int fd=open("Account.txt", O_RDWR, 0766);
	
	struct account a1;
	int ch=1, retval=-1;
	
	while(read(fd, &a1, sizeof(a1))>0)
	{		
		if( (a1.accno-acc_num) == 0 )
		{	
			break;
		}
		ch++;
	}
	if(a1.balance>0)
	{	
		int ch=1, total_bal, amount=a1.balance;
		struct flock lock;
		lock.l_type = F_WRLCK;
		lock.l_whence = SEEK_SET;
		lock.l_start = ((ch-1)*sizeof(a1))+1;
		lock.l_len = sizeof(a1);
		lock.l_pid = getpid();
		fcntl(fd, F_SETLKW, &lock);	//locking	
		retval = a1.balance;
		struct account a2;
		a2.accno = acc_num;
		a2.balance = 0;
		a2.acc_status = a1.acc_status;
		total_bal = a2.balance;
		lseek(fd, ((ch-1) * 12), SEEK_SET);		// critical section...
		write(fd, &a2, 12);
		lock.l_type = F_UNLCK;
		fcntl(fd, F_SETLK, &lock);			// unlocking
		close(fd);
		amount=-amount;			//-ve(amount);
		updateTransactionFile(acc_num, amount, total_bal);
	}
	struct account a2;
	a2.accno = a1.accno;
	a2.balance = a1.balance;
	a2.acc_status = 0;
	
	struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = ((ch-1)*12)+1;
	lock.l_len = 12;
	lock.l_pid = getpid();
	fcntl(fd, F_SETLKW, &lock);		// locking.......
	lseek(fd, ((ch-1) * 12), SEEK_SET);		// critical section...
	write(fd, &a2, 12);
	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &lock);		// unlocked.
	close(fd);
	return retval;
}

void deleteCustomer(int accountnumber)
{
	int fd=open("cust.txt", O_RDWR, 0766);	
	int i=1;
	
	struct customer c1, c;
	while(read(fd, &c1, sizeof(c1))>0)
	{		
		if( (c1.accno - accountnumber) == 0)
		{
			strcpy(c.username, c1.username);		// copying from c1.username to c.username
			strcpy(c.password, c1.password);
			c.accno = c1.accno;
			c.type = c1.type;
			c.user_status=0;
				
			struct flock lock;
			lock.l_type = F_WRLCK;
			lock.l_whence = SEEK_SET;
			lock.l_start = ((i-1)*2012)+1;
			lock.l_len = 2012;
			lock.l_pid = getpid();
			fcntl(fd, F_SETLKW, &lock);	
			lseek(fd, ((i-1) * 2012), SEEK_SET);
			write(fd, &c, 2012);
			lock.l_type = F_UNLCK;
			fcntl(fd, F_SETLK, &lock);
		}
		i++;
	}
	close(fd);
}

void deleteJointCustomer(int accountnumber)
{
	int fd=open("Joint.txt", O_RDWR, 0766);	
	int i=1;
	
	struct jointcustomer c1, c;
	
	while(read(fd, &c1, sizeof(c1))>0)
	{		
		if( (c1.accno - accountnumber) == 0)
		{
			strcpy(c.username1, c1.username1);		// copying from c1.username to c.username
			strcpy(c.username2, c1.username2);
			strcpy(c.password1, c1.password1);
			strcpy(c.password2, c1.password2);
			c.accno = c1.accno;
			c.type = c1.type;
			c.user_status=0;
				
			struct flock lock;
			lock.l_type = F_WRLCK;
			lock.l_whence = SEEK_SET;
			lock.l_start = ((i-1)*4012)+1;
			lock.l_len = 4012;
			lock.l_pid = getpid();
			fcntl(fd, F_SETLKW, &lock);
			lseek(fd, ((i-1) * 4012), SEEK_SET);
			write(fd, &c, 4012);			
			lock.l_type = F_UNLCK;
			fcntl(fd, F_SETLK, &lock);
		}
		i++;
	}
	close(fd);
}
int delete_Account(int accountnumber, int user_type)
{
	
	int total = deleteAccount(accountnumber);
	
	if(user_type==1)		// user_type = 1 : single user  user_type = 2 : joint user
		deleteCustomer(accountnumber);
		
	else if(user_type==2)
		deleteJointCustomer(accountnumber);
	return total;
}

//--------------------------------------------MOdify functionality in Admin -----------------------------------------------------

int modifyAccount(int acc_num)
{
	int fd=open("Account.txt", O_RDWR, 0766);
	
	struct account a1;
	int ch=1, retval=-1;
	
	while(read(fd, &a1, sizeof(a1))>0)
	{		
		if( (a1.accno-acc_num) == 0 )
		{	
			retval=1;
			break;
		}
		ch++;
	}
	struct account a2;
	a2.accno = a1.accno;
	a2.balance = a1.balance;
	a2.acc_status = 1;
	
	struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = ((ch-1)*12)+1;
	lock.l_len = 12;
	lock.l_pid = getpid();
	fcntl(fd, F_SETLKW, &lock);		// locking.......
	lseek(fd, ((ch-1) * 12), SEEK_SET);		// critical section...
	write(fd, &a2, 12);
	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &lock);		// unlocked.
	close(fd);
	return retval;
}


void modifyCustomer(int accountnumber)
{
	int fd=open("cust.txt", O_RDWR, 0766);	
	int i=1;
	
	struct customer c1, c;
	while(read(fd, &c1, sizeof(c1))>0)
	{		
		if( (c1.accno - accountnumber) == 0)
		{
			strcpy(c.username, c1.username);		// copying from c1.username to c.username
			strcpy(c.password, c1.password);
			c.accno = c1.accno;
			c.type = c1.type;
			c.user_status=1;
				
			struct flock lock;
			lock.l_type = F_WRLCK;
			lock.l_whence = SEEK_SET;
			lock.l_start = ((i-1)*2012)+1;
			lock.l_len = 2012;
			lock.l_pid = getpid();
			fcntl(fd, F_SETLKW, &lock);	
			lseek(fd, ((i-1) * 2012), SEEK_SET);
			write(fd, &c, 2012);
			lock.l_type = F_UNLCK;
			fcntl(fd, F_SETLK, &lock);
		}
		i++;
	}
	close(fd);
}

void modifyJointCustomer(int accountnumber)
{
	int fd=open("Joint.txt", O_RDWR, 0766);	
	int i=1;
	
	struct jointcustomer c1, c;
	
	while(read(fd, &c1, sizeof(c1))>0)
	{		
		if( (c1.accno - accountnumber) == 0)
		{
			strcpy(c.username1, c1.username1);		// copying from c1.username to c.username
			strcpy(c.username2, c1.username2);
			strcpy(c.password1, c1.password1);
			strcpy(c.password2, c1.password2);
			c.accno = c1.accno;
			c.type = c1.type;
			c.user_status=1;
				
			struct flock lock;
			lock.l_type = F_WRLCK;
			lock.l_whence = SEEK_SET;
			lock.l_start = ((i-1)*4012)+1;
			lock.l_len = 4012;
			lock.l_pid = getpid();
			fcntl(fd, F_SETLKW, &lock);
			lseek(fd, ((i-1) * 4012), SEEK_SET);
			write(fd, &c, 4012);			
			lock.l_type = F_UNLCK;
			fcntl(fd, F_SETLK, &lock);
		}
		i++;
	}
	close(fd);
}

int modify_Account(int accountnumber, int user_type)
{
	int total = modifyAccount(accountnumber);
	
	if(user_type==1)		// user_type = 1 : single user    user_type = 2 : joint user
		modifyCustomer(accountnumber);
		
	else if(user_type==2)
		modifyJointCustomer(accountnumber);
	return total;
}

//----------------------------------------------------- PASSWORD CHANGE ---------------------------------------------------

void passwordChange(char *username, char *newpass)
{
	int fd=open("cust.txt", O_RDWR, 0766);	
	int i=1;
	
	struct customer c1, c;
	while(read(fd, &c1, sizeof(c1))>0)
	{		
		if( strcmp(c1.username, username)==0 )
		{
			strcpy(c.username, c1.username);		// copying from c1.username to c.username
			strcpy(c.password, newpass);
			c.accno = c1.accno;				// changing account number to new account number 
			c.type = c1.type;
			c.user_status=c1.user_status;
			struct flock lock;
			lock.l_type = F_WRLCK;
			lock.l_whence = SEEK_SET;
			lock.l_start = ((i-1)*2012)+1;
			lock.l_len = 2012;
			lock.l_pid = getpid();
			fcntl(fd, F_SETLKW, &lock);
			lseek(fd, ((i-1) * 2012), SEEK_SET);
			write(fd, &c, 2012);
			lock.l_type = F_UNLCK;
			fcntl(fd, F_SETLK, &lock);
		}
		i++;
	}	
	close(fd);	
}

void passwordJointChange(char *username, char *newpass, int accno)
{
	int fd=open("Joint.txt", O_RDWR, 0766);	
	int i=1;
	
	struct jointcustomer c1;
	char c[size];
	while(read(fd, &c1, sizeof(c1))>0)
	{		
		if(strcmp(c1.username1, username)==0 && c1.accno == accno)
		{
			strcpy(c, newpass);
			struct flock lock;
			lock.l_type = F_WRLCK;
			lock.l_whence = SEEK_SET;
			lock.l_start = ((i-1)*4012)+1000;
			lock.l_len = 1000;
			lock.l_pid = getpid();
			fcntl(fd, F_SETLKW, &lock);
			lseek(fd, (((i-1) * 4012)+1000), SEEK_SET);
			write(fd, &c, 1000);
			lock.l_type = F_UNLCK;
			fcntl(fd, F_SETLK, &lock);
		}
		else if(strcmp(c1.username2, username)==0 && c1.accno == accno)
		{
			strcpy(c, newpass);
			struct flock lock;
			lock.l_type = F_WRLCK;
			lock.l_whence = SEEK_SET;
			lock.l_start = ((i-1)*4012)+3000;
			lock.l_len = 4012;
			lock.l_pid = getpid();
			fcntl(fd, F_SETLKW, &lock);
			lseek(fd, (((i-1) * 4012)+3000), SEEK_SET);
			write(fd, &c, 1000);
			lock.l_type = F_UNLCK;
			fcntl(fd, F_SETLK, &lock);
		}
		i++;
	}	
	close(fd);	
}

//---------------------------------------------- DEPOSIT --------------------------------------------------------------------

void getTransactions()
{
	int fd=open("Transc.txt", O_RDONLY, 0766);
	struct transaction t1;
	while(read(fd, &t1, sizeof(t1))>0)
	{		
		printf("\n%s\t%d\t%d\t%d", t1.date, t1.accno, t1.amount, t1.balance);
	}
	close(fd);
}


void updateTransactionFile(int accno, int amount, int total_bal)
{
	/* date part */
	
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
  	int year = tm.tm_year + 1900;
  	int month = tm.tm_mon + 1;
  	int day = tm.tm_mday;
  	char y[10], m[10], d[10];
  	sprintf(y, "%d", year);
  	sprintf(m, "%d", month);
  	sprintf(d, "%d", day);
  
  	char date[size];
  	snprintf(date, sizeof(date), "%s/%s/%s", d, m, y);
	// date part end--------------------------------------
	
	int fd = open("Transc.txt", O_CREAT | O_RDWR | O_APPEND, 0766);

	struct transaction txn;
	strcpy(txn.date, date);
	txn.accno = accno;
	txn.amount = amount;
	txn.balance = total_bal;
	
	struct flock lock;
	lock.l_type = F_WRLCK;	
	lock.l_whence = SEEK_CUR;
	lock.l_start = 0;
	lock.l_len = 0;
	lock.l_pid = getpid();		
	fcntl(fd, F_SETLKW, &lock);		// locking.  -> here on critical section.
	write(fd, &txn, sizeof(txn));
	lock.l_type = F_UNLCK;			// unlocking.
	fcntl(fd, F_SETLK, &lock);		// unlocking.
	close(fd);
}

void deposit(int accno, int amount)
{
	int fd=open("Account.txt", O_RDWR, 0766);
	
	struct account a1;
	int ch=1, total_bal;
	while(read(fd, &a1, sizeof(a1))>0)
	{		
		if( (a1.accno-accno) == 0 )
		{	
			break;
		}
		ch++;
	}
	struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = ((ch-1)*sizeof(a1))+1;
	lock.l_len = sizeof(a1);
	lock.l_pid = getpid();
	fcntl(fd, F_SETLKW, &lock);	// locking.
	lseek(fd,(-1)*sizeof(a1),SEEK_CUR);
	read(fd,&a1,sizeof(a1));
	struct account a2;
	a2.accno = accno;
	a2.balance = a1.balance + amount;
	a2.acc_status = a1.acc_status;
	total_bal = a2.balance;
	lseek(fd, ((ch-1) * 12), SEEK_SET);		// critical section...
	write(fd, &a2, 12);
	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &lock);		// unlocking.
	close(fd);
	updateTransactionFile(accno, amount, total_bal);
}

//--------------------------------------WITHDRAW ------------------------------------------------------

int withdraw(int accno, int amount)
{
	int balance_old;
	int fd=open("Account.txt", O_RDWR, 0766);
	
	struct account a1;
	int ch=1, total_bal;
	while(read(fd, &a1, sizeof(a1))>0)
	{		
		if( (a1.accno-accno) == 0 )
		{	
			break;
		}
		ch++;
	}
	struct flock lock;
		lock.l_type = F_WRLCK;
		lock.l_whence = SEEK_SET;
		lock.l_start = ((ch-1)*sizeof(a1))+1;
		lock.l_len = sizeof(a1);
		lock.l_pid = getpid();
		fcntl(fd, F_SETLKW, &lock);	//locking	
	balance_old = a1.balance;
	if(balance_old>=amount)
	{
		struct account a2;
		a2.accno = accno;
		a2.balance = a1.balance - amount;
		a2.acc_status = a1.acc_status;
		total_bal = a2.balance;
		lseek(fd, ((ch-1) * 12), SEEK_SET);		// critical section...
		write(fd, &a2, 12);
		lock.l_type = F_UNLCK;
		fcntl(fd, F_SETLK, &lock);			// unlocking
		close(fd);
		amount=-amount;			//-ve(amount);
		updateTransactionFile(accno, amount, total_bal);
		return 1;
	}
	fcntl(fd, F_SETLK, &lock);			// unlocking
	close(fd);
	return -1;
}	

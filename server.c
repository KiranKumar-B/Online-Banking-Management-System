#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <string.h>
#include "comm_func.h"
#define PORT 8090
#define size 1000

int main()
{
	if(findSize()==-1)
	{
		struct customer c1;
		printf("Create Admin\n");
		printf("Enter Admin name:");
		scanf("%s", c1.username);
		printf("Enter Admin Password:");
		scanf("%s", c1.password);
		c1.accno = 0;
		c1.type = 3;
		c1.user_status = 1;
		addAdmin(c1);
		printf("Admin Created Successfully\n");
		exit(0);
	}

	struct customer c1;
	struct account a1;
	
	struct sockaddr_in server, client;    
	int socket_desc, size_client, client_desc,temp, acno;    
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);    
	server.sin_family = AF_INET;    
	server.sin_addr.s_addr = INADDR_ANY;    
	server.sin_port = htons(PORT);    
	
	char user[size], pass[size], sample[size];
	
	if(bind(socket_desc, (void *)(&server), sizeof(server)) < 0)    
	{        
		perror("Error on binding:");        
		exit(EXIT_FAILURE);    
	}    
	listen(socket_desc, 5);    
	for(;;)
	{        
		size_client = sizeof(client);        
		if((client_desc = accept(socket_desc, (struct sockaddr*)&client,&size_client)) < 0) 
		{            
			perror("Error on accept:");            
			exit(EXIT_FAILURE);        
		}        
		if(fork() == 0)
		{
			for(;;)
			{
				
				int choice, buff;
				read(client_desc, &choice, sizeof(choice));
				read(client_desc, user, sizeof(user));  // reading username
				if(choice==1 || choice==3)
					buff = isUserValid(user, choice);   		// username validation.
				else if(choice==2)
					buff = isJointUserValid(user);	// for joint user.			
				write(client_desc, &buff, sizeof(buff));		// sending client whether the user is valid or not.. valid -> 1 not valid -> 0
				read(client_desc, pass, sizeof(pass));	// reading password
				if(choice==1 || choice==3)
					buff = isPasswordValid(user,pass,choice);    // password validation and finding the type of user it is. 0 -> not valid, 10-> normal user, 11->joint user, 12->admin
				else if(choice==2)
					buff = isJointPasswordValid(user,pass);			
				write(client_desc, &buff, sizeof(int));	// sending validation information to client.
				printf("%d", buff);				// 3 -> admin...  1 -> single .. 2 -> joint.
				struct customer c3;
				struct jointcustomer c4;
				struct account a,x;
				if(buff==1)			// single user.
				{
					int flag=0;
					for(;;)
					{	
						read(client_desc, &buff, sizeof(buff));	// read choice inside admin
						int b, accno, amt, bala;
						switch(buff)
						{
						
							case 1:
								a=searchDetailsByName(user);			// search the file for the details.
								int stat = a.acc_status;
								write(client_desc, &stat, sizeof(stat));	// writing account status to the client.
								if(stat != 0)
								{
									int accntno=a.accno;
									write(client_desc, &accntno, sizeof(accntno));		// write accnt number to client.
									
									
									read(client_desc, &amt, sizeof(amt));			// read amt from client.
									deposit(a.accno, amt);
								}		
								break;
								
							case 2:
								read(client_desc, &b, sizeof(b));
								a=searchDetailsByName(user);
								bala=a.balance;
								write(client_desc, &bala, sizeof(bala));		// write balance to client.
								if(b<=bala)
								{
									accno = a.accno;
									withdraw(accno,b);				// b => amount to withdraw, bala => present balance in account.
								}
								break;
							
							case 3:
								a=searchDetailsByName(user);			// search the file for the details.
								int accntno=a.accno;
								write(client_desc, &accntno, sizeof(accntno));		// write accnt number to client.
								bala=a.balance;
								write(client_desc, &bala, sizeof(bala));		// write balance to client.
								break;
							
							case 4:
								read(client_desc, sample, sizeof(sample));		// read new password from the user.
								passwordChange(user, sample);
								break;
							
							case 5:
								c3=viewDetailsByName(user);			// search the file for the details.
								int acctno=c3.accno;
								write(client_desc, &acctno, sizeof(acctno));		// write acctno to client.
								int type=c3.type;
								write(client_desc, &type, sizeof(type));		// write type to client.
								int ustat=c3.user_status;
								write(client_desc, &ustat, sizeof(ustat));		// write type to client.								
								break;
								
							default:
								exit(0);
						}
					}
				}
				else if(buff == 2)
				{
					int accno;
					read(client_desc, &accno, sizeof(accno));
					for(;;)
					{
						read(client_desc, &buff, sizeof(buff));	// read choice inside admin
						int b, amt, op_suc, cur_bal;
						switch(buff)
						{
							case 1:
								read(client_desc, &amt, sizeof(amt));			// read amt from client.
								deposit(accno, amt);
								break;
								
							case 2:
								read(client_desc, &b, sizeof(b));	//amount to be withdrawn
								op_suc = withdraw(accno,b);				// b => amount to withdraw, bala => present balance in account.
								write(client_desc, &op_suc, sizeof(op_suc));		// write operation success status to client.
								a=searchDetails(accno);			// search the file for the details.
								cur_bal=a.balance;
								write(client_desc, &cur_bal, sizeof(cur_bal));						
								break;
							
							case 3:
								a=searchDetails(accno);			// search the file for the details.
								cur_bal=a.balance;
								write(client_desc, &cur_bal, sizeof(cur_bal));		// write balance to client.
								break;
							
							case 4:
								read(client_desc, sample, sizeof(sample));		// read new password from the user.
								passwordJointChange(user, sample, accno);
								break;
							
							case 5:
								c4=viewJointDetailsByName(user);			// search the file for the details.
								accno = c4.accno;
								write(client_desc, &accno, sizeof(accno));		// write acctno to client.
								int type=c4.type;
								write(client_desc, &type, sizeof(type));		// write type to client.
								int ustat=c4.user_status;
								write(client_desc, &ustat, sizeof(ustat));		// write type to client.
								break;
								
							default:
								exit(0);
						}
					}
					
				}
				else if(buff==3)								// admin.
				{
					for(;;)
					{
						read(client_desc, &buff, sizeof(buff));	// read choice inside admin
						struct customer c3;
						struct jointcustomer c4;
						int b, accno1, accno2, opn, user_type, retval, m_temp;
						switch(buff)
						{
							case 1: // ADD USER
								int utype;
								read(client_desc, &utype, sizeof(utype)); // type  1-> normal user   2->joint user 
								if(utype==1)
								{
									read(client_desc, c3.username, sizeof(c3.username)); // username
									read(client_desc, c3.password, sizeof(c3.password)); // password
									int account_number, user_status;
									read(client_desc, &account_number, sizeof(account_number)); // accno
									c3.accno=account_number;
									c3.type=utype; //type
									read(client_desc, &user_status, sizeof(user_status)); // user_status
									c3.user_status=user_status;
									b=addUser(c3);
								}
								else if(utype==2)
								{
									read(client_desc, c4.username1, sizeof(c4.username1)); // username1
									read(client_desc, c4.password1, sizeof(c4.password1)); // password1
									read(client_desc, c4.username2, sizeof(c4.username2)); // username2
									read(client_desc, c4.password2, sizeof(c4.password2)); // password2
									int account_number, user_status;
									read(client_desc, &account_number, sizeof(account_number)); // accno
									c4.accno=account_number;
									c4.type=utype; //type
									read(client_desc, &user_status, sizeof(user_status)); // user_status
									c4.user_status=user_status;
									b=addJointUser(c4);
								}
								write(client_desc, &b, sizeof(b));
								break;
								
							case 2:	// DELETE ACCOUNT DETAILS.
								read(client_desc, &user_type, sizeof(user_type));
								read(client_desc, &acno, sizeof(acno));
								retval = delete_Account(acno, user_type);				// user_type = 1 -> normal, 2 -> joint
								write(client_desc, &retval, sizeof(retval));
								break;
							
							case 3:
							
								read(client_desc, &user_type, sizeof(user_type));
								read(client_desc, &accno1, sizeof(accno1));		// read original accno from client.
								m_temp=modify_Account(accno1, user_type);
								write(client_desc, &retval, sizeof(retval));
								break;
								
							case 4:
								read(client_desc, &b, sizeof(b));  // receive accno from client.
								a=searchDetails(b); // search the file for the details.
								int bala=a.balance;
								write(client_desc, &bala, sizeof(bala)); // write balance to client.
								int acc_stat = a.acc_status;
								write(client_desc, &acc_stat, sizeof(acc_stat)); // write account status to client.
								break;
							case 5:
								break;
							default:
								exit(0);
						}
					}
				}
				else
					exit(0);	
			}
		}
	}
	close(socket_desc);
	return (0);
}

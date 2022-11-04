#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include "comm_func.h"
#define PORT 8090
#define size 1000

int last_accno = 0;
int main()
{
	char buff[size],username[size], password[size];
	char username1[size], username2[size], password1[size], password2[size];
	int i, q, p, ch, choice, socket_desc,sfd;
	last_accno=lastid(0);
	printf("%d",last_accno);
	
	struct sockaddr_in server;     
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);    
	server.sin_family = AF_INET;    
	server.sin_addr.s_addr = INADDR_ANY;    
	server.sin_port = htons(PORT);    
	sfd=connect(socket_desc, (void *)(&server), sizeof(server));  
	 if(sfd==-1)
	 {
	 perror("");
	 }
	else
	{
	printf("\n\t\tWelcome To Online Banking System\n\n");	
	printf("You want to login as\n1. user\n2. jointuser\n3. Admin:\n");
	scanf("%d", &choice);
	if(choice==1||choice==2||choice==3)
	{
	write(socket_desc, &choice, sizeof(choice));
	printf("Enter username: ");
	scanf("%s", username);
	write(socket_desc, username, strlen(username));
	read(socket_desc, &p, sizeof(p));
	if((p)!=-1)
	{
		printf("Enter password: ");
		scanf("%s", password);
	 	write(socket_desc, password, strlen(password));
	 	read(socket_desc, &p, sizeof(p));
	 	if((p)!=-1)
	 	{	
		 	if(p==1)	//Normal user
			{
				printf("\n=================== Welcome user: %s ===================\n", username);
				for(;;)
				{
					printf("\nChoose one option:\n1.Deposit\n2.Withdraw\n3.Balance Enquiry\n4.Password change\n5.View details\n6.Exit\n");
					scanf("%d", &ch);
					write(socket_desc, &ch, sizeof(ch));			// write choice to server.
					int accno, type, user_status, temp, amt;
					char amount[100];		
					switch(ch)
					{
						case 1:
							printf("\n-----------------DEPOSIT-----------------\n");
							int t;
							read(socket_desc, &t, sizeof(t));	// account status 
							if(t!=0)
							{
								read(socket_desc, &temp, sizeof(temp));	// account number
								while(1)
								{
									printf("\nEnter amount(>0) to deposit: ");
									scanf("%s",amount);
									amt=atoi(amount);
									if(amt>0)
									break;
								}							
								write(socket_desc, &amt, sizeof(amt));	// send amt to deposit to server.
							}
							else
								printf("Your account has been closed.\nYou cannot Deposit!\n");	
							break;
						
						case 2:
							printf("\n-----------------WITHDRAW-----------\n");
							while(1)
							{
								printf("Enter amount(>0) to withdraw: ");
								scanf("%s",amount);
								amt=atoi(amount);
								if(amt>0)
									break;
							}
							write(socket_desc, &amt, sizeof(amt));	// sending amount to server.
							read(socket_desc, &temp, sizeof(temp));	// balance 
							if(amt<=temp)
							{
								printf("withdrawn\n");
								printf("Balance = %d\n",(temp-amt));
							}
							else
							{	
								printf("\nInsufficient Funds!!\nYour balance is %d\n", temp);
							}
							break;
						
						case 3: 
							printf("\n-----------------BALANCE ENQUIRY-----------\n");
							printf("%s\n", username);
							read(socket_desc, &temp, sizeof(temp));	// account number 
							printf("Account Number is = %d\n", temp);
							read(socket_desc, &temp, sizeof(temp));	// balance 
							printf("Balance is = %d\n", temp);
							break;
							
						case 4:
							printf("\n-----------------PASSWORD CHANGE-----------\n");
							printf(" Password Change \n ");
							printf("Enter new password: ");
							scanf("%s", buff);
							write(socket_desc, buff, sizeof(buff));	// send new password to server.
							break;
							
						case 5:
							printf("\n-----------------VIEW DETAILS-----------\n");
							printf("Hi %s, Your account details are:\n", username);
							read(socket_desc, &temp, sizeof(temp));	// account number. 
							printf("Account number= %d\n", temp);
							read(socket_desc, &temp, sizeof(temp));	// type of customer. 
							printf("Type of Account: ");
							if(temp==1)
								printf("Normal user\n");
							else if(temp==2)
								printf("Joint account\n");	
							read(socket_desc, &temp, sizeof(temp));	// account status. 
							if(temp==0)
								printf(" Your Account is inactive\n");
							else
								printf("Your Account is active\n");
							break;
						
						default:
							printf("\n-----------------EXIT-----------\n");
							printf("Exiting.....\n");
							lastid(last_accno);
							exit(0);
					}
				}
				
			}
			else if(p==2)	//Joint user
			{
				printf("\n===================Welcome Joint user: %s===================\n", username);
				int flag=0;
				int ano;
				struct account x;
				printf("Enter your Acc no.: ");
				scanf("%d",&ano);
				x = searchDetails(ano);
				if(isjointaccvalid(ano,username)) //To check if the user and accno matches
				{
					printf("Enter correct Account no.\n");
					exit(0);
				}
				write(socket_desc, &ano, sizeof(ano));
				for(;;)
				{
					printf("Choose one option:\n1.Deposit\n2.Withdraw\n3.Balance Enquiry\n4.Password change\n5.View details\n6.Exit\n");
					scanf("%d", &ch);
					write(socket_desc, &ch, sizeof(ch));			// write choice to server.
					int accno, type, user_status, amt, gar;
					char amount[50];			
					switch(ch)
					{
						case 1:
							printf("\n-----------------DEPOSIT-----------------\n");
							while(1)
							{
								printf("\nEnter amount(>0) to deposit: ");
								scanf("%d", &amt);
								if(amt>0)
									break;
							}
							write(socket_desc, &amt, sizeof(amt));	// send amt to deposit to server.
							break;
						
						case 2:
							printf("\n-----------------WITHDRAW-----------------\n");
							int temp, cur_bal;
							while(1)
							{
								printf("Enter amount(>0) to withdraw: ");
								scanf("%s",amount);
								amt=atoi(amount);
								if(amt>0)
									break;
							}
							write(socket_desc, &amt, sizeof(amt));	// sending amount to server.
							read(socket_desc, &temp, sizeof(temp));	// success state of operation
							read(socket_desc, &cur_bal, sizeof(cur_bal));	//balance available		
							if(temp!=-1)
							{
								printf("Withdrawn\n");
								printf("Balance = %d\n",cur_bal);
							}
							else if(temp==-1)
							{	
								printf("\nInsufficient Funds!!\n");
							}
							break;
						
						case 3: 
							printf("\n-----------------BALANCE ENQUIRY-----------------\n");
							printf("%s\n", username);
							printf("Account Number is = %d\n", ano);
							read(socket_desc, &temp, sizeof(temp));	// balance 
							printf("balance is = %d\n", temp);
							break;
							
						case 4:
							printf("\n-----------------PASSWORD CHANGE-----------------\n");
							printf(" Password Change \n ");
							printf("Enter new password: ");
							scanf("%s", buff);
							write(socket_desc, buff, sizeof(buff));	// send new password to server.
							break;
							
							
						case 5:
							printf("\n-----------------VIEW DETAILS-----------------\n");
							printf("%s's Account Details are\n", username);						
							read(socket_desc, &temp, sizeof(temp));	// account number.
							printf("Account number= %d\n", temp);
							read(socket_desc, &temp, sizeof(temp));	// type of customer. 
							if(temp==1)
								printf("Normal user\n");
							else if(temp==2)
								printf("Joint account\n");	
							read(socket_desc, &temp, sizeof(temp));	// account status. 
							if(temp==0)
								printf("Account inactive\n");
							else
								printf("Account is active\n");
							break;
						
						default:
							printf("\n-----------------EXIT-----------------\n");
							printf("Exiting.....\n");
							lastid(last_accno);
							exit(0);	
					}
				}
			}
			else if(p==3)
			{
				printf("\n===================Welcome admin: %s===================\n", username);
				for(;;)
				{
					printf("\nchoose one option:\n1.Add Account Details\n2.Delete Account Details\n3.Modify Account Details\n");
					printf("4.Search Account Details\n5. Transactions History\n6. exit\n\n");
					scanf("%d", &ch);
					write(socket_desc, &ch, sizeof(ch));
					int account_no, type, user_status, temp, opn, user_type, val;
					switch(ch)
					{
						case 1: 
							printf("\n-----------------ADD USER-----------------\n");
							printf("\nPress\n1. Single user account\n2. Joint user account: ");
							scanf("%d", &type);
							write(socket_desc, &type, sizeof(type));			// user type to server. 1 -> single 2->joint
							if(type==1)		// single user.
							{
								last_accno++;
								printf("Enter name of user to add: ");
								scanf("%s", username);
								write(socket_desc, username, sizeof(username));
								printf("Enter password of user to add: ");
								scanf("%s", password);
								write(socket_desc, password, sizeof(password));
								printf("username: %s password: %s\n", username, password);
								account_no = last_accno;
								user_status=1;
								write(socket_desc, &account_no, sizeof(account_no));	 // accno to server.
								write(socket_desc, &user_status, sizeof(user_status));   // user status to server.
								printf("Your Account No. is: %d\n", account_no);
							}
							else if(type==2)				// joint user.
							{
								last_accno++;
								printf("Enter name of user1 to add: ");
								scanf("%s", username1);								
								printf("Enter password of user1 to add: ");
								scanf("%s", password1);								
								printf("Enter name of user2 to add: ");
								scanf("%s", username2);								
								printf("Enter password of user2 to add: ");
								scanf("%s", password2);
								printf("User1's name: %s & Password: %s\nUser2's name: %s & Password: %s\n\n", username1, password1, username2, password2);
								if(strcmp(username1,username2)==0)
								{
									last_accno--;
									lastid(last_accno);
									printf("Both the users can't have same username\n");
									exit(0);
								}
								write(socket_desc, username1, sizeof(username1));
								write(socket_desc, password1, sizeof(password1));
								write(socket_desc, username2, sizeof(username2));
								write(socket_desc, password2, sizeof(password2));				
								account_no = last_accno;
								user_status=1;
								write(socket_desc, &account_no, sizeof(account_no));	 // accno to server.
								write(socket_desc, &user_status, sizeof(user_status));   // user status to server.
								printf("Your Account No. is: %d\n", account_no);
							}
							else
							{
								printf("Choose correct choice\n");
								exit(0);
							}				
							read(socket_desc, &temp, sizeof(temp));		
							if(temp==0)
							{
								last_accno--;
								printf("Username already exists\n\n");
								exit(0);
							}
							break;
						
						case 2:
							int accno;
							printf("\n-----------------DELETE ACCOUNT-----------\n");
							printf("Enter single user(1) or joint user(2): ");
							scanf("%d", &user_type);
							if(user_type==1||user_type==2)
							{
							write(socket_desc, &user_type, sizeof(user_type));	// send user_type to server.
							printf("Enter account number to delete: ");
							scanf("%d", &accno);
							struct account x = searchDetails(accno);
							if(x.acc_status==-10) 	//To check if accno matches
							{
								printf("Enter correct Account no.\n");
								exit(0);
							}
							write(socket_desc, &accno, sizeof(accno));		// send accno to server.
							read(socket_desc, &val, sizeof(val));
							if(val!=-1)
								printf("Your Account is withdrawn by %d amount and Deleted!\n\n",val);
							}
							else
							{
								printf("Choose correct choice\n");
								exit(0);
							}
							break;
							
						case 3:
							printf("\n------------------MODIFY ACCOUNT---------------\n");
							printf("Enter\n1. Normal user\n2. Joint user: ");
							scanf("%d", &user_type);
							if(user_type==1||user_type==2)
							{
							printf("Enter Account Number to modify: ");
							scanf("%d", &temp);
							struct account x = searchDetails(temp);
							if(x.acc_status==-10) 	//To check if accno matches
							{
								printf("Enter correct Account no.\n");
								exit(0);
							}				
							write(socket_desc, &user_type, sizeof(user_type));		// send user_type to server.
							write(socket_desc, &temp, sizeof(temp));			// send account number to server.
							read(socket_desc, &temp, sizeof(temp));
							if(temp==1)
							{
								printf("You are reactivating the Account\n");
								printf("Account Successfully reactivated\n");
							}
							else if(temp==-1)
							{
								printf("Account not found\n");
								exit(0);
							}
							}
							else
							{
								printf("Choose correct choice\n");
								exit(0);
							}
							break;
							
						case 4:
							printf("\n-----------------SEARCH ACCOUNT-----------\n");
							printf("Enter account number to search for details:");
							scanf("%d", &accno);
							if(!(accno>=1001 && accno<=last_accno))	
							{
								printf("Account Number doesnt exist\n");
								break;
							}
							write(socket_desc, &accno, sizeof(accno));  // send accno to server 
							int bal, as;
							read(socket_desc, &bal, sizeof(bal));	// balance 
							read(socket_desc, &as, sizeof(as));	// account status. 
							if(as == -10)
							{
								printf("Account Number doesnt exist\n");
							}
							else
							{
								printf("Account Details\nAccount number = %d\n", accno);
								printf("Balance= %d\n", bal);
								if(as==0)
									printf("Account status= %d(inactive)\n\n", as);
								else
									printf("Account status= %d(active)\n\n", as);
							}
							break;
						case 5:
							printf("\n--------------Transactions History--------------\n");
							printf("Date	   Accno	Trans_amount	   Cur_Balance\n");
							getTransactions();
							printf("\n\n");
							break;
						default:
							lastid(last_accno);
							exit(0);
					}
				}
			}
			else
				exit(0);
		}
		else
		{
			printf("wrong password\nenter details again!!\n");
		}
	}
	 else
	{
	 	printf("wrong username\nenter again\n");
	}
	}
	else
	{
		printf("Exiting enter correct choice\n");
		exit(0);
	}
	}
	return (0);
}

#define size 1000


struct customer{
	char username[size];
	char password[size];
	int accno;
	int type;  // 1 -> normal 2 -> joint 3-> admin
	int user_status;  
};

struct jointcustomer{
	char username1[size];
	char password1[size];
	char username2[size];
	char password2[size];
	int accno;
	int type;  // 1 -> normal 2 -> joint 3-> admin
	int user_status;  
};


struct account{

	int accno;
	int balance;
	int acc_status;  // 0 -> inactive 1 -> active

};

struct transaction{

	char date[size];
	int accno;
	int amount;
	int balance;		
};


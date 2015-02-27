#include <iostream>
#include "client.h"
using namespace std;

int main()
{
	
	Client c;
	string server_addr;
	int portno;
	printf("Enter server address : ");
	cin>>server_addr;
	printf("\n portno: ");
	cin>>portno;
	c.connectToServer(c.stringToChar(server_addr),portno);
	char a[80];
	char opt;
	while(1)
	{
		printf("Enter message : ");
		cin>>a;
		c.writeCommand(a);
		cout<<"Enter filename: ";
		cin>>a;
		c.writeToServer(c.stringToChar(a));
		cout<<"Enter more ?";
		cin>>opt;
		if(opt == 'n' || opt == 'N')
			break;
	}

return 0;

}
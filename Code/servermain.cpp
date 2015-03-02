#include "server.h"
#include <iostream>

using namespace std;

int main()
{
	Server s;
	int portno;
	cout<<"Enter port no.";
	cin>>portno;
	s.CreateServer(portno);
}
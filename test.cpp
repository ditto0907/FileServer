#include <stdio.h>   
#include <stdlib.h>   
#include <string.h> 

#include <iostream>
//#include <pthread.h>
//Linux socket 
  
#include <sys/types.h>   
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <unistd.h> 	//close函数
#include <netdb.h>   	//gethostbyname
#include <arpa/inet.h>	//inet_ntoa
#include <sys/select.h>	//select

#include <sys/wait.h> 
#include <sys/time.h> 

#include <fcntl.h>
#include <dirent.h>

using namespace std;


class cParse
{
public:
	cParse()
	{
		
		portstr = (char*)malloc(sizeof(char)*6);
		
		method = (char*)malloc(sizeof(char)*5);

		filename = (char*)malloc(sizeof(char)*30);
		
		ipaddr = (char*)malloc(sizeof(char)*30);

		
		
	};
	~cParse()
	{
		
		printf("free filename\n");
		free(filename);
		
		printf("free ipaddr\n");
		free(ipaddr);


		printf("free portstr\n");
		free(portstr);

		printf("free method\n");
		free(method);
		
	};


	char* portstr;
	char* method;
	char* filename;
	char* ipaddr;

};

int main()
{
	cParse myparse;

	return 0;
}
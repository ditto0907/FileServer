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

using namespace std;



#define IPSTR "127.0.0.1" 
#define PORT 8091 


int main()
{
	
	int sockfd;
	char *sendCommand = (char*)"get -h 127.0.0.1 -p 8091 test.txt";
	char recvBuf[409600];
	struct sockaddr_in servaddr;


	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) 
	{ 
		printf("socket error!\n"); 
		exit(0);
    }

    bzero(&servaddr, sizeof(servaddr)); 
    servaddr.sin_family = AF_INET; 
   	servaddr.sin_port = htons(PORT); 

   	if (inet_pton(AF_INET, IPSTR, &servaddr.sin_addr) <= 0 )    
	{ 
		printf("inet_pton error!\n"); 
		exit(0); 
	}

	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) 
	{     
	    printf("connect error!\n"); 
	    exit(0); 
	}

	int ret = send(sockfd,sendCommand,strlen(sendCommand),0); 
	if (ret < 0) 
	{ 
		printf("send error!!"); 
		exit(0); 
	}
	else
	{ 
		printf("send success ,total send %d \n", ret); 
	}
	while(1)
	{
		int i = recv(sockfd, recvBuf, 4092,0);
		if(i>0)
		{
			printf("%s\n",recvBuf);
		}
	}
	

	return 0;
}
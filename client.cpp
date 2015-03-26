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



#define IPSTR "127.0.0.1" 
#define PORT 8091 

class cParse
{
public:
	cParse()
	{		
		filename = (char*)malloc(sizeof(char)*30);
		
		ipaddr = (char*)malloc(sizeof(char)*30);

		portstr = (char*)malloc(sizeof(char)*30);

		method = (char*)malloc(sizeof(char)*5);

	};
	~cParse()
	{
		printf("free method\n");
		free(method);
		
		printf("free filename\n");
		free(filename);
		
		printf("free ipaddr\n");
		free(ipaddr);

		printf("free portstr\n");
		free(portstr);
	};

	char* portstr;
	char* method;
	char* filename;
	char* ipaddr;
	
	int port;
	
	int GetCommand(char* commandline);
};

class cFile
{
public:
	//cFile();
	//~cFile();
	int OpenFile(char *filename);
	int ReadFile(int fd,char* &outbuff,int readsize);
	int WriteFile(int fd,char* &inbuff,int writesize);

};

class cSocket
{
public:
	cSocket()
	{
		serverip = (char*)malloc(sizeof(char)*20);
	};
	~cSocket()
	{
		printf("free serverip\n");
		free(serverip);
	};

	int clientSocket;
	int serverport;
	char* serverip;

	//int listenSocket;
	struct sockaddr_in serveraddr;

	char recvCommand[100];

	//char sendBuf[1024000];

	int SocketInit();
	int ReceiveCommand();
	int SendCommand(char* commandline);
	int SendFile(char *filename);
	int RecvFile(char* filename);
	int Close();
};


int main()
{
	cParse myparse;
	cSocket mysocket;
	cFile myfile;

	char *commandline = (char*)"get -h 127.0.0.1 -p 8091 test.txt";
	myparse.GetCommand(commandline);
	//mysocket.serverip = myparse.ipaddr;
	//mysocket.serverport = 8090;

	//mysocket.SocketInit();


	return 0;
}


int cParse::GetCommand(char* commandline)
{


	char *p = strstr(commandline,"put");
	if(p)
	{
		strcpy(method,"put");
	}
	else
	{
		p = strstr(commandline,"get");
		if(p)
		{
			strcpy(method,"get");
		}
		else
		{
			strcpy(method,"error");
			return -1;
		}
	}

	p = strstr(commandline,"-h");
	if(p)
	{
		p = p+3;
		char *end = p;
		while(*end!=' ')
			end++;
		end++;
		strcpy(ipaddr,p);
		*(ipaddr+(end-p)) = '\0';
	}
	else
	{
		return -2;
	}

	p = strstr(commandline,"-p");
	if(p)
	{
		p = p+3;
		char *end = p;
		while(*end!=' ')
			end++;
		end++;
		strcpy(portstr,p);
		*(portstr+(end-p)) = '\0';
		strcpy(filename,end);
	}
	else
	{
		return -3;
	}
	printf("%s\n",method);
	printf("%s\n",filename);
	printf("%s\n",ipaddr);
	printf("%s\n",portstr);

	return 1;
}

int cFile::OpenFile(char *filename)
{
	int fd;
	int error = 0;
	try
	{
		if((fd = open(filename,O_RDONLY))==-1)
		{
			error = fd;
			throw error;
		}
	}
	catch(int &error)
	{
		switch(error)
		{
			case -1:
				printf("OpenFile failed\n");
				printf("%s\n", filename);
				break;
			default:
				break;
		}
	}
	return fd;
}

int cFile::ReadFile(int fd,char* &outbuff,int readsize)
{
	// wait .....
	//outbuff = (char*)"This is my test file wawaawawawawawawawawaw\n";
	int nbyte;
	nbyte = read(fd, outbuff, readsize);
	return nbyte;
}

int cFile::WriteFile(int fd,char* &inbuff,int writesize)
{	
	

}

int cSocket::SocketInit()
{
	
	bzero(&serveraddr, sizeof(serveraddr)); 
    serveraddr.sin_family = AF_INET; 
   	serveraddr.sin_port = htons(serverport); 

	if ((clientSocket= socket(AF_INET, SOCK_STREAM, 0)) < 0 ) 
	{ 
		printf("socket error!\n"); 
		return -1;
    }
   	if (inet_pton(AF_INET, serverip, &serveraddr.sin_addr) <= 0 )    
	{ 
		printf("inet_pton error!\n"); 
		return -2; 
	}
	if (connect(clientSocket, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) 
	{     
	    printf("connect error!\n"); 
	    return -3; 
	}
	return 1;
}

int cSocket::ReceiveCommand()
{
/*
	bzero(&recvCommand,sizeof(recvCommand));

	socklen_t len = sizeof(serveraddr);

	serverSocket = accept(listenSocket,(struct sockaddr*)&serveraddr,&len); 

	int n = recv(serverSocket,recvCommand,1024,0);

	printf("Accept command success!!\n");
	printf("%s\n", recvCommand);
	return 1;*/

}

int cSocket::SendCommand(char* commandline)
{

}

int cSocket::SendFile(char *filename)
{
	//char *outbuff = (char*)malloc(sizeof(char)*4096); 
	//outbuff = (char *)"This is my test file\n";
	cFile myfile;
	int fd,nbyte = 0;;
	char* sendbuf = (char*)malloc(sizeof(char)*1024);

	if((fd = myfile.OpenFile(filename))>0)
	{	
		while((nbyte = myfile.ReadFile(fd,sendbuf,1024))>0)
		{
			int ret = send(clientSocket,sendbuf,strlen(sendbuf),0);	
			if(ret)
			{
				printf("Send success!!\n");
				//printf("Send buffer:\n%s\n",sendbuf);
			}
			else
			{
				printf("Send failed\n");
				return -1;
			}
		}

	}
	free(sendbuf);
	return 1;
}

int cSocket::RecvFile(char* filename)
{
	int i ;
	char *recvBuf = (char*)malloc(sizeof(char)*4096);
	while((i = recv(clientSocket, recvBuf, 4096,0))>0)
	{

		printf("Recv success!!\n");
		printf("%s\n",recvBuf);
	}

	free(recvBuf);
	return	1;

}

int cSocket::Close()
{
	close(clientSocket);
	return 1;
}

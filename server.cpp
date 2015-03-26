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
		method = (char *)malloc(sizeof(char)*5);
		filename = (char *)malloc(sizeof(char)*30);
	};
	~cParse()
	{
		delete(method);
		delete(filename);
	};

	char* method;
	char* filename;
	
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
	//cSocket();
	//~cSocket();

	int serverSocket;
	int listenSocket;
	struct sockaddr_in serveraddr;

	char recvCommand[100];

	//char sendBuf[1024000];

	int SocketInit();
	int ReceiveCommand();
	int SendFile(char *filename);
	int RecvFile(char* filename);
	int Close();
};


int main()
{
	printf("This is file Server\n");

	cSocket myserver;
	cParse  myparse;
	//cFile 	myfile;

	char *sendbuffer = (char*)malloc(sizeof(char)*102400);
	
	myserver.SocketInit();
	

	while(1)
	{
		myserver.ReceiveCommand();

		if(myparse.GetCommand(myserver.recvCommand))
		{
			printf("Parse Succeed\n");
		}
		if(myparse.method == "get")
		{
			myserver.SendFile(myparse.filename);
		}
		else if(myparse.method == "get")
		{
		

		}
	}
	myserver.Close();

}



int cParse::GetCommand(char* commandline)
{
	char *p = strstr(commandline,"put");
	if(p)
	{
		method = (char*)"put";
	}
	else
	{
		p = strstr(commandline,"get");
		if(p)
		{
			method = (char*)"get";
		}
		else
		{
			method = (char*)"error";
			return -1;
		}
	}

	p = strstr(commandline,"-p");
	if(p)
	{
		p = p+3;
		while(*p!=' ')
			p++;
		p++;
		strcpy(filename,p);
	}
	else
	{
		return -2;
	}
	printf("%s\n",method);
	printf("%s\n",filename);

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
	
	bzero(&serveraddr,sizeof(serveraddr));

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htons(INADDR_ANY);
	serveraddr.sin_port = htons(8091);

	listenSocket = socket(AF_INET,SOCK_STREAM,0);

	if(listenSocket<0)
	{
		printf("Creat socket failed!\n");
		exit(0);
	}
	if(bind(listenSocket,(struct sockaddr*)&serveraddr,sizeof(serveraddr))==-1)
	{
		printf("Bind socket failed\n");
		exit(0);
	}
	if(listen(listenSocket,10)==-1)
	{
		printf("Listen socket failed\n");
		exit(0);
	}

	printf("======waiting for client's request======\n");
	return 1;
	
}

int cSocket::ReceiveCommand()
{

	bzero(&recvCommand,sizeof(recvCommand));

	socklen_t len = sizeof(serveraddr);

	serverSocket = accept(listenSocket,(struct sockaddr*)&serveraddr,&len); 

	int n = recv(serverSocket,recvCommand,1024,0);

	printf("Accept command success!!\n");
	printf("%s\n", recvCommand);
	return 1;

}

int cSocket::SendFile(char *filename)
{
	//char *outbuff = (char*)malloc(sizeof(char)*4096); 
	//outbuff = (char *)"This is my test file\n";
	cFile myfile;
	int fd,nbyte = 0;;
	char* sendbuf = (char*)malloc(sizeof(char)*4096);

	if((fd = myfile.OpenFile(filename))>0)
	{	
		while((nbyte = myfile.ReadFile(fd,sendbuf,4096))>0)
		{
			int ret = send(serverSocket,sendbuf,strlen(sendbuf),0);	
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
	return 1;
}

int cSocket::Close()
{
	close(listenSocket);
	close(serverSocket);
	return 1;
}




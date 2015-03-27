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
		free(method);
		free(filename);
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
	int CreatFile(char *filename);
	int ReadFile(int fd,char* &outbuff,int readsize);
	int WriteFile(int fd,char* &inbuff,int writesize);
	int CloseFile(int fd);
};

class cSocket
{
public:
	//cSocket();
	//~cSocket();

	int mySocket;
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
		if(strstr(myparse.method,"get"))
		{
			myserver.SendFile(myparse.filename);

		}
		else if(strstr(myparse.method,"put"))
		{
			myserver.RecvFile(myparse.filename);
		
		}
		else
		{
			printf("Error\n");
			close(myserver.mySocket);
		}

	}
	myserver.Close();

}



int cParse::GetCommand(char* commandline)
{
	char *p = strstr(commandline,"put");
	if(p)
	{
		//method = (char*)"put";
		strcpy(method,"put");
	}
	else
	{
		p = strstr(commandline,"get");
		if(p)
		{
			//method = (char*)"get";
			strcpy(method,"get");
		}
		else
		{
			//method = (char*)"error";
			strcpy(method,"error");
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
	//printf("%s-\n",method);
	//printf("%s-\n",filename);

	return 1;
}

int cFile::OpenFile(char *filename)
{
	int fd;
	int error = 0;
	try
	{
		fd = open(filename,O_RDONLY);
		if(fd<0)
		{
			error = fd;
			throw error;
		}
		else
		{
			return fd;
		}
	}
	catch(int &error)
	{
		switch(error)
		{
			case -1:
				printf("OpenFile failed\n");
				printf("%s,%d\n", filename,fd);
				return error;
				break;
			default:
				break;
		}
	}
	return fd;
}

int cFile::CreatFile(char *filename)
{
	int fd=0;
	int error = 0;

	//fd = open(filename,O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	try
	{
		fd = open(filename,O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
		if(fd<0)
		{
			error = fd;
			throw error;
		}
		else
			return fd;
	}
	catch(int &error)
	{
		switch(error)
		{
			case -1:
				printf("CreatFile failed\n");
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
	int nbyte;
	nbyte = write(fd,inbuff,writesize);
	return nbyte;	
}

int cFile::CloseFile(int fd)
{
	
	close(fd);
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

	mySocket = accept(listenSocket,(struct sockaddr*)&serveraddr,&len); 

	int n = recv(mySocket,recvCommand,1024,0);

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
			int ret = send(mySocket,sendbuf,strlen(sendbuf),0);	
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
	else
	{
		close(mySocket);
		myfile.CloseFile(fd);
		free(sendbuf);
		return -1;
	}
	close(mySocket);
	myfile.CloseFile(fd);
	free(sendbuf);
	return 1;
}

int cSocket::RecvFile(char* filename)
{
	cFile myfile;
	int fd,nbyte = 0;
	char *recvbuf = (char*)malloc(sizeof(char)*4096);

	//fd = open(filename,O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);

	if((fd = myfile.CreatFile(filename))>0)
	{
		nbyte = recv(mySocket,recvbuf,4096,0);
		while(nbyte>0)
		{
			int ret = myfile.WriteFile(fd,recvbuf,nbyte);
			nbyte = recv(mySocket,recvbuf,4096,0);
			printf("Recv success!!%d\n",ret);
		}
	}
	else
	{
		close(mySocket);
		myfile.CloseFile(fd);
		free(recvbuf);
		printf("Recv failed %d!!\n",fd);
		return -1;
	}
	close(mySocket);
	myfile.CloseFile(fd);
	free(recvbuf);
	printf("done\n");
	return	1;
}

int cSocket::Close()
{
	close(listenSocket);
	close(mySocket);
	return 1;
}




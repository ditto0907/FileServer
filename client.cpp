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
		filename = (char*)malloc(sizeof(char)*30);
		
		ipaddr = (char*)malloc(sizeof(char)*30);

		portstr = (char*)malloc(sizeof(char)*30);

		method = (char*)malloc(sizeof(char)*5);

	};
	~cParse()
	{
		//printf("free method\n");
		free(method);
		
		//printf("free filename\n");
		free(filename);
		
		//printf("free ipaddr\n");
		free(ipaddr);

		//printf("free portstr\n");
		free(portstr);
	};

	char* portstr;
	char* method;
	char* filename;
	char* ipaddr;
	
	int port;
	
	int GetCommand(char* commandline);
	int GetPort(char *portstr);
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
	cSocket()
	{
		serverip = (char*)malloc(sizeof(char)*20);
	};
	~cSocket()
	{
		printf("free serverip\n");
		free(serverip);
	};

	
	int serverport;
	char* serverip;

	int mySocket;
	int listenSocket;
	struct sockaddr_in serveraddr;
	char recvCommand[100];


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
	cSocket myclient;
	cFile myfile;

	char *commandline = (char*)malloc(sizeof(char)*40);
	
	while(1)
	{
		cin.getline(commandline,40);

		int ret = 0;

		if((ret = myparse.GetCommand(commandline))>0)
		{	
			myclient.serverip = myparse.ipaddr;
			myclient.serverport = myparse.port;
			myclient.SocketInit();
			myclient.SendCommand(commandline);

			if(strstr(myparse.method,"get"))
			{
				myclient.RecvFile(myparse.filename);
				printf("Downloaded\n");
			}
			else if(strstr(myparse.method,"put"))
			{
				myclient.SendFile(myparse.filename);
				printf("Uploaded\n");
			}
			else
			{
				printf("Error\n");
				close(myclient.mySocket);
			}
		}
		else
		{
			cout<<ret<<endl;
		}

		
	}

	myclient.Close();
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
		*(ipaddr+(end-p-1)) = '\0';
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
		*(portstr+(end-p-1)) = '\0';
		strcpy(filename,end);
	}
	else
	{
		return -3;
	}

	GetPort(portstr);

	//printf("%s-\n",method);
	//printf("%s-\n",filename);
	//printf("%s-\n",ipaddr);
	//printf("%s-\n",portstr);
	//printf("%d-\n",port);

	return 1;
}

int cParse::GetPort(char *portstr)
{
	int len = strlen(portstr);
	port = 0;
	for(int i = 0;i<len;i++)
	{
		port = port*10+(*(portstr+i)-'0');
	}
	//printf("%d,%d\n",len,port);
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
				printf("%s\n,%d", filename,fd);
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
	
	bzero(&serveraddr, sizeof(serveraddr)); 
    serveraddr.sin_family = AF_INET; 
   	serveraddr.sin_port = htons(serverport); 

	if ((mySocket= socket(AF_INET, SOCK_STREAM, 0)) < 0 ) 
	{ 
		printf("socket error!\n"); 
		return -1;
    }
   	if (inet_pton(AF_INET, serverip, &serveraddr.sin_addr) <= 0 )    
	{ 
		printf("inet_pton error!\n"); 
		return -2; 
	}
	if (connect(mySocket, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) 
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
	int ret = send(mySocket,commandline,strlen(commandline),0);
	if (ret < 0) 
	{ 
		printf("send error!!"); 
		return 0; 
	}
	else
	{ 
		printf("send command success ,total send %d \n", ret);
		//printf("%s\n",commandline); 
	}
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
	close(mySocket);
	return 1;
}

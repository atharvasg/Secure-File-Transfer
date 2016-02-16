#include<stdio.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<stdlib.h>

#define MAX 256
#define PORT 43454
void communicate(int socketdescriptor)
{
	char buffer[MAX];
	int n;
	for(;;)
	 {
		memset(buffer,0,sizeof(buffer));		
		printf("To Server : ");
		n=0;
		while((buffer[n++]=getchar())!='\n');
		write(socketdescriptor,buffer,sizeof(buffer));
		memset(buffer,0,sizeof(buffer));
		read(socketdescriptor,buffer,sizeof(buffer));
		printf("\t\t\tFrom Server : %s",buffer);
		if((strncmp(buffer,"exit",4))==0)
		 {
			printf("Client Exit...\n");
			break;
		 }
	 }
}

int main(int argc,char *argv[])
{
	int socketdescriptor,connectiondescriptor,flag=0;
	//unsigned int ipaddress;
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));	
	if(argc>1)
		{
			printf("%s\n",argv[1]);
			servaddr.sin_addr.s_addr=inet_addr(argv[1]);
		}
	else
		{
		 	servaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
		}
	socketdescriptor=socket(AF_INET,SOCK_STREAM,0);
	if(socketdescriptor==-1)
	 {
		printf("socket creation failed...\n");
		exit(0);
	 }
	else
		printf("Socket successfully created..\n");
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(PORT);
	
	if(connect(socketdescriptor,(struct sockaddr *)&servaddr,sizeof(servaddr))!=0)
	 {
		printf("connection with the server failed...:%d\n",connect(socketdescriptor,(struct sockaddr *)&servaddr,sizeof(servaddr)));
		exit(0);
	 }
	else
		printf("connected to the server..\n");
	communicate(socketdescriptor);
	close(socketdescriptor);
}

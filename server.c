#include<stdio.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<stdlib.h>
#include<string.h>
#define MAX 256
#define PORT 43454
void communicate(int sockfd)
{
	char buffer[MAX];
	int n,status;
	for(;;)
	 {
		n=0;
		memset(buffer,0,sizeof(buffer));
		read(sockfd,buffer,sizeof(buffer));
		printf("\t\t\tFrom client: %s\nTo client : ",buffer);
		memset(buffer,0,MAX);
		while((buffer[n++]=getchar())!='\n');
		status=write(sockfd,buffer,sizeof(buffer));
		if(status==-1)
		{
		  printf("-------------ERROR--------------\n");
		  printf("Client exited or error in communication!\n");
		}		
		if(strncmp("exit",buffer,4)==0)
		 {
			printf("Server Exit...\n");
			break;
		}
	 }
}

int main()
{
	int socketdescriptor,conndescriptor,len;
	struct sockaddr_in servaddr,cli;
	socketdescriptor=socket(AF_INET,SOCK_STREAM,0);
	if(socketdescriptor==-1)
	 {
		printf("socket creation failed...\n");
		exit(0);
	 }
	else
		printf("Socket successfully created..\n");
	memset(&servaddr,0,sizeof(servaddr));	
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(PORT);
	if((bind(socketdescriptor,(struct sockaddr *)&servaddr, sizeof(servaddr)))!=0)
	 {
		printf("socket bind failed...\n");
		exit(0);
	 }
	else
	printf("Socket successfully binded..\n");
	if((listen(socketdescriptor,5))!=0)
	 {
		printf("Listen failed...\n");
		exit(0);
	 }
	else
		printf("Server listening..\n");
	len=sizeof(cli);
	conndescriptor=accept(socketdescriptor,(struct sockaddr *)&cli,&len);
	if(conndescriptor<0)
	 {
		printf("server acccept failed...\n");
		exit(0);
	 }
	else
		printf("server acccept the client...\n");
	communicate(conndescriptor);
	close(socketdescriptor);
}

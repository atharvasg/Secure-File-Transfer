#include<stdio.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<stdlib.h>

#define MAX 256
#define PORT 43455
void communicate(int socketdescriptor)
{
	char buffer[MAX];
	int n;
	for(;;)
	 {
		memset(buffer,0,sizeof(buffer));		
		printf("To Server: ");
		n=0;
		while((buffer[n++]=getchar())!='\n');
		write(socketdescriptor,buffer,sizeof(buffer));								//step 3 - sending the data to server.
		memset(buffer,0,sizeof(buffer));
		read(socketdescriptor,buffer,sizeof(buffer));								//step 4 - reading the data sent by server
		printf("\t\t\tFrom Server : %s",buffer);
		if((strncmp(buffer,"exit",4))==0)
		 {
			printf("Client will Exit...End of communication\n");
			break;
		 }
	 }
}

int main(int argc,char *argv[])
{
	int socketdescriptor,connectiondescriptor;
	//unsigned int ipaddress;
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));	
	if(argc>1)
		{
			servaddr.sin_addr.s_addr=inet_addr(argv[1]);
		}
	else
		{
		 	servaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
		}
	socketdescriptor=socket(AF_INET,SOCK_STREAM,0);									//step 1- Creating a socket and returning it's ID i.e. Socketdescriptor
	if(socketdescriptor==-1)
	 {
		printf("socket creation failed...\n");
		exit(0);
	 }
	else
		printf("Socket successfully created..\n");
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(PORT);
	
	if(connect(socketdescriptor,(struct sockaddr *)&servaddr,sizeof(servaddr))!=0)					//step 2- Connect to server on the IP and port provided.
	 {
		printf("connection with the server failed...:%d\n",connect(socketdescriptor,(struct sockaddr *)&servaddr,sizeof(servaddr)));
		exit(0);
	 }
	else
		printf("connected to the server..\n");
	communicate(socketdescriptor);											//function for sending and receiving data from server.
	close(socketdescriptor);											//closing the socket after the communcation completes.
} 

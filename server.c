#include<stdio.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<stdlib.h>
#include<string.h>
#define MAX 256
#define PORT 43455
void communicate(int socketdescriptor,struct sockaddr_in cli,int len)
{
	char buffer[MAX];
	int n=0;
	int a=0;
	for(;;)																//server will be running continuously
	 {
		n=0;
		memset(buffer,0,sizeof(buffer));
		a = recvfrom(socketdescriptor,buffer,sizeof(buffer),0,(struct sockaddr *)&cli,&len);				//step 5 - Reading the data sent by client.	
		if(a<0)
			printf("Receive Failed!!");	
		printf("\n");
		printf("\t\t\tFrom client: %s\nTo client : ",buffer);
		memset(buffer,0,MAX);
		while((buffer[n++]=getchar())!='\n');
		a = sendto(socketdescriptor,buffer,sizeof(buffer),0,(struct sockaddr *)&cli,len);				//step 6 - Sending the message to client.		
		if(a<0)		
			printf("Sending Failed!!\n");								
		if(strncmp("exit",buffer,4)==0)
		 {
			printf("Server will Exit...end of communication\n");
			break;
		}
	 }
}

int main(int argc,char *argv[])
{
	int socketdescriptor,len;
	struct sockaddr_in servaddr,cli;

	socketdescriptor=socket(AF_INET,SOCK_DGRAM,0);					//step 1- Creating a socket and returning it's ID i.e. Socketdescriptor
	if(socketdescriptor==-1)
	 {
		printf("socket creation failed...\n");
		exit(0);
	 }
	else
		printf("Socket successfully created..\n");
	memset(&servaddr,0,sizeof(servaddr));								// filling data in structure sockaddr_in.
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);							// host to network byte conversion for maintaining uniformity inIP address.
	if(argc>1)
	{
		servaddr.sin_port=htons(atoi(argv[1]));		//for taking port from user	
	}
	else
	{
		printf("No port provided...hence default port 43454 will be used\n");
		servaddr.sin_port=htons(PORT);									// host to network byte conversion for maintaining uniformity in port no.
	}
	if((bind(socketdescriptor,(struct sockaddr *)&servaddr, sizeof(servaddr)))!=0)					//Step 2- binding socket to the IP and port provided. 
	 {
		printf("socket bind failed...\n");
		exit(0);
	 }
	else
		printf("Socket successfully binded..waiting for clients to connect..\n");
	len=sizeof(struct sockaddr_in);
        communicate(socketdescriptor,cli,len);									//function for sending and receiving data from client
	close(socketdescriptor);									//closing the socket after the communcation completes.
}

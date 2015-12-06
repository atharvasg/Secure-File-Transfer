#include<stdio.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<stdlib.h>
#define MAX 256
#define PORT 43455
void communicate(int socketdescriptor,struct sockaddr_in server,int len)
{
	char buffer[MAX];
	int n=0;
	int a=0;
	for(;;)
	 {
		memset(buffer,0,sizeof(buffer));		
		printf("To Server: ");
		n=0;
		while((buffer[n++]=getchar())!='\n');
		a = sendto(socketdescriptor,buffer,sizeof(buffer),0,(struct sockaddr *)&server,len);			//step 3 - sending the data to server.
		
		memset(buffer,0,sizeof(buffer));
		a = recvfrom(socketdescriptor,buffer,sizeof(buffer),0,(struct sockaddr *)&server,len);			//step 4 - reading the data sent by server
		printf("Received Message:%s/n",buffer);
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
	int socketdescriptor,length;
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
	socketdescriptor=socket(AF_INET,SOCK_DGRAM,0);							//step 1- Creating a socket and returning it's ID i.e. Socketdescriptor
	printf("Descriptor:%d\n",socketdescriptor);
	if(socketdescriptor==-1)
	 {
		printf("socket creation failed...\n");
		exit(0);
	 }
	else
		printf("Socket successfully created..\n");
	servaddr.sin_family=AF_INET;
	if(argc>2)
	{
		printf("ARGS:%d\n",atoi(argv[2]));
		servaddr.sin_port=htons(atoi(argv[2]));		//for taking port from user	
	}
	else
	{
		servaddr.sin_port=htons(PORT);
	}
	length=sizeof(struct sockaddr_in);
	communicate(socketdescriptor,servaddr,length);											//function for sending and receiving data from server.
	close(socketdescriptor);											//closing the socket after the communcation completes.
} 

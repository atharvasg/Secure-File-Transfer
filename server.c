#include<stdio.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<stdlib.h>
#include<string.h>
#define MAX 200
#define DATA 196
#define PORT 43455
struct packet
{
	int seqno;
	char data[DATA];
};


void communicate(int socketdescriptor,struct sockaddr_in cli,int len)
{
	char buffer[MAX];
	static int framecounter;	
	FILE *fp;
	char c;
	int n,a,i,flag;
	n=a=i=flag=0;
	framecounter=1;	
	struct packet NewPacket;
	a = recvfrom(socketdescriptor,(void *)&NewPacket,sizeof(buffer),0,(struct sockaddr *)&cli,&len);
	char filename[80];
	
	printf("a is:%d\n",a);	
	if(a<0)
		printf("Problem in receiving!!!\n");
	else
 	{
			while(NewPacket.seqno!=-1)
			{	
		
				switch(NewPacket.seqno)
				{
				  	case 0:
							
							strcpy(filename,NewPacket.data);				
							fp=fopen(NewPacket.data,"w");
							printf("Contents are:-\n");
							printf("1.Seq No:-%d\n",NewPacket.seqno);
							printf("2.data:-%s\n",NewPacket.data);
							fclose(fp);
		
						break;
					case 1: 
							i=0;
							fp=fopen(filename,"a+");
							printf("Inside Case 1:\n");
							/*while(i<DATA)
							{
							//	printf("-----------------------\n");
							//	printf("%c",NewPacket.data[i]);	
							//	printf("-----------------------\n");
								fputc(NewPacket.data[i++],fp);
								//i++;
								if(i>900)
								{
									printf("Infinite Loop...Server will exit now");
									break;
								}
								if(i==DATA)
								{
									printf("i is:%d\n",i);								
									break;
								}
							}*/
							fwrite(NewPacket.data,1,DATA-1,fp);	
							fclose(fp);
							framecounter++;
						break;
					default:
						printf("Inside Default!!\n");	
						flag=1;
						break;
				}
				if(flag==1)
				{
					break;
				}	
				a = recvfrom(socketdescriptor,(void *)&NewPacket,sizeof(struct packet),0,(struct sockaddr *)&cli,&len);
				if(a<0)
				{
					printf("Error while Receiving!!\n");
				}
				else
				{
					printf("Frame %d received successfully\n",framecounter);
				}
				printf("Received Data is:%s\n",NewPacket.data);
				printf("Contents are:-\n");
				printf("1.Seq No:-%d\n",NewPacket.seqno);
				printf("2.data:-%s\n",NewPacket.data);	
			 }
		
	
	}
	//fclose(fp);		
        		

			
	/*for(;;)																//server will be running continuously
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
	 }*/
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

#include<stdio.h>
#include<netinet/in.h>
#include<sys/time.h>
#include<sys/select.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<stdlib.h>
#include<string.h>
#define MAX 200
#define WINDOWSIZE 3
#define DATA 188
#define PORT 43455
struct packet
{
	int seqno;
	int packettype;
	int datasize;
	char data[DATA];
};

struct packet *packetqueue;
int noofpackets=0;
long int datasize=0;
void initPacketqueue()
{
 	int i;
	for(i=0;i<noofpackets;i++)
	{
		packetqueue[i]=(struct packet){0};
	}
}
void communicate(int socketdescriptor,struct sockaddr_storage cli,socklen_t len)
{
	char buffer[MAX];
	char filename[80];
	static int framecounter;
	FILE *fp;
	fd_set rdset,wrset;
	char c;
	int n,a,i,flag,result,maxfd;
	n=a=i=flag=0;
	framecounter=1;
	FD_ZERO(&rdset);	
	struct packet NewPacket,ackpacket;
	a = recvfrom(socketdescriptor,(void *)&NewPacket,sizeof(struct packet),0,(struct sockaddr *)&cli,&len);
	
	if(a<0)
	{
		printf("Problem in receiving!!!\n");
	}
	else
	{
		while(NewPacket.packettype!=-1)
		{	
			switch(NewPacket.packettype)
			{
			  	case 0:
					
					strcpy(filename,NewPacket.data);				
					fp=fopen(NewPacket.data,"w");
					fclose(fp);
					noofpackets=NewPacket.seqno;
					packetqueue=(struct packet *)malloc(sizeof(struct packet)*(noofpackets+1));								
					initPacketqueue();
					break;
								
				 case 1: 
					packetqueue[NewPacket.seqno]=NewPacket;
					ackpacket.seqno=NewPacket.seqno;
					datasize+=strlen(NewPacket.data);
					ackpacket.packettype=2;
					ackpacket.datasize=0;
					strcpy(ackpacket.data,"ACK");	
					a=sendto(socketdescriptor,(const void *)&ackpacket,sizeof(struct packet),0,(struct sockaddr *)&cli,len);
					printf("ACK for frame %d is sent.\n",ackpacket.seqno);
					framecounter++;
					break;
				case 3:
					printf("All the frames are received successfully\n");
					fp=fopen(filename,"a+");
					for(i=0;i<=noofpackets;i++)
					{
						datasize+=strlen(packetqueue[i].data);
						
					}
					for(i=0;i<=noofpackets;i++)
					{
						fwrite(packetqueue[i].data,1,packetqueue[i].datasize,fp);
					}
					fclose(fp);
					flag=1;		
					break;
			}
			if(flag==1)
			{
				break;
			}
			else
			{
				FD_SET(socketdescriptor,&rdset);
				maxfd=socketdescriptor;
				result=select(maxfd+1,&rdset,&wrset,NULL,NULL);
				if(result>0)
				{
					a = recvfrom(socketdescriptor,(void *)&NewPacket,sizeof(struct packet),0,(struct sockaddr *)&cli,&len);
					if(a<0)
					{
						printf("Error while Receiving!!\n");
					}
				}
			}
			
		    }
			if(flag==0)
			{
				fp=fopen(filename,"a+");
				for(i=1;i<=noofpackets;i++)
				{
					fwrite(packetqueue[i].data,1,packetqueue[i].datasize,fp);
				}
				fclose(fp);
			}
		//	printf("Received data:%d\n",datasize);
	
	}
	
		
}

int main(int argc,char *argv[])
{
	int socketdescriptor,len;
	struct sockaddr_in servaddr,cli;
	struct sockaddr_storage serverStorage;
  	socklen_t addr_size, client_addr_size;
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
	addr_size = sizeof(serverStorage);
	len=sizeof(struct sockaddr_in);
        communicate(socketdescriptor,serverStorage,addr_size);									//function for sending and receiving data from client
	close(socketdescriptor);									//closing the socket after the communcation completes.
}

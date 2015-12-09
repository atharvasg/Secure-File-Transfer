#include<stdio.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<stdlib.h>
#define WINDOWSIZE 3
#define NAMESIZE 60
#define SEQNO 1
#define DATA  196
#define MAX 200
#define PORT 43455
//window will be array of pointers  for struct  packet.
struct packet
{
	int seqno;
	char data[DATA];
// 	char data[MAX-sizeof(int)];
};
/*struct 

*/
 
unsigned long int getFileSize(FILE *fp)
{
	unsigned long int filesize=0; 
	fseek(fp, 0L, SEEK_END);
	filesize = ftell(fp);
	fseek(fp,0L,SEEK_SET);
 return filesize;
}
int getnoofPackets(unsigned long int filesize)
{
  int noofpackets=0;
 	noofpackets=filesize/MAX;
	if(filesize%MAX==0)
	{
		return noofpackets;	
	}
	else
	{
		return noofpackets+1;
	}
}
struct packet createPacket(int type,char *data)
{
	struct packet NewPacket;	
	char * packetdata=(char *)malloc(sizeof(DATA));
	strcpy(packetdata,data);
  	switch(type)
  	{
		case 1:
			NewPacket.seqno=0;
			strcpy(NewPacket.data,packetdata);
			break;
		case 2:
			break;
 	}
	printf("Packet Data is:%s\n",NewPacket.data);
  return NewPacket;
}
char * readData(FILE *fp)
{
  	int i=0;
  	char c;
	char *buffer;
	buffer=(char *)malloc(DATA);
	while(c=fgetc(fp)!=EOF)	 
   	{	
		buffer[i++]=c;
		if(i==DATA)
		{
			break;	
		}
   	}
	buffer[i]='\0';
 printf("Printing data size from readData():- %ld\n",sizeof(buffer));
 return buffer;
}

void communicate(int socketdescriptor,struct sockaddr_in server,int len)
{
	unsigned long int filesize=0; 	
	int noofpackets,n,a;
	static int frameNo=0;
	struct packet init;
	char filename[NAMESIZE];
	FILE *fp;
	printf("Enter the file name to be sent:");
	scanf("%s",filename);
        noofpackets=n=a=0;
	init=createPacket(1,filename);
	a=sendto(socketdescriptor,(const void *)&init,sizeof(struct packet),0,(struct sockaddr *)&server,len);
//	if(a>0)
//	{
        	/*fp=fopen(filename,"r");
		if(fp>0)
		{			
			filesize=getFileSize(fp);
			noofpackets=getnoofPackets(filesize);
			while(fgetc(fp)!=EOF)
			{
				
			}
		}
		else
			printf("Error while reading the file!!\n");*/
		for(;;)
		{
		  		
		}
/*		for(;;)
		{
 			frameno=fread(buffer,sizeof(buffer),1,fp);
			while(frameno>0 && i<WINDOWSIZE)
			{			
				a = sendto(socketdescriptor,buffer,sizeof(buffer),0,(struct sockaddr *)&server,len);			//step 3 - sending the data to server.
				if(a>0)
					i++;
				else
				{
					printf("Problem in network!\n");
					break;				
				}
			}		
			printf("Frame No: %d transferred.\n",frameno);	
		}
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
		 }*/
//	}
//	else
//	{
//	  printf();
//	}
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

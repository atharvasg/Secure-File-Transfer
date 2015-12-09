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
	printf("FileSize:%ld\n",filesize);	
 return filesize;
}
int getnoofPackets(unsigned long int filesize)
{
  int noofpackets=0;
 	noofpackets=filesize/DATA;
	if(filesize%DATA==0)
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
	char * packetdata=(char *)malloc(sizeof(DATA-1));
	strcpy(packetdata,data);
  	switch(type)
  	{
		case 1:
			NewPacket.seqno=0;
			strcpy(NewPacket.data,packetdata);
			break;
		case 2:
			NewPacket.seqno=1;
			strcpy(NewPacket.data,packetdata);
			break;
 	}
	printf("Packet Data is:%s\n",NewPacket.data);
  return NewPacket;
}
int getSize(char * buffer)
{
	int i=0;
	while(buffer[i]!='\0')
	{
	 i++;
	}	
 printf("size is:%d",i);
 return i;	
}
char *readData(FILE *fp)
{
  	int i,n;
	char buffer[DATA],c;
	char *p;
	i=n=0;
	memset(buffer,0,sizeof(buffer));	
	n=fread(buffer,1,DATA-1,fp);
	printf("BUFFER is:%s\n",buffer);
	//buffer[DATA]='\0';
	//printf("N IS:%d\n",n);
	if(n<DATA-1)
	{
	
		printf("N IS:%d\n",n);
		if(feof(fp))
		{
			printf("End of file\n");
		}
		else
		{
			printf("Error while reading!!\n");
		}
	}							
	p=buffer;
 	printf("Printing data size from readData():- %d\n",getSize(p));
 return p;
}

void communicate(int socketdescriptor,struct sockaddr_in server,int len)
{
	unsigned long int filesize=0; 	
	int noofpackets,n,a,i;
	static int framecounter=0;
	struct packet init,frame;
	char filename[NAMESIZE];
	char *packetdata;
	FILE *fp;
	noofpackets=n=i=a=0;
	framecounter=1;
	printf("Enter the file name to be sent:");
	scanf("%s",filename);
        init=createPacket(1,filename);
	a=sendto(socketdescriptor,(const void *)&init,sizeof(struct packet),0,(struct sockaddr *)&server,len);
	printf("a is:%d\n",a);
	printf("FileName sent successfully!!\n");        	
	fp=fopen(filename,"r");
	if(fp>0)
	{		
		filesize=getFileSize(fp);
		noofpackets=getnoofPackets(filesize);
		printf("No.of packets will be:%d\n",noofpackets);
				
		for(;;)
		{
			while(i<noofpackets)
			{
			 	//strcpy(packetdata,readData(fp));
				packetdata=readData(fp);
				//printf("PACKET DATA IS:%s\n",packetdata);
			  	frame=createPacket(2,packetdata);	 	
				if(sizeof(frame)>MAX)
			   	{
					printf("ReadFile Failed");
					break;
			   	}	
			  	a=sendto(socketdescriptor,(const void *)&frame,sizeof(struct packet),0,(struct sockaddr *)&server,len);
			  	if(a<0)
			   	{			
					printf("Sending Failed!!");
					break;
			   	}	
				else
				{
					printf("Frame %d sent successully\n",framecounter);
					framecounter++;
					printf("I Is:%d\n",i);
				}
			  	i++;	  	
			}
			fclose(fp);
			if(i==noofpackets)
			{
				printf("ALL the %d frames sent successfully!!\n",i);
				break;			
			}
			
		
		}
	 }
	 else
		printf("Error while reading the file!!\n");
		
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

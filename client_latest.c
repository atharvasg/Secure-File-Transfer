#include<stdio.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/time.h>
#include<sys/socket.h>
#include<sys/select.h>
#include<netdb.h>
#include<math.h>
#include<string.h>
#include<stdlib.h>
#define WINDOWSIZE 3
#define NAMESIZE 60
#define SEQNO 1
#define DATA  188
#define TIMEOUT 1000
#define MAX 200
#define PORT 43455

int framecounter=0;
int sequenceno=1;
int noofpackets=0;
int count=0;
FILE *fp;
struct timeval timeout,lastTime,now;	
struct packet
{
	int seqno;
	int packettype;
	int datasize;	
	char data[DATA];
};
struct packet *LFS,*LAR,*next,*latestack;
struct packet *packetqueue;
unsigned long int getFileSize(FILE *fp)
{
	unsigned long int filesize=0; 
	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	rewind(fp);	
 return filesize;
}
int getnoofPackets(unsigned long int filesize)
{
  	int noofpackets=0;
  	noofpackets=filesize/DATA;
	if((int)filesize%DATA==0)
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
	char packetdata[DATA];
	strncpy(packetdata,data,DATA);
	switch(type)
  	{
		case 1:
			NewPacket.seqno=noofpackets;
			NewPacket.packettype=0;
			NewPacket.datasize=strlen(packetdata);			
			strcpy(NewPacket.data,packetdata);
			break;
		case 2:

			NewPacket.seqno=sequenceno++;
			NewPacket.packettype=1;
			NewPacket.datasize=strlen(packetdata);
			strncpy(NewPacket.data,packetdata,NewPacket.datasize);
			break;
		case 3:
			printf("No. of Packets.%d\n",noofpackets);
			NewPacket.seqno=-1;
			NewPacket.packettype=3;
			strcpy(NewPacket.data,data);
			break;
 	}
  return NewPacket;
}
int getSize(char * buffer)
{
	int i=0;
	while(buffer[i]!='\0')
	{
	 i++;
	}	
 return i;	
}
char *readData(FILE *fp)
{
  	int i,n;
	char buffer[DATA],c;
	char *p;
	i=n=0;
	memset(buffer,0,sizeof(buffer));	
	n=fread(buffer,1,DATA,fp);
	if(n<DATA-1)
	{
		count++;
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
	
 return p;
}
void initWindow(int noofpackets,int hasACK[])
{
	int i;	
	for(i=0;i<noofpackets;i++)
	{
		hasACK[i]=0;	
	}
}
void sendFrame(int socketdescriptor,struct packet *frame,struct sockaddr_in server,int len)
{
	int a;
	struct packet singleframe;
	singleframe.seqno=frame->seqno;
	singleframe.packettype=frame->packettype;
	singleframe.datasize=frame->datasize;
	strcpy(singleframe.data,frame->data);
	a=sendto(socketdescriptor,(const void *)&singleframe,sizeof(struct packet),0,(struct sockaddr *)&server,len);
	printf("seq no:%d:%d\n",singleframe.seqno,singleframe.datasize);
	gettimeofday(&lastTime, NULL);				
}
void communicate(int socketdescriptor,struct sockaddr_in server,int len)
{
	unsigned long int filesize=0;
	long int packetqueuesize=0; 	
	int n,a,i,j,flag,nextframe,result,maxfd,k,delta;
	static int framecounter=0;
	struct packet init,frame,ackframe,singleframe;
	char filename[NAMESIZE];
	char *packetdata;
	int *hasACK;	
	fd_set rdset,wrset;
	LFS=LAR=next=NULL;	
	nextframe=noofpackets=n=i=j=a=k=0;
	framecounter=1;
	printf("Enter the file name to be sent:");
	scanf("%s",filename);
	fp=fopen(filename,"rb");
	if(fp>0)
	{		
		filesize=getFileSize(fp);
		noofpackets=getnoofPackets(filesize);
        	hasACK=(int *)malloc(sizeof(int)*noofpackets);
        	init=createPacket(1,filename);											//initial package
		a=sendto(socketdescriptor,(const void *)&init,sizeof(struct packet),0,(struct sockaddr *)&server,len);
		initWindow(noofpackets,hasACK);
		packetqueue=(struct packet *) malloc(sizeof(struct packet)*noofpackets);
		FD_ZERO(&rdset);
		FD_ZERO(&wrset);	
		i=0;
		while(i<noofpackets)
		{
			packetdata=readData(fp);
			frame=createPacket(2,packetdata);
			packetqueue[i]=frame;
			packetqueuesize+=packetqueue[i++].datasize;
		}
		i=0;
		LAR=LFS=next=latestack=&packetqueue[0];
		while(i<noofpackets && LFS->seqno-LAR->seqno<=WINDOWSIZE)
		{
			if(LFS->seqno-LAR->seqno<=WINDOWSIZE)
			{	
				
				sendFrame(socketdescriptor,LFS,server,len);		
				*(LFS++);
			}
			else
			{
				break;
			}
			FD_SET(socketdescriptor,&rdset);
			timeout.tv_sec = TIMEOUT / 1000;
			timeout.tv_usec = (TIMEOUT % 1000) * 1000;
		        result = select(maxfd+1, &rdset, &wrset, NULL, &timeout);	
			if(result>0)
			{
				a=recvfrom(socketdescriptor,(void *)&ackframe,sizeof(struct packet),0,(struct sockaddr *)&server,&len);
				for(k=LAR->seqno;k<=LFS->seqno;k++)
				{
					if(k==ackframe.seqno)
					{
						printf("ACK for Frame %d recived.\n",ackframe.seqno);
						latestack=&packetqueue[k-1];
						hasACK[latestack->seqno]=1;
						break;			
					}
				}
				if(latestack->seqno==LAR->seqno)
				{
					j=latestack->seqno;
					while(hasACK[j]==1)
					{					
						*(LAR++);
						j++;
					}
				}
				else if(latestack->seqno>LAR->seqno && latestack->seqno<=LFS->seqno)
				{
					hasACK[latestack->seqno]=1;
				}
			        
			}	
			else if(result==0)
			{
				gettimeofday(&now, NULL);				
    				delta = (now.tv_sec - lastTime.tv_sec) * 1000; /* in miliseconds */
		                delta += (now.tv_usec - lastTime.tv_usec) / 1000;
        			if(delta>TIMEOUT)
				{
					for(i=LAR->seqno;i<LFS->seqno;i++)
					{
						if(hasACK[i-1]==0)
						{
							printf("ACK for %d not received.\n",i);
							next=&packetqueue[i-1];
							printf("Resending Frame:%d\n",next->seqno);
							sendFrame(socketdescriptor,next,server,len);												
							next=LFS;
							*(next++);//problem here.	
						}
					}
				}
			}
			i++;		
		}
			frame=createPacket(3,"END");
			printf("Total transferred data: %d\n",packetqueuesize);
			a=sendto(socketdescriptor,(const void *)&frame,sizeof(struct packet),0,(struct sockaddr *)&server,len);	
			
	 }		
	 else
		printf("Error while reading the file!!\n");
		
}

int main(int argc,char *argv[])
{
	int socketdescriptor,length;
	struct sockaddr_in servaddr;
	
	memset(&servaddr,0,sizeof(servaddr));	
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
	if(argc>1)
	{
		servaddr.sin_addr.s_addr=inet_addr(argv[1]);
	}
	else
	{
	 	servaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	}
	length=sizeof(struct sockaddr_in);
	communicate(socketdescriptor,servaddr,length);											//function for sending and receiving data from server.
	close(socketdescriptor);											//closing the socket after the communcation completes.
} 

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netdb.h>
#include<unistd.h>
#include<errno.h>
#include<sys/select.h>
#include<sys/time.h>

#define PORT "8888"

struct info{
	int client_no;
	char name[30];
};

struct message{
	//int fun;
	char data[1024];
	struct info src;
	struct info dest;	
};

void *get_in_addr(struct sockaddr *sa){
	if(sa->sa_family==AF_INET){
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void compute_crc(char *data){
	printf("Enter the polynomial generator\n");
	char pg[100],rem[100],msg[100];
	read(0,pg,sizeof(pg));
	int size_pg,size_msg,size_rem,i,j,k;
	size_pg=strlen(pg);
	size_msg=strlen(data);
	data[size_msg-1]='\0';
	pg[size_pg-1]='\0';
	//printf("last %caa\n",data[size_msg-1]);
	
	strcpy(msg,data);
	//printf("msg is :%s\n",msg);
	strcpy(rem,data);
	for(i=1;i<size_pg-1;i++){ strcat(rem,"0");}
	//printf("rem is :%s\n",rem);
	size_rem=strlen(rem);
	for(i=0;i<=(size_rem - size_pg);i++){
		if(rem[i]=='1'){
			k=0;
			for(j=i;j<i+size_pg-1;j++){
				rem[j]=(rem[j]==pg[k++])?'0':'1';
			}
		}
		//printf("pg is :%s\n",pg);
		//printf("rem after %d itr is :%s\n",i,rem);
	}
	char rem1[size_pg];
	j=0;
	for(i=size_msg-1;i<size_rem;i++){
		rem1[j++]=rem[i];	
	}
	strcat(msg,rem1);
	printf("final rem is :%s\n",rem1);
	printf("final msg is: %s\n",msg);
	strcpy(data,msg);	
	
}

int main(int argc,char *argv[]){


/* *********************** Normal work to prepare socket for server **************************************************/
	struct addrinfo hints, *res,*p;
	socklen_t client_addrlen;
	int status,yes=1,sockfd;
	char name[30];
	if(argc!=2){
		fprintf(stderr,"Give ip address\n");
		exit(0);
	}
	memset(&hints,0,sizeof(hints));
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;	
	hints.ai_flags=AI_PASSIVE;

	if((status=getaddrinfo(argv[1],PORT,&hints,&res))==-1){
		fprintf(stderr,"getaddrinfo error %s\n",gai_strerror(status));
		return 1;
	}
	
	for(p=res;p!=NULL;p=p->ai_next){
		if((sockfd=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1){
			perror("client:socket");
			continue;
		}
		if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1){
			perror("client:setsockopt");
			exit(0);
		}
		if(connect(sockfd,p->ai_addr,p->ai_addrlen)==-1){
			perror("client:bind");
			continue;
		}
		break;
	}

	if(p==NULL){
		fprintf(stderr,"client can't connect to server\n");
		exit(0);
	}
	
/********************************************************************************************************/
	struct message *msg;
	char data[256],data1[256];
	memset(data,0,sizeof(data));
	msg=(struct message*)malloc(sizeof(struct message));
	printf("Enter some data\n");
	read(0,data,sizeof(data));
	printf("prepared message :%s\n",data);
	compute_crc(data);
	send(sockfd,data,sizeof(data),0);
	printf("message sent\n");
	
	
return 0;
}

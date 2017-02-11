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
#include<signal.h>

#define PORT "8888"

struct info{
	int client_no;
	char name[30];
};

struct getlist{
	int size;
	int cno[20];
	char name1[20][30];
};	

struct message{
	int fun;
	char data[256];
	struct info src;
	struct info dest;
	struct getlist list;
};

void *get_in_addr(struct sockaddr *sa){
	if(sa->sa_family==AF_INET){
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
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
	struct message *msg,*msg1;
	struct getlist *list;
	char name2[30],buf[256];
	int client_no,choice;
	msg=(struct message*)malloc(sizeof(struct message));
	printf("Enter your name.\n");
	read(0,name2,sizeof(name2));
	int zzz=strlen(name2),z;
	name2[zzz]='\0';
	strcpy(msg->src.name,name2);
	strcpy(msg->dest.name,"server");
	strcpy(msg->data,"Hello world");
	msg->fun=0;
	read(sockfd,&client_no,sizeof(client_no));
	msg->src.client_no=client_no;
	msg->dest.client_no=0;
	printf("server:your client no is %d\n",client_no);
	write(sockfd,(char*)msg,sizeof(struct message));
	free(msg);
	printf("waiting to receive data from sender\n");
	while(1){
		msg=(struct message*)malloc(sizeof(struct message));
		msg1=(struct message*)malloc(sizeof(struct message));
		if(read(sockfd,(char*)msg,sizeof(struct message))>0){
			printf("sender: %s\n",msg->data);
			printf("do you want to send ACK\n0:No\n1:yes\n");
			scanf("%d",&z);
			if(z==1){memset(msg1->data,0,sizeof(msg->data));
				read(0,buf,sizeof(buf));
				strcpy(msg1->data,buf);
				msg1->fun=1;
				msg1->dest.client_no=msg->src.client_no;
				msg1->src.client_no=msg->dest.client_no;
				strcpy(msg1->src.name,name2);
				write(sockfd,(char*)msg1,sizeof(struct message));
			}
			free(msg);
			free(msg1);
		}else{
			printf("sender disconnected\n");
			close(sockfd);exit(0);
		}
	}
	close(sockfd);
	
return 0;
}

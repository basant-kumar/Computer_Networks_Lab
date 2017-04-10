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
#include<string>

using namespace std;


#define PORT "8888"

struct info{
	int client_no;
	char name[30];
	char MAC[4];
};

struct getlist{
	int size;
	int cno[20];
	char name1[20][30];
	char MAC1[20][4];
	//string name1[20];
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
	struct message *msg;
	struct getlist *list;
	char name2[30],buf[256],buf1[30],mac[4];
	int client_no,choice;
	msg=(struct message*)malloc(sizeof(struct message));
	printf("Enter your name.\n");
	scanf("%s",name2);
	//int zzz=strlen(name2);//printf("size of string is %d\n",zzz);
	//name2[zzz]='\0';
	printf("Enter your MAC address\n");
	scanf("%s",mac);
	strncpy(msg->src.name,name2,strlen(name2));
	strncpy(msg->src.MAC,mac,strlen(mac));
	strcpy(msg->dest.name,"server");
	strcpy(msg->data,"Hello world");
	msg->fun=0;
	read(sockfd,&client_no,sizeof(client_no));
	msg->src.client_no=client_no;
	msg->dest.client_no=0;
	printf("server:your client no is %d\n",client_no);
	write(sockfd,(char*)msg,sizeof(struct message));
	free(msg);
	int pid=fork();
	while(1){
		
		if(pid == 0){
			printf("***************************************************\n");
			printf("1:online clients\n2:private message\n3:more options to come\n4:quit\n");
			printf("***************************************************\n");
			read(0,buf1,sizeof(buf1));
			choice=atoi(buf1);
			if(choice==1){//online clients
				msg=(struct message*)malloc(sizeof(struct message));
				msg->fun=3;//get list
				msg->src.client_no=client_no;
				msg->dest.client_no=0;
				write(sockfd,(char*)msg,sizeof(struct message));
				free(msg);sleep(1);
			}else if(choice==2){//private message
				printf("Enter exit if you want to quit private message chat\n");
				printf("Whom you want to chat, enter MAC Address....\n");
				char cc[4];
				scanf("%s",cc);
				printf("client no is %s\n",cc);
				printf("start chatting...\n");
				while(1){
					memset(buf,0,sizeof(buf));
					msg=(struct message*)malloc(sizeof(struct message));
					read(0,buf,sizeof(buf));
					int sd=strlen(buf);
					buf[sd-1]='\0';
					msg->fun=4;
					//msg->dest.client_no=cc;
					strncpy(msg->dest.MAC,cc,strlen(cc));
					strncpy(msg->src.MAC,mac,strlen(mac));
					msg->src.client_no=client_no;
					strcpy(msg->src.name,name2);
					if(strcmp(buf,"exit")==0){
						//char ccc[10];
						//sprintf(ccc,"%d",cc);
						strcpy(msg->data,"Chat ended with ");strcat(msg->data,name2);strcat(msg->data," (");
						strcat(msg->data,cc);strcat(msg->data,")");
						printf("connection ended with receiver\n");
						write(sockfd,(char*)msg,sizeof(struct message));						
						break;
					}
					strcpy(msg->data,buf);
					write(sockfd,(char*)msg,sizeof(struct message));			
					free(msg);
				}
			}else if(choice==4){
				msg=(struct message*)malloc(sizeof(struct message));
				strcpy(msg->data,"bye");
				write(sockfd,(char*)msg,sizeof(struct message));
			}else{
				printf("Please enter right choice\n");
			}
		}
		else{
			msg=(struct message*)calloc(sizeof(struct message),1);
			if(read(sockfd,(char*)msg,sizeof(struct message))>0){
				int i=0,fun=msg->fun;
				if(fun==3){
					int s1,size=msg->list.size; //printf("size is %d\n",size);
					if(size==0){
						printf("No other client online\n");
					}
					else{
						printf("Online clients......\n");
						while(i<size){
							//s1=strlen(msg->list.name1[i]);
							//msg->list.name1[i][s1-1]='\0';
							//char temp[30]; strncmp(temp,msg->list.name1[i],s1);
							char tt=msg->list.MAC1[i][4];
							msg->list.MAC1[i][4]='\0';

							printf("%s:(%d) with MAC: %s\n",msg->list.name1[i],msg->list.cno[i],msg->list.MAC1[i]);
							msg->list.MAC1[i][4]=tt;
							i++;
						}
					}
				}
				else{  
					int sp=strlen(msg->src.name);
					msg->src.name[sp]='\0';
					printf("%s(%d): %s\n",msg->src.name,msg->src.client_no,msg->data);
				}
			}else{
				printf("server disconnect...\n");
				kill(pid,SIGKILL);
				break;
			}
			free(msg);
		}
		
	}
	close(sockfd);
	
return 0;
}

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
#include<math.h>

#define PORT "8888"

void *get_in_addr(struct sockaddr *sa){
	if(sa->sa_family==AF_INET){
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int compute_haming_code(char *buf,int last){
	char data[256];
	memset(data,0,sizeof(data));
	int size_data,size_hm_code,i,j,k,s,idx,count,yes;
	size_data=strlen(buf);
	j=0;
	for(i=size_data-1;i>=0;i--){
		data[j++]=buf[i];
	}
	printf("data1 is %s\n",data);
	printf("do you want to currupt data\n0:No\n1:Yes\n");
	scanf("%d",&yes);
	if(yes==1){
		int r=rand() % size_data;
		if(data[r-1]='1'){
			data[r-1]='0';
		}else {data[r-1]='1';}
		printf("currupted data position is %d\n",r);
	}
	printf("data is %s\n",data);
	int r[last+2];
	memset(r,0,sizeof(r));
	
	for(i=0;i<=(last);i++){
		count=1;//printf("in\n");
		for(j=pow(2,i);j<=size_data;j++){//printf("inside j\n");
			if(count<=pow(2,i)){//printf("inside count at %d\n",j);
				r[i]=(r[i]==((data[j-1])-'0'))?0:1;//printf("%d and %c\n",r[i],data[j-1]);
				count++;
			}else{
				j=j+pow(2,i)-1;
				count=1;
			}
		}
	}
	int pos=0;
	for(i=0;i<=last;i++){
		printf("%d ",r[i]);
		if(r[i]!=0){
			pos+=pow(2,i);
		}	
	}printf("\n");
	return pos;	
	
}


int main(int argc,char *argv[]){


/* *********************** Normal work to prepare socket for server **************************************************/
	struct addrinfo hints,*res,*p;
	struct sockaddr_storage client_addr;
	socklen_t client_addrlen;
	int status,yes=1,sockfd,clientfd,n=1;
	char ip[INET6_ADDRSTRLEN];
	char buf[256];

	memset(&hints,0,sizeof(hints));
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;	
	hints.ai_flags=AI_PASSIVE;

	if((status=getaddrinfo(NULL,PORT,&hints,&res))==-1){
		fprintf(stderr,"getaddrinfo error %s\n",gai_strerror(status));
		return 1;
	}
	
	for(p=res;p!=NULL;p=p->ai_next){
		if((sockfd=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1){
			perror("server:socket");
			continue;
		}
		if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1){
			perror("server:setsockopt");
			exit(0);
		}
		if(bind(sockfd,p->ai_addr,p->ai_addrlen)==-1){
			perror("server:bind");
			continue;
		}
		break;
	}

	if(p==NULL){
		fprintf(stderr,"server can't bind\n");
		exit(0);
	}
	freeaddrinfo(res);
	if(listen(sockfd,5)==-1){
		perror("server:listen");
		exit(0);
	}
/********************************************************************************************************/
	
	printf("server is online\n");
	while(1){
		client_addrlen=sizeof(client_addr);
		if((clientfd=accept(sockfd,(struct sockaddr *)&client_addr,&client_addrlen))==-1){
			perror("server:accept");
			continue;
		}
		int s1;
		recv(clientfd,&s1,sizeof(int),0);
		recv(clientfd,buf,sizeof(buf),0);
		printf("received data is %s and size is %d\n",buf,s1);
		int flag=compute_haming_code(buf,s1);
		send(clientfd,&flag,sizeof(int),0);
	}
	
return 0;
}

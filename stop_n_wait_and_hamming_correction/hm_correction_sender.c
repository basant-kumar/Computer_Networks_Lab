#include<stdio.h>
#include<stdlib.h>
#include<math.h>
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

void *get_in_addr(struct sockaddr *sa){
	if(sa->sa_family==AF_INET){
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int compute_haming_code(char *buf){
	char data[256];
	memset(data,0,sizeof(data));
	int size_data,size_hm_code,i,j,k,s,last,idx,count;
	size_data=strlen(buf);
	j=0;
	for(i=size_data-1;i>=0;i--){
		data[j++]=buf[i];
	}
	
	s=size_data;
	i=0;
	char new_data[256];
	memset(new_data,0,sizeof(new_data));
	while(i<=s){
		if(pow(2,i)<=s){
			s++;
			idx=(pow(2,i)-1);
			new_data[idx]='2';
			last=i;
		}
		else{ break; }
		i++;
	}
	j=0;
	for(i=0;i<s;i++){
		if(new_data[i]!='2'){
			new_data[i]=data[j++];
		}
	}
	int r[last+2];
	memset(r,0,sizeof(r));
	
	for(i=0;i<=(last);i++){
		count=1;
		for(j=pow(2,i);j<=s;j++){
			if(count<=pow(2,i)){//printf("inside count at %d\n",j);
				if(new_data[j-1]!='2'){//printf("inside new_data at %d data is %c\n",j,new_data[j-1]);
					r[i]=(r[i]==(new_data[j-1])-'0')?0:1;
				}
				count++;
			}else{
				j=j+pow(2,i)-1;
				count=1;
			}
		}
	}
	for(i=0;i<=last;i++){
		printf("%d ",r[i]);
		idx=(pow(2,i)-1);
		new_data[idx]=r[i]+'0';
	}printf("\n");
	size_data=strlen(new_data);
	j=0;
	for(i=size_data-1;i>=0;i--){
		data[j++]=new_data[i];
	}
	strcpy(buf,data);
	return last+1;
	
	
}

int main(int argc,char *argv[]){


/* *********************** Normal work to prepare socket for server **************************************************/
	struct addrinfo hints,*res,*p;
	struct sockaddr_storage client_addr;
	socklen_t client_addrlen;
	int status,yes=1,sockfd,clientfd,n=1;
	char ip[INET6_ADDRSTRLEN];
	

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
			perror("server:socket");
			continue;
		}
		if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1){
			perror("server:setsockopt");
			exit(0);
		}
		if(connect(sockfd,p->ai_addr,p->ai_addrlen)==-1){
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
/********************************************************************************************************/
	printf("Enter the message\n");
	char buf[256];
	int pos,s1;
	scanf("%s",buf);
	s1=compute_haming_code(buf);
	printf("data is %s\n",buf);
	send(sockfd,&s1,sizeof(int),0);sleep(1);
	send(sockfd,buf,sizeof(buf),0);
	recv(sockfd,&pos,sizeof(int),0);
	printf("error position is :%d\n",pos);
	
	
return 0;
}

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<errno.h>
#include<netdb.h>

#define PORT "8888"

int main(int argc,char *argv[]){
	int sockfd,clientfd,status;
	struct addrinfo hints,*res,*p;
	struct sockaddr_storage client_addr;
	socklen_t client_addrlen;
	char buf[1024];
	int numbytes;
	int yes=1;        	
	if(argc!=2){
		printf("need ip address\n");
		exit(0);
	}
	memset(&hints,0,sizeof hints);
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_flags=AI_PASSIVE;
	if((status=getaddrinfo(argv[1],PORT,&hints,&res))==-1){
		fprintf(stderr,"getaddrinfo error...:%s\n",gai_strerror(status));
		return 1;
	}
	for(p=res;p!=NULL;p=p->ai_next){
		sockfd=socket(p->ai_family,p->ai_socktype,p->ai_protocol);
		if(sockfd<0){
			perror("client:socket");
			continue;
		}
		// to resolve "address already in use" error message
		setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		if(connect(sockfd,p->ai_addr,p->ai_addrlen)<0){
			close(sockfd);			
			//perror("server:bind");
			continue;
		}
		break;
	}
	if(p==NULL){
		fprintf(stderr,"client:failed to connect\n");
		exit(2);
	}
	freeaddrinfo(res);
	int n,b,c=0,sum=0,k,i,j;
	printf("Enter length of message\n");
	scanf("%d",&n);
	char arr[n+1],arr1[n+1],cc;
	memset(arr1,0,sizeof(arr1));
	printf("Enter the message\n");
	//for( i=0;i<n;i++){
		scanf("%s",arr);
		strcpy(arr1,arr);
	//}
	printf("msg is :%s\n",arr);
	printf("Enter the length of one block\n");
	scanf("%d",&b);
	k=2;
	int blah=(n/b)-1;
	char csum[b];
	printf("arr1 is :%s\n",arr1);
	while(blah--){
		c=0;	
		printf("k is :%d\n",k);
		j=((b*k)-1);
		printf("j is :%d\n",j);
		for( i=b-1;i>=0;i--){//printf("check1\n");
			int z1=arr1[i]-'0';
			int z2=arr1[j]-'0';
			printf("%d->%d\n",z1,z2);
			//if(i>=0 && j>=b){//printf("check2\n");
				sum=z1+z2;
				if((sum+c)==0){arr1[i]='0';c=0;}
				else if((sum+c)==1){arr1[i]='1';c=0;}
				else if((sum+c)==2){arr1[i]='0';c=1;}
				else if((sum+c)==3){arr1[i]='1';c=1;}
			//}
			j--;
		}
		k++;
	}
	printf("arr1 is :%s\n",arr1);
	for( i=0;i<b;i++){
		if(arr1[i]=='0'){arr1[i]='1';}
		else {arr1[i]='0';}
	}
	for(i=0;i<b;i++){csum[i]=arr1[i];}
	printf("arr1 is :%s\n",arr1);
	printf("checksum is :%s\n",csum);
	printf("n and b is: %d ::%d\n",n,b);
	printf("size of arr1 and arr is :%d::%d\n",strlen(arr1),strlen(arr));			
	char msg[n+b];
	for( i=0;i<n;i++){
		msg[i]=arr[i]; 
	}
	for(i=0;i<b;i++){
		msg[n+i]=csum[i];
	}
	printf("size of msg is :%d\n",strlen(msg));
	printf("message %s sending to server\n",msg);
	send(sockfd,&b,sizeof(int),0);
	sleep(1);
	//msg[n-1]=1;
	if(send(sockfd,msg,sizeof(msg),0)==-1){
		perror("client:send");
		exit(0);
	}
	int yes1=1	;
	recv(sockfd,&yes1,sizeof(int),0);
	if(yes1==1){ printf("server received correct data\n");}
	else {printf("server received currpted data\n");}

	close(sockfd);
	
	

return 0;
}












//#define _GNU_SOURCE
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

using namespace std;

int main(void)
{		char f[50];
		printf("Enter tr file name(e.g. reno.tr/tahoe.tr/new_reno.tr/sack.tr/vegas.tr)\n");
		scanf("%s",f);
    FILE * fp,*fp1;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
		remove("out");
    fp = fopen(f, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
		fp1 = fopen("out", "a+");
    if (fp1 == NULL)
        exit(EXIT_FAILURE);
	
		int time=0,r_cnt=0,d_cnt=0;
		float delay=0.0;
		int count=0,cnt=0;
		vector<float> plus;
		vector<float> minus;
	
    while ((read = getline(&line, &len, fp)) != -1) {
		if(line[0]=='d') d_cnt++;
		if(line[0]=='+') {
			char *buf1;
			buf1=strtok(line," ");
			buf1=strtok(NULL," ");//printf("time1 is %f\n",atof(buf1));
			//plus.push_back(atof(buf1));
			delay+=atof(buf1);
			continue;
		}
		if(line[0]=='-') {
			char *buf2;
			buf2=strtok(line," ");
			buf2=strtok(NULL," ");//printf("time2 is %f\n",atof(buf2));
			//minus.push_back(atof(buf2));
			delay-=atof(buf2);
			continue;
		}
    	if(line[0]=='r'){
			r_cnt++;
			char *buf;
			int c=0;
			buf=strtok(line," ");
			while(buf!=NULL){
				c++;
				if(c==2){
					if(atoi(buf)==(time+1)){
						char sec[10],val[50];
						char str[100];
						sprintf(sec,"%d",time);
						sprintf(val,"%f",8*((float)count/1000000));
						strcpy(str,sec); strcat(str," "); strcat(str,val); strcat(str," \n");
						int s=strlen(str); //printf("str is %s\n",str);
						fwrite(str,sizeof(char),s,fp1);
						count=0;time++;
					}
				}
				if(c==6){
					count+=atoi(buf);
					cnt+=atoi(buf);
				}
				buf=strtok(NULL," ");
			}
		}
		
    }
		fclose(fp1);
    fclose(fp);
    if (line)
        free(line);
		
	/*	vector<float>::iterator it1,it2;
		for( it1=plus.begin(), it2=minus.begin();it1!=plus.end(), it2!=minus.end();++it1,++it2){
				delay+=(*it2-*it1);
		}*/


	printf("thoughput is %f Mb\n",8*((float)cnt/1000000));
	printf("Avg. thoughput is %f Mb\n",(8*((float)cnt/1000000))/100);\
	printf("r_cnt and d_cnt are %d : %d\n",r_cnt,d_cnt);
	printf("PDR is %f\n",((float)d_cnt/(float)(d_cnt+r_cnt)));
	//printf("end to end delay is %f\n",(delay)/100);
	system("gnuplot > load 'plot' ");
  printf("Congestion window graph named as output.png for every file, so please check before running for new file\n");
    exit(EXIT_SUCCESS);
}

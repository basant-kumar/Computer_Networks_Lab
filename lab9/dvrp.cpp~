#include<iostream>


using namespace std;

//Vertex names
char ch[7]={'A','B','C','D','E','F','G'};
int final[50][50],nn;
int via[50][50];
int graph[50][50];

void printtable(int idx){
	cout<<"\nAfter Update for node "<<ch[idx]<<" :";
	int i,j,k;
    for (i=0;i<nn;i++)
    {
        cout<<"\n"<<ch[i]<<" Table";
        cout<<"\nNode\tDist\tVia";
        for(j=0;j<nn;j++)
        {	if(i>idx){
				cout<<"\n"<<ch[j]<<"\t"<<graph[i][j]<<"\t-1";
			}
			else{
		        cout<<"\n"<<ch[j]<<"\t"<<final[i][j]<<"\t";
		        if(i==via[i][j])
		            cout<<"-";
		        else
		            cout<<ch[via[i][j]];
			}
        }
    }
    cout<<endl;
}


 
int main()
{
    
    int i,j,k,t;
 
    cout<<"\nEnter Number of Nodes:";
    cin>>nn;
 
    
    for (i=0;i<nn;i++)
    {
        for(j=0;j<nn;j++)
        {
            graph[i][j]=-1;
        }
    }
 
   
 

    for (i=0;i<nn;i++)
    {
        for(j=0;j<nn;j++)
        {
            if(i==j)
            {
                graph[i][j]=0;
            }
            if(graph[i][j]==-1)
            {
                cout<<"\nEnter Distance between "<<ch[i]<<" - "<<ch[j]<<" : ";
                cin>>t;
                graph[i][j]=graph[j][i]=t;
            }
        }
    }
    
    for (i=0;i<nn;i++)
    {
        for(j=0;j<nn;j++)
        {
            via[i][j]=-1;
        }
    }
 
    cout<<"\nAfter Initialization";
    for (i=0;i<nn;i++)
    {
        cout<<"\n"<<ch[i]<<" Table";
        cout<<"\nNode\tDist\tVia";
        for(j=0;j<nn;j++)
        {
            cout<<"\n"<<ch[j]<<"\t"<<graph[i][j]<<"\t"<<via[i][j];
        }
    }
 
    //sharing table
    int sh[50][50][50];
    for(i=0;i<nn;i++)
    {
        for(j=0;j<nn;j++)
        {
            for (k=0;k<nn;k++)
            {
               
                if((graph[i][j]>-1)&&(graph[j][k]>-1))
                {
                    sh[i][j][k]=graph[j][k]+graph[i][j];
                }
                else
                {
                    sh[i][j][k]=-1;
                }
            }
        }
    }
 
    //displaying shared table 
   /* for(i=0;i<nn;i++)
    {
        cout<<"\n\n For "<<ch[i];
        for (j=0;j<nn;j++)
        {
            cout<<"\n From "<<ch[j];
            for(k=0;k<nn;k++)
            {
                cout<<"\n "<<ch[k]<<" "<<sh[i][j][k];
            }
        }
    }*/ 
     
    //Updating
    
    for(i=0;i<nn;i++)
    {	sleep(5);
        for(j=0;j<nn;j++)
        {
           
            final[i][j]=graph[i][j];
            via[i][j]=i;
             
          
            for(k=0;k<nn;k++)
            {
                 
                if((final[i][j]>sh[i][k][j]) || (final[i][j] == -1))
                {
                    if(sh[i][k][j]>-1)
                    {
                        final[i][j]=sh[i][k][j];
                        via[i][j]=k;
                    }
                }
            }
       
 
            if(final[i][j]==-1)
            {
                for(k=0;k<nn;k++)
                {
 
                    if((final[i][k]!=-1)&&(final[k][j]!=-1))
                    {
                        if((final[i][j]==-1) || ((final[i][j]!=-1) &&(final[i][j]>final[i][k]+final[k][j]))) 
                        {
                            if(final[i][k]+final[k][j]>-1)
                            {
                                final[i][j]=final[i][k]+final[k][j];
                                via[i][j]=k;
                            }
                        }
                    }
                     
                }
            }
        }
				printtable(i);
    }
 	
   	cout<<"completed"<<endl;	
   
    return 0;
}
 


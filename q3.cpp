#include<sys/types.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<unistd.h>
#include<time.h>
#include<iostream>
#include<pthread.h>
#include<bits/stdc++.h>
#include<sys/mman.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<climits>
#include<time.h>

using namespace std;


void selection_sort(int v[],int low,int high)
{
    int i=high-low+1;

    int j=low;
    int min=v[low];
    int min_index=low;
    i--;
    while(i--)
    {
      
        for(int k=j+1;k<=high;k++)
        {
            if(v[k]<min)
            {
                min=v[k];
                min_index=k;
            }

        }
        v[min_index]=v[j];
        v[j]=min;
       
        j++;
        min=v[j];
        min_index=j;
    }
    return;
}


void merge(int v[],int low,int mid,int high)
{
    int v1[mid-low+2],v2[high-mid+1];

    int i;
    int zz=0;
    for( i=low;i<=mid;i++)
    {
        v1[zz]=v[i];
        zz++;
    }

    v1[zz]=1e9;

    int j;
    int k=0;
    for(j=mid+1;j<=high;j++)
    {
        v2[k]=v[j];
        k++;
    }

    v2[k]=1e9;

    i=0,j=0;

    for(int k=low;k<=high;k++)
    {

        if(v1[i] >= v2[j])
            {
                v[k]=v2[j];
                j++;
            }
        else
        {
            v[k]=v1[i];
            i++;
        }
    }
    return ;
}


void mergeSort(int v[],int low,int high)
{


    
    if(low>=high)
        return;

    
    if(high-low+1 <5)
    {
        selection_sort(v,low,high);
        return;
    }

    pid_t left_child,right_child,wpid;
   
    int status,mid;

    mid=(low+high)/2;
 

    left_child=fork();

 
    if(left_child == 0)
    {
        mergeSort(v,low,mid);

        _exit(0);
    }
    else
    {
        right_child=fork();

        if(right_child == 0){
        mergeSort(v,mid+1,high);
        _exit(0);
        }
    }


    while((wpid = wait(&status))>0);
    merge(v,low,mid,high);
    return;
}


void input_values(int v[],int len)
{
    for(int i=0;i<len;i++)
    {
        cin>>v[i];
    }
    return;
}



int main()
{
    size_t size;
    int n;
    cin>>n;
    
    clock_t start,end;

    int *v;

    // int protection=PROT_READ | PROT_WRITE;

    // int visibility=MAP_SHARED | MAP_ANONYMOUS;

    size=n*sizeof(int);
    //v=(int*)mmap(NULL,size,protection,visibility,-1,0);

    int id;


    key_t key;


    key=ftok("q3.cpp",1);
    
    id=shmget(key,size,0666 | IPC_CREAT);

    if(id == -1)
    {
        printf("%s",strerror(errno));
        cout<<"Error";
        return 0;
    }
    
    v=(int*)shmat(id,NULL,0);

    input_values(v,n);

   
    cout<<endl;
    start=clock();

    mergeSort(v,0,n-1);

    for(int i=0;i<n;i++)
    {
        cout<<v[i]<<" ";
    }

    cout<<endl;
    int a=shmdt(v);

    int detach=shmctl(id,IPC_RMID,NULL);

    end=clock();

    double cpu_time_used=((double)(end-start))/CLOCKS_PER_SEC;

    cout<<cpu_time_used<<endl;

   
    return 0;
}
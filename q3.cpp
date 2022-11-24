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


using namespace std;


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
    

    int *v;

    int protection=PROT_READ | PROT_WRITE;

    int visibility=MAP_SHARED | MAP_ANONYMOUS;

    size=n*sizeof(int);
    v=(int*)mmap(NULL,size,protection,visibility,-1,0);

    input_values(v,n);


    cout<<endl;
    mergeSort(v,0,n-1);

    for(int i=0;i<n;i++)
    {
        cout<<v[i]<<" ";
    }

    cout<<endl;
    return 0;
}
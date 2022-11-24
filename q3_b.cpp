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

vector<int>v;
int n1;

void merge(int low,int mid,int high)
{
    int v1[mid-low+2],v2[high-mid+1];

    int i;
    int zz=0;
    for( i=low;i<=mid;i++)
    {
        v1[zz]=v[i];
        cout<<"v1[zz]"<<" "<<v1[zz]<<endl;
        zz++;
    }

    v1[zz]=1e9;

    int j;
    int k=0;
    for(j=mid+1;j<=high;j++)
    {
        v2[k]=v[j];
        cout<<"v2[k]"<<" "<<v2[k]<<endl;
        k++;
    }

    v2[k]=1e9;

    i=0,j=0;

    for(int k=low;k<=high;k++)
    {

        if(v1[i] >= v2[j])
            {
                v[k]=v2[j];
                cout<<"v[k]"<<" "<<v[k]<<endl;
                j++;
            }
        else
        {
            v[k]=v1[i];
            cout<<"v[k]"<<" "<<v[k]<<endl;
            i++;
        }
    }
    return ;
}


struct arg_struct{
    int low;
    int high;
};


void* mergesort(void* arguements)
{

    struct arg_struct args=*((struct arg_struct*)arguements);

    int low=args.low;
    int high=args.high;

    cout<<"low1"<<" "<<low<<endl;
    cout<<"high1"<<" "<<high<<endl;

    if(low>=high)
    {
        pthread_exit(NULL);
        return NULL;
    }


    int mid=(low+high)/2;
    cout<<"mid1"<<" "<<mid<<endl;

    pthread_t left;
    pthread_t right;

    struct arg_struct arg1;
    struct arg_struct arg2;

    arg1.high=mid;
    arg1.low=low;

    arg2.high=high;
    arg2.low=mid+1;


    pthread_create(&left,NULL,mergesort,(void*)&arg1);
    pthread_create(&right,NULL,mergesort,(void*)&arg2);

    pthread_join(left,NULL);
    pthread_join(right,NULL);

    merge(low,mid,high);

    pthread_exit(NULL);


}


void solve(int low,int high)
{

    int a=low;
    int b=high;

    cout<<"low"<<" "<<low<<endl;
    cout<<"high"<<" "<<high<<endl;

    int mid=(low+high)/2;

    cout<<"mid"<<" "<<mid<<endl;

    pthread_t left;
    pthread_t right;

    struct arg_struct arg1;
    struct arg_struct arg2;

    arg1.high=mid;
    arg1.low=low;

    arg2.high=high;
    arg2.low=mid+1;


    pthread_create(&left,NULL,mergesort,(void*)&arg1);
    pthread_create(&right,NULL,mergesort,(void*)&arg2);

    pthread_join(left,NULL);
    pthread_join(right,NULL);

    merge(low,mid,high);


    for(int i=0;i<n1;i++)
    {
        cout<<v[i]<<" ";
    }

    cout<<endl;
    pthread_exit(NULL);
}


int main()
{

    int n;
    cin>>n;

    // if(n<=5)
    // {

    // }
    n1=n;

    vector<int>v1;

    for(int i=0;i<n;i++)
    {
        int a;
        cin>>a;
        v1.push_back(a);
    }

    v=v1;
    solve(0,n-1);

    return 0;
}
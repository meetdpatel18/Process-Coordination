#include<iostream>
#include<time.h>
#include<bits/stdc++.h>

using namespace std;

void selection_sort(vector<float>&v,int low,int high)
{
    int i=high-low+1;

    int j=low;
    float min=v[low];
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

void merge(vector<float>&v,int low,int mid,int high)
{
    float v1[mid-low+2],v2[high-mid+1];

    int i;
    int zz=0;
    for( i=low;i<=mid;i++)
    {
        v1[zz]=v[i];
        zz++;
    }

    v1[zz]=FLT_MAX;

    int j;
    int k=0;
    for(j=mid+1;j<=high;j++)
    {
        v2[k]=v[j];
        k++;
    }

    v2[k]=FLT_MAX;

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
    return;
}


void mergesort(vector<float>&v,int low,int high)
{
    if(low>=high)
        return;

    if(high-low+1 <5)
    {
        selection_sort(v,low,high);
        return;
    }
    int mid=(low+high)/2;

    mergesort(v,low,mid);
    mergesort(v,mid+1,high);
    merge(v,low,mid,high);
    return;
}


void solve(vector<float>&v,int low,int high)
{
    if(low>=high)
        return;

    if(high-low+1 <5)
    {
        selection_sort(v,low,high);
        return;
    }
    int mid=(low+high)/2;

    mergesort(v,low,mid);
    mergesort(v,mid+1,high);
    merge(v,low,mid,high);
    return;
}


int main()
{
    int n;
    cin>>n;
    vector<float>v(n);

    clock_t start,end;


    for(int i=0;i<n;i++)
    {
        cin>>v[i];
    }

    start=clock();
     solve(v,0,n-1);


    // for(int i=0;i<n;i++)
    // {
    //     cout<<v[i]<<" ";
    // }

    // cout<<endl;
    
    for(int i=0;i<n-1;i++)
    {
        if(v[i] > v[i+1])
            cout<<"false"<<endl;
    }

    cout<<endl;
    end=clock();

    double cpu_time_used=((double)(end-start))/CLOCKS_PER_SEC;

    cout<<cpu_time_used<<endl;

    cout<<endl;

    return 0;
}
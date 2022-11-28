#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <time.h>
#include <iostream>
#include <pthread.h>
#include <bits/stdc++.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <climits>
#include <time.h>

using namespace std;

vector<float> v;
int n1;
clock_t start, e;

void selection_sort(int low, int high)
{
    int i = high - low + 1;

    int j = low;
    float min = v[low];
    int min_index = low;
    i--;
    while (i--)
    {

        for (int k = j + 1; k <= high; k++)
        {
            if (v[k] < min)
            {
                min = v[k];
                min_index = k;
            }
        }
        v[min_index] = v[j];
        v[j] = min;

        j++;
        min = v[j];
        min_index = j;
    }
    return;
}

void merge(int low, int mid, int high)
{
    float v1[mid - low + 1], v2[high - mid];

    int i;
    int zz = 0;
    for (i = low; i <= mid; i++)
    {
        v1[zz] = v[i];
        zz++;
    }

    // v1[zz]=FLT_MAX;

    int j;
    int k = 0;
    for (j = mid + 1; j <= high; j++)
    {
        v2[k] = v[j];
        k++;
    }

    // v2[k]=FLT_MAX;

    i = 0, j = 0;
    int kk = low;
    // for(int k=low;k<=high;k++)
    while ((i < mid - low + 1) && (j < high - mid))
    {
        // cout<<5<<endl;
        if (v1[i] >= v2[j])
        {
            v[kk] = v2[j];
            kk++;
            j++;
        }
        else
        {
            v[kk] = v1[i];
            kk++;
            i++;
        }
    }

    while (i < (mid - low + 1))
    {
        v[kk] = v1[i];
        kk++;
        i++;
    }

    while (j < (high - mid))
    {
        v[kk] = v2[j];
        kk++;
        j++;
    }
    return;
}

struct arg_struct
{
    int low;
    int high;
};

void *mergesort(void *arguements)
{

    struct arg_struct args = *((struct arg_struct *)arguements);

    int low = args.low;
    int high = args.high;

    // cout<<"low1"<<" "<<low<<endl;
    // cout<<"high1"<<" "<<high<<endl;

    if (low >= high)
    {
        pthread_exit(NULL);
        return NULL;
    }

    if (high - low + 1 < 5)
    {
        selection_sort(low, high);
        pthread_exit(NULL);
        return NULL;
    }

    int mid = (low + high) / 2;
    // cout<<"mid1"<<" "<<mid<<endl;

    pthread_t left;
    pthread_t right;

    struct arg_struct arg1;
    struct arg_struct arg2;

    arg1.high = mid;
    arg1.low = low;

    arg2.high = high;
    arg2.low = mid + 1;

    pthread_create(&left, NULL, mergesort, (void *)&arg1);
    pthread_create(&right, NULL, mergesort, (void *)&arg2);

    pthread_join(left, NULL);
    pthread_join(right, NULL);

    merge(low, mid, high);

    pthread_exit(NULL);
}

void solve(int low, int high)
{

    int a = low;
    int b = high;

    if (high - low + 1 < 5)
    {
        selection_sort(low, high);
        // e = clock();
        // double cpu_time_used = ((double)(e - start)) / CLOCKS_PER_SEC;
        // cout << cpu_time_used << endl;
        return;
    }

    // cout<<"low"<<" "<<low<<endl;
    // cout<<"high"<<" "<<high<<endl;

    int mid = (low + high) / 2;

    // cout<<"mid"<<" "<<mid<<endl;

    pthread_t left;
    pthread_t right;

    struct arg_struct arg1;
    struct arg_struct arg2;

    arg1.high = mid;
    arg1.low = low;

    arg2.high = high;
    arg2.low = mid + 1;

    pthread_create(&left, NULL, mergesort, (void *)&arg1);
    pthread_create(&right, NULL, mergesort, (void *)&arg2);

    pthread_join(left, NULL);
    pthread_join(right, NULL);

    merge(low, mid, high);

    // cout<<endl;
    // for (int i = 0; i < n1 - 1; i++)
    // {
    //     if (v[i] > v[i + 1])
    //         cout << "false";
    // }

    // cout << endl;

    // e = clock();

    // double cpu_time_used = ((double)(e - start)) / CLOCKS_PER_SEC;

    // cout << cpu_time_used << endl;
    // pthread_exit(NULL);
}

int main()
{

    int n;
    cin >> n;

    // if(n<=5)
    // {

    // }
    n1 = n;

    vector<float> v1;

    for (int i = 0; i < n; i++)
    {
        float a;
        cin >> a;
        v1.push_back(a);
    }

    v = v1;

    auto start = chrono::steady_clock::now();
    solve(0, n - 1);

    auto end = chrono::steady_clock::now();
    auto diff = end - start;
    cout << endl
         << chrono::duration<double, milli>(diff).count() << " ms" << endl;
    return 0;
}
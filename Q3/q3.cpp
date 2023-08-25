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

void selection_sort(float v[], int low, int high)
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

void merge(float v[], int low, int mid, int high)
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

void mergeSort(float v[], int low, int high)
{

    if (low >= high)
        return;

    if (high - low + 1 < 5)
    {
        selection_sort(v, low, high);
        return;
    }

    pid_t left_child, right_child, wpid;

    int status, mid;

    mid = (low + high) / 2;

    left_child = fork();

    if (left_child == 0)
    {
        mergeSort(v, low, mid);

        _exit(0);
    }
    else
    {
        right_child = fork();

        if (right_child == 0)
        {
            mergeSort(v, mid + 1, high);
            _exit(0);
        }
    }

    while ((wpid = wait(&status)) > 0)
        ;
    merge(v, low, mid, high);
    return;
}

void input_values(float v[], int len)
{
    for (int i = 0; i < len; i++)
    {
        cin >> v[i];
    }
    return;
}

int main()
{
    size_t size;
    int n;
    cin >> n;

    float *v;

    // int protection=PROT_READ | PROT_WRITE;

    // int visibility=MAP_SHARED | MAP_ANONYMOUS;

    size = n * sizeof(float);
    // v=(int*)mmap(NULL,size,protection,visibility,-1,0);

    int id;

    key_t key;

    key = ftok("q3.cpp", 1);

    id = shmget(key, size, 0666 | IPC_CREAT);

    if (id == -1)
    {
        printf("%s", strerror(errno));
        cout << "Error";
        return 0;
    }

    v = (float *)shmat(id, NULL, 0);

    input_values(v, n);

    cout << endl;

    auto start = chrono::steady_clock::now();

    mergeSort(v, 0, n - 1);

    auto end = chrono::steady_clock::now();
    auto diff = end - start;
    cout << endl
         << chrono::duration<double, milli>(diff).count() << " ms" << endl;

    // for(int i=0;i<n;i++)
    // {
    //     cout<<v[i]<<" ";
    // }

    // cout<<endl;

    for (int i = 0; i < n - 1; i++)
    {
        if (v[i] > v[i + 1])
            cout << "false";
    }

    // cout << endl;
    int a = shmdt(v);

    int detach = shmctl(id, IPC_RMID, NULL);

    // end = clock();

    // double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    // cout << cpu_time_used << endl;

    return 0;
}
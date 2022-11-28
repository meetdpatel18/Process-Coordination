#include <iostream>
#include <time.h>
#include <bits/stdc++.h>

using namespace std;

void selection_sort(vector<float> &v, int low, int high)
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

void merge(vector<float> &v, int low, int mid, int high)
{
    // cout<<68768<<endl;
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

void mergesort(vector<float> &v, int low, int high)
{
    if (low >= high)
        return;

    if (high - low + 1 < 5)
    {
        selection_sort(v, low, high);
        return;
    }
    int mid = (low + high) / 2;

    mergesort(v, low, mid);
    mergesort(v, mid + 1, high);
    merge(v, low, mid, high);
    return;
}

void solve(vector<float> &v, int low, int high)
{
    if (low >= high)
        return;

    if (high - low + 1 < 5)
    {
        selection_sort(v, low, high);
        return;
    }
    int mid = (low + high) / 2;

    mergesort(v, low, mid);
    mergesort(v, mid + 1, high);
    merge(v, low, mid, high);
    return;
}

int main()
{
    int n;
    cin >> n;
    vector<float> v(n);

    for (int i = 0; i < n; i++)
    {
        cin >> v[i];
    }

    auto start = chrono::steady_clock::now();
    solve(v, 0, n - 1);

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
            cout << "false" << endl;
    }

    return 0;
}
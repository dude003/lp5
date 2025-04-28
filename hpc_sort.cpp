// hpc_sorts.cpp
#include <iostream>
#include <algorithm>   // for std::copy
#include <omp.h>

using namespace std;

// ----- Bubble Sort -----
void sequentialBubbleSort(int *a, int n) {
    bool swapped;
    for (int i = 0; i < n; i++) {
        swapped = false;
        for (int j = 0; j < n - 1; j++) {
            if (a[j] > a[j + 1]) {
                swap(a[j], a[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}

void parallelBubbleSort(int *a, int n) {
    int swapped;
    for (int i = 0; i < n; i++) {
        swapped = 0;
        int first = i % 2;
        #pragma omp parallel for shared(a, first) reduction(|:swapped)
        for (int j = first; j < n - 1; j += 2) {
            if (a[j] > a[j + 1]) {
                swap(a[j], a[j + 1]);
                swapped = 1;
            }
        }
        if (!swapped) break;
    }
}

// ----- Merge Sort -----
void merge(int a[], int i1, int j1, int i2, int j2) {
    int *temp = new int[j2 - i1 + 1];
    int i = i1, j = i2, k = 0;

    while (i <= j1 && j <= j2) {
        temp[k++] = (a[i] < a[j]) ? a[i++] : a[j++];
    }
    while (i <= j1) temp[k++] = a[i++];
    while (j <= j2) temp[k++] = a[j++];

    for (int x = 0; x < k; x++) {
        a[i1 + x] = temp[x];
    }
    delete[] temp;
}

void mergesort(int a[], int i, int j) {
    if (i >= j) return;
    int mid = (i + j) / 2;
    #pragma omp parallel sections
    {
        #pragma omp section
        mergesort(a, i, mid);
        #pragma omp section
        mergesort(a, mid + 1, j);
    }
    merge(a, i, mid, mid + 1, j);
}

int main() {
    int n;
    cout << "Enter number of elements: ";
    cin  >> n;

    int *orig       = new int[n];
    int *arr        = new int[n];
    int *arr_copy   = new int[n];

    cout << "Enter elements:\n";
    for (int i = 0; i < n; i++) {
        cin >> orig[i];
    }

    double t1, t2;

    // --- Bubble Sort ---
    copy(orig, orig + n, arr);
    copy(orig, orig + n, arr_copy);

    t1 = omp_get_wtime();
    sequentialBubbleSort(arr, n);
    t2 = omp_get_wtime();
    cout << "\nSequential Bubble Sort time: " << (t2 - t1) << " s\n";
    cout << "Sequential Bubble Sorted array: ";
    for (int i = 0; i < n; i++) cout << arr[i] << " ";
    cout << "\n";

    t1 = omp_get_wtime();
    parallelBubbleSort(arr_copy, n);
    t2 = omp_get_wtime();
    cout << "Parallel Bubble Sort time:   " << (t2 - t1) << " s\n";
    cout << "Parallel Bubble Sorted array: ";
    for (int i = 0; i < n; i++) cout << arr_copy[i] << " ";
    cout << "\n";

    // --- Merge Sort ---
    copy(orig, orig + n, arr);
    copy(orig, orig + n, arr_copy);

    t1 = omp_get_wtime();
    mergesort(arr, 0, n - 1);
    t2 = omp_get_wtime();
    cout << "\nSequential Merge Sort time:  " << (t2 - t1) << " s\n";
    cout << "Sequential Merge Sorted array: ";
    for (int i = 0; i < n; i++) cout << arr[i] << " ";
    cout << "\n";

    t1 = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single
        mergesort(arr_copy, 0, n - 1);
    }
    t2 = omp_get_wtime();
    cout << "Parallel Merge Sort time:    " << (t2 - t1) << " s\n";
    cout << "Parallel Merge Sorted array: ";
    for (int i = 0; i < n; i++) cout << arr_copy[i] << " ";
    cout << "\n";

    delete[] orig;
    delete[] arr;
    delete[] arr_copy;
    return 0;
}

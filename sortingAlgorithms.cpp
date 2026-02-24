#include "sortingAlgorithms.h"

void SortingAlgorithms::bubbleSort(std::vector<int> &arr) {
    int n = arr.size();
    for (int i = 0; i < n-1; i++)
        for (int j = 0; j < n-i-1; j++)
            if (arr[j] > arr[j+1])
                std::swap(arr[j], arr[j+1]);
}

void SortingAlgorithms::selectionSort(std::vector<int> &arr) {
    int n = arr.size();
    for (int i = 0; i < n-1; i++) {
        int min_idx = i;
        for (int j = i+1; j < n; j++)
            if (arr[j] < arr[min_idx])
                min_idx = j;
        std::swap(arr[i], arr[min_idx]);
    }
}
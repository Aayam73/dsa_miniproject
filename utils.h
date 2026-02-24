#pragma once
#include <vector>
#include <random>

std::vector<int> generateRandomArray(int size, int minVal, int maxVal) {
    std::vector<int> arr(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(minVal, maxVal);

    for (int &val : arr)
        val = dis(gen);

    return arr;
}
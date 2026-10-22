#ifndef MERGE_SORT_H
#define MERGE_SORT_H

#include <vector>
#include <cstddef>
#include <functional>

/**
 * @brief Custom Merge Sort implementation.
 * 
 * Concept: Divide-and-Conquer stable sorting algorithm.
 * Applications:
 *  1. Sorting word frequencies for Document Statistics.
 *  2. Ranking autocomplete suggestions alphabetically or by frequency.
 * 
 * Time Complexities:
 *  - Best Case: O(n log n)
 *  - Average Case: O(n log n)
 *  - Worst Case: O(n log n)
 *  - Space Complexity: O(n) auxiliary space for merge buffers.
 */
class MergeSort {
private:
    template <typename T, typename Compare>
    static void merge(std::vector<T>& arr, size_t left, size_t mid, size_t right, Compare comp) {
        size_t n1 = mid - left + 1;
        size_t n2 = right - mid;

        std::vector<T> leftArr(n1);
        std::vector<T> rightArr(n2);

        for (size_t i = 0; i < n1; ++i) {
            leftArr[i] = arr[left + i];
        }
        for (size_t j = 0; j < n2; ++j) {
            rightArr[j] = arr[mid + 1 + j];
        }

        size_t i = 0;
        size_t j = 0;
        size_t k = left;

        while (i < n1 && j < n2) {
            if (comp(leftArr[i], rightArr[j])) {
                arr[k++] = leftArr[i++];
            } else {
                arr[k++] = rightArr[j++];
            }
        }

        while (i < n1) {
            arr[k++] = leftArr[i++];
        }

        while (j < n2) {
            arr[k++] = rightArr[j++];
        }
    }

    template <typename T, typename Compare>
    static void sortInternal(std::vector<T>& arr, size_t left, size_t right, Compare comp) {
        if (left < right) {
            size_t mid = left + (right - left) / 2;
            sortInternal(arr, left, mid, comp);
            sortInternal(arr, mid + 1, right, comp);
            merge(arr, left, mid, right, comp);
        }
    }

public:
    template <typename T, typename Compare = std::less<T>>
    static void sort(std::vector<T>& arr, Compare comp = Compare()) {
        if (arr.size() <= 1) return;
        sortInternal(arr, 0, arr.size() - 1, comp);
    }
};

#endif // MERGE_SORT_H

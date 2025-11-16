#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <fstream>

class ArrayGenerator {
private:
    std::vector<int> random_array;
    std::vector<int> reverse_sorted_array;
    std::vector<int> almost_sorted_array;

    const int MAX_SIZE = 100000;
    const int VALUE_RANGE_MIN = 0;
    const int VALUE_RANGE_MAX = 6000;

public:
    ArrayGenerator() {
        generateBaseArrays();
    }

private:
    void generateBaseArrays() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(VALUE_RANGE_MIN, VALUE_RANGE_MAX);

        random_array.resize(MAX_SIZE);
        for (int i = 0; i < MAX_SIZE; i++) {
            random_array[i] = dist(gen);
        }

        reverse_sorted_array = random_array;
        std::sort(reverse_sorted_array.rbegin(), reverse_sorted_array.rend());

        almost_sorted_array = reverse_sorted_array;
        std::sort(almost_sorted_array.begin(), almost_sorted_array.end());
        makeAlmostSorted(almost_sorted_array);
    }

    void makeAlmostSorted(std::vector<int>& arr) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> index_dist(0, MAX_SIZE - 1);

        int swap_count = MAX_SIZE / 100;
        for (int i = 0; i < swap_count; i++) {
            int idx1 = index_dist(gen);
            int idx2 = index_dist(gen);
            std::swap(arr[idx1], arr[idx2]);
        }
    }

public:
    std::vector<int> getRandomArray(int size) {
        return getSubArray(random_array, size);
    }

    std::vector<int> getReverseSortedArray(int size) {
        return getSubArray(reverse_sorted_array, size);
    }

    std::vector<int> getAlmostSortedArray(int size) {
        return getSubArray(almost_sorted_array, size);
    }

    std::vector<int> getTestSizes() {
        std::vector<int> sizes;
        for (int size = 500; size <= 100000; size += 100) {
            sizes.push_back(size);
        }
        return sizes;
    }

private:
    std::vector<int> getSubArray(const std::vector<int>& source, int size) {
        return std::vector<int>(source.begin(), source.begin() + size);
    }
};

class SortTester {
private:
    const int num_trials = 5;
    const int threshold = 15;

public:
    SortTester() = default;

    void mergeSort(std::vector<int>& arr, int left, int right) {
        if (left >= right) return;

        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }

    void mergeSort(std::vector<int>& arr) {
        if (!arr.empty()) {
            mergeSort(arr, 0, arr.size() - 1);
        }
    }

    // гибридная сортировка
    void insertionSort(std::vector<int>& arr, int left, int right) {
        for (int i = left + 1; i <= right; i++) {
            int key = arr[i];
            int j = i - 1;

            while (j >= left && arr[j] > key) {
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = key;
        }
    }

    void hybridMergeSort(std::vector<int>& arr, int left, int right) {
        if (left >= right) return;

        if (right - left + 1 <= threshold) {
            insertionSort(arr, left, right);
            return;
        }

        int mid = left + (right - left) / 2;
        hybridMergeSort(arr, left, mid);
        hybridMergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }

    void hybridMergeSort(std::vector<int>& arr) {
        if (!arr.empty()) {
            hybridMergeSort(arr, 0, arr.size() - 1);
        }
    }

private:
    // Слияние (одна для двух алгоритмов)
    void merge(std::vector<int>& arr, int left, int mid, int right) {
        std::vector<int> temp(right - left + 1);
        int i = left, j = mid + 1, k = 0;

        while (i <= mid && j <= right) {
            if (arr[i] <= arr[j]) {
                temp[k++] = arr[i++];
            } else {
                temp[k++] = arr[j++];
            }
        }

        while (i <= mid) temp[k++] = arr[i++];
        while (j <= right) temp[k++] = arr[j++];

        for (int p = 0; p < k; p++) {
            arr[left + p] = temp[p];
        }
    }

public:
    // считаем время исполнения
    long long measureTime(std::vector<int> arr, void (SortTester::*sortFunc)(std::vector<int>&)) {
        auto start = std::chrono::high_resolution_clock::now();
        (this->*sortFunc)(arr);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        return std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    }

    // вычисляем среднее время выполнения
    double averageTime(std::vector<int> arr, void (SortTester::*sortFunc)(std::vector<int>&)) {
        long long total_time = 0;
        for (int i = 0; i < num_trials; i++) {
            std::vector<int> copy_arr = arr;
            total_time += measureTime(copy_arr, sortFunc);
        }

        return static_cast<double>(total_time) / num_trials;
    }

    std::vector<std::pair<int, double>> testStandardSorting(
        ArrayGenerator& generator,
        std::vector<int> (ArrayGenerator::*getArrayFunc)(int),
        const std::string& data_type) {

        std::vector<std::pair<int, double>> results;
        std::vector<int> sizes = generator.getTestSizes();

        std::cout << "Testing " << data_type << " merge sort\n";

        for (int size : sizes) {
            std::vector<int> arr = (generator.*getArrayFunc)(size);
            double avg_time = averageTime(arr, &SortTester::mergeSort);
            results.push_back({size, avg_time});

            if (size % 10000 == 0 || size == 500) {
                std::cout << "Size: " << size << ", Time: " << avg_time << " μs\n";
            }
        }

        return results;
    }

    std::vector<std::pair<int, double>> testHybridSorting(
        ArrayGenerator& generator,
        std::vector<int> (ArrayGenerator::*getArrayFunc)(int),
        const std::string& data_type) {

        std::vector<std::pair<int, double>> results;
        std::vector<int> sizes = generator.getTestSizes();

        std::cout << "Testing " << data_type << " hybrid sort (threshold=" << threshold << ")...\n";

        for (int size : sizes) {
            std::vector<int> arr = (generator.*getArrayFunc)(size);
            double avg_time = averageTime(arr, &SortTester::hybridMergeSort);
            results.push_back({size, avg_time});

            if (size % 10000 == 0 || size == 500) {
                std::cout << "Size: " << size << ", Time: " << avg_time << " мс\n";
            }
        }

        return results;
    }

    void saveResultsToCSV(const std::vector<std::pair<int, double>>& results,
                         const std::string& filename) {
        std::ofstream file(filename);
        file << "Size,TimeMicroseconds\n";
        for (const auto& result : results) {
            file << result.first << "," << result.second << "\n";
        }
        file.close();
        std::cout << "Results saved to " << filename << "\n";
    }
};

int main() {
    ArrayGenerator generator;
    SortTester tester;

    std::cout << "merge sort" << "\n";

    auto standard_random = tester.testStandardSorting(generator, &ArrayGenerator::getRandomArray, "random");
    tester.saveResultsToCSV(standard_random, "standard_merge_random.csv");

    auto standard_reverse = tester.testStandardSorting(generator, &ArrayGenerator::getReverseSortedArray, "reverse sorted");
    tester.saveResultsToCSV(standard_reverse, "standard_merge_reverse.csv");

    auto standard_almost = tester.testStandardSorting(generator, &ArrayGenerator::getAlmostSortedArray, "almost sorted");
    tester.saveResultsToCSV(standard_almost, "standard_merge_almost.csv");

    std::cout << "\n hybrid_sort \n";

    auto hybrid_random = tester.testHybridSorting(generator, &ArrayGenerator::getRandomArray, "random");
    tester.saveResultsToCSV(hybrid_random, "hybrid_merge_random.csv");

    auto hybrid_reverse = tester.testHybridSorting(generator, &ArrayGenerator::getReverseSortedArray, "reverse sorted");
    tester.saveResultsToCSV(hybrid_reverse, "hybrid_merge_reverse.csv");

    auto hybrid_almost = tester.testHybridSorting(generator, &ArrayGenerator::getAlmostSortedArray, "almost sorted");
    tester.saveResultsToCSV(hybrid_almost, "hybrid_merge_almost.csv");


    return 0;
}
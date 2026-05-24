#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <algorithm>
#include <functional>
#include <fstream>

using namespace std;

class StringGenerator {
private:
    const string alphabet =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789"
        "!@#%:;^&*()-.";

    mt19937 rng;

public:
    StringGenerator() {
        rng.seed(random_device{}());
    }

    string randomString(int minLen = 10, int maxLen = 200) {
        uniform_int_distribution<int> lenDist(minLen, maxLen);
        uniform_int_distribution<int> charDist(0, alphabet.size() - 1);

        int len = lenDist(rng);

        string s;
        s.reserve(len);

        for (int i = 0; i < len; i++) {
            s += alphabet[charDist(rng)];
        }

        return s;
    }

    vector<string> generateRandomArray(int n) {
        vector<string> arr(n);

        for (int i = 0; i < n; i++) {
            arr[i] = randomString();
        }

        return arr;
    }

    vector<string> generateReverseSortedArray(int n) {
        auto arr = generateRandomArray(n);

        sort(arr.begin(), arr.end());

        reverse(arr.begin(), arr.end());

        return arr;
    }

    vector<string> generateAlmostSortedArray(int n, int swaps = 10) {
        auto arr = generateRandomArray(n);

        sort(arr.begin(), arr.end());

        uniform_int_distribution<int> dist(0, n - 1);

        for (int i = 0; i < swaps; i++) {
            int a = dist(rng);
            int b = dist(rng);

            swap(arr[a], arr[b]);
        }

        return arr;
    }
};

class StringSortTester {
private:
    long long charComparisons = 0;

public:
    void resetComparisons() {
        charComparisons = 0;
    }

    long long getComparisons() const {
        return charComparisons;
    }

    bool lessString(const string& a, const string& b) {
        int n = min(a.size(), b.size());

        for (int i = 0; i < n; i++) {
            charComparisons++;

            if (a[i] < b[i]) return true;
            if (a[i] > b[i]) return false;
        }

        return a.size() < b.size();
    }

    int charAt(const string& s, int d) {
        if (d >= s.size()) return -1;
        return (unsigned char)s[d];
    }

    // QUICK SORT
    int partition(vector<string>& a, int l, int r) {
        string pivot = a[r];

        int i = l - 1;

        for (int j = l; j < r; j++) {
            if (lessString(a[j], pivot)) {
                i++;
                swap(a[i], a[j]);
            }
        }

        swap(a[i + 1], a[r]);

        return i + 1;
    }

    void quickSort(vector<string>& a, int l, int r) {
        if (l >= r) return;

        int p = partition(a, l, r);

        quickSort(a, l, p - 1);
        quickSort(a, p + 1, r);
    }

    void quickSort(vector<string>& a) {
        quickSort(a, 0, a.size() - 1);
    }

    // MERGE SORT
    void merge(vector<string>& a, int l, int m, int r) {
        vector<string> left(a.begin() + l, a.begin() + m + 1);
        vector<string> right(a.begin() + m + 1, a.begin() + r + 1);

        int i = 0;
        int j = 0;
        int k = l;

        while (i < left.size() && j < right.size()) {
            if (!lessString(right[j], left[i])) {
                a[k++] = left[i++];
            } else {
                a[k++] = right[j++];
            }
        }

        while (i < left.size()) {
            a[k++] = left[i++];
        }

        while (j < right.size()) {
            a[k++] = right[j++];
        }
    }

    void mergeSort(vector<string>& a, int l, int r) {
        if (l >= r) return;

        int m = (l + r) / 2;

        mergeSort(a, l, m);
        mergeSort(a, m + 1, r);

        merge(a, l, m, r);
    }

    void mergeSort(vector<string>& a) {
        mergeSort(a, 0, a.size() - 1);
    }

    // STRING QUICK SORT (3-WAY)
    void stringQuickSort(vector<string>& a, int l, int r, int d) {
        if (l >= r) return;

        int lt = l;
        int gt = r;

        int v = charAt(a[l], d);

        int i = l + 1;

        while (i <= gt) {
            int t = charAt(a[i], d);

            charComparisons++;

            if (t < v) {
                swap(a[lt++], a[i++]);
            }
            else if (t > v) {
                swap(a[i], a[gt--]);
            }
            else {
                i++;
            }
        }

        stringQuickSort(a, l, lt - 1, d);

        if (v >= 0)
            stringQuickSort(a, lt, gt, d + 1);

        stringQuickSort(a, gt + 1, r, d);
    }

    void stringQuickSort(vector<string>& a) {
        stringQuickSort(a, 0, a.size() - 1, 0);
    }

    // STRING MERGE SORT WITH LCP
    int lcpCompare(
        const string& a,
        const string& b,
        int start = 0
    ) {
        int i = start;

        while (i < a.size() && i < b.size()) {
            charComparisons++;

            if (a[i] < b[i]) return -1;
            if (a[i] > b[i]) return 1;

            i++;
        }

        if (a.size() < b.size()) return -1;
        if (a.size() > b.size()) return 1;

        return 0;
    }

    void stringMerge(
        vector<string>& a,
        int l,
        int m,
        int r
    ) {
        vector<string> left(a.begin() + l, a.begin() + m + 1);
        vector<string> right(a.begin() + m + 1, a.begin() + r + 1);

        int i = 0;
        int j = 0;
        int k = l;

        while (i < left.size() && j < right.size()) {

            int cmp = lcpCompare(left[i], right[j]);

            if (cmp <= 0) {
                a[k++] = left[i++];
            } else {
                a[k++] = right[j++];
            }
        }

        while (i < left.size()) {
            a[k++] = left[i++];
        }

        while (j < right.size()) {
            a[k++] = right[j++];
        }
    }

    void stringMergeSort(
        vector<string>& a,
        int l,
        int r
    ) {
        if (l >= r) return;

        int m = (l + r) / 2;

        stringMergeSort(a, l, m);
        stringMergeSort(a, m + 1, r);

        stringMerge(a, l, m, r);
    }

    void stringMergeSort(vector<string>& a) {
        stringMergeSort(a, 0, a.size() - 1);
    }

    // MSD RADIX SORT
    static const int R = 256;

    void msdRadixSort(
        vector<string>& a,
        int l,
        int r,
        int d,
        vector<string>& aux
    ) {
        if (l >= r) return;

        vector<int> count(R + 2, 0);

        for (int i = l; i <= r; i++) {
            count[charAt(a[i], d) + 2]++;
        }

        for (int i = 0; i < R + 1; i++) {
            count[i + 1] += count[i];
        }

        for (int i = l; i <= r; i++) {
            int c = charAt(a[i], d);

            aux[count[c + 1]++] = a[i];
        }

        for (int i = l; i <= r; i++) {
            a[i] = aux[i - l];
        }

        for (int i = 0; i < R; i++) {
            int start = l + count[i];
            int end = l + count[i + 1] - 1;

            if (start <= end) {
                msdRadixSort(a, start, end, d + 1, aux);
            }
        }
    }

    void msdRadixSort(vector<string>& a) {
        vector<string> aux(a.size());

        msdRadixSort(a, 0, a.size() - 1, 0, aux);
    }

    template<typename Func>
    void benchmark(
        const string& name,
        Func sorter,
        vector<string> arr
    ) {
        resetComparisons();

        auto start = chrono::high_resolution_clock::now();

        sorter(arr);

        auto end = chrono::high_resolution_clock::now();

        auto duration =
            chrono::duration_cast<chrono::microseconds>(
                end - start
            ).count();

        cout << "Algorithm: " << name << '\n';
        cout << "Time (microseconds): " << duration << '\n';
        cout << "Char comparisons: " << charComparisons << '\n';
        cout << "-----------------------------------\n";
    }
};


// int main() {
//
//     StringGenerator generator;
//
//     vector<string> arr =
//         generator.generateRandomArray(1000);
//
//     StringSortTester tester;
//
//     tester.benchmark(
//         "QuickSort",
//         [&](vector<string>& a) {
//             tester.quickSort(a);
//         },
//         arr
//     );
//
//     tester.benchmark(
//         "MergeSort",
//         [&](vector<string>& a) {
//             tester.mergeSort(a);
//         },
//         arr
//     );
//
//     tester.benchmark(
//         "StringQuickSort",
//         [&](vector<string>& a) {
//             tester.stringQuickSort(a);
//         },
//         arr
//     );
//
//     tester.benchmark(
//         "StringMergeSort",
//         [&](vector<string>& a) {
//             tester.stringMergeSort(a);
//         },
//         arr
//     );
//
//     tester.benchmark(
//         "MSDRadixSort",
//         [&](vector<string>& a) {
//             tester.msdRadixSort(a);
//         },
//         arr
//     );
//
//     return 0;
// }


#include <fstream>
#include <map>

struct Result {
    string algorithm;
    string arrayType;
    int size;
    long long avgTime;
    long long avgComparisons;
};

int main() {

    StringGenerator generator;
    StringSortTester tester;

    // генерация массивов на 3000
    auto baseRandom =
        generator.generateRandomArray(3000);

    auto baseReverse =
        generator.generateReverseSortedArray(3000);

    auto baseAlmost =
        generator.generateAlmostSortedArray(3000);

    vector<Result> results;

    const int RUNS = 10;

    vector<pair<string, function<void(vector<string>&)>>> algorithms = {

        {
            "QuickSort",
            [&](vector<string>& a) {
                tester.quickSort(a);
            }
        },

        {
            "MergeSort",
            [&](vector<string>& a) {
                tester.mergeSort(a);
            }
        },

        {
            "StringQuickSort",
            [&](vector<string>& a) {
                tester.stringQuickSort(a);
            }
        },

        {
            "StringMergeSort",
            [&](vector<string>& a) {
                tester.stringMergeSort(a);
            }
        },

        {
            "MSDRadixSort",
            [&](vector<string>& a) {
                tester.msdRadixSort(a);
            }
        }
    };

    for (int size = 100; size <= 3000; size += 100) {

        cout << "Testing size = " << size << endl;

        vector<pair<string, vector<string>>> testArrays = {

            {
                "random",
                vector<string>(
                    baseRandom.begin(),
                    baseRandom.begin() + size
                )
            },

            {
                "reverse",
                vector<string>(
                    baseReverse.begin(),
                    baseReverse.begin() + size
                )
            },

            {
                "almost_sorted",
                vector<string>(
                    baseAlmost.begin(),
                    baseAlmost.begin() + size
                )
            }
        };

        for (auto& test : testArrays) {

            string arrayType = test.first;

            for (auto& algo : algorithms) {

                string algoName = algo.first;

                long long totalTime = 0;
                long long totalComparisons = 0;

                // заупскаю 10 раз для усреднения
                for (int run = 0; run < RUNS; run++) {

                    vector<string> arr = test.second;

                    tester.resetComparisons();

                    auto start =
                        chrono::high_resolution_clock::now();

                    algo.second(arr);

                    auto end =
                        chrono::high_resolution_clock::now();

                    auto duration =
                        chrono::duration_cast
                        <chrono::microseconds>(
                            end - start
                        ).count();

                    totalTime += duration;

                    totalComparisons +=
                        tester.getComparisons();
                }

                results.push_back({

                    algoName,
                    arrayType,
                    size,

                    totalTime / RUNS,

                    totalComparisons / RUNS
                });

                cout
                    << algoName
                    << " | "
                    << arrayType
                    << " DONE"
                    << endl;
            }
        }
    }

    // CSV
    ofstream out("results.csv");

    out << "algorithm,array_type,size,time_us,char_comparisons\n";

    for (auto& r : results) {

        out
            << r.algorithm << ","
            << r.arrayType << ","
            << r.size << ","
            << r.avgTime << ","
            << r.avgComparisons
            << "\n";
    }

    out.close();

    cout << "\nResults saved to results.csv\n";

    return 0;
}

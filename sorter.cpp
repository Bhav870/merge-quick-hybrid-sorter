// sorter.cpp  —  Merge-Quick Hybrid Sorter  (compile with Emscripten)
//
// Build command:
//   em++ -O2 sorter.cpp -o sorter.js \
//        -s MODULARIZE=1 -s EXPORT_NAME=SorterModule \
//        -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
//        -s EXPORTED_FUNCTIONS='["_set_array","_run_sort","_get_size","_get_element",\
//                               "_get_step_count","_get_step_a","_get_step_b",\
//                               "_get_step_type","_get_comparisons","_get_swaps"]' \
//        --no-entry
//
// Then load the generated sorter.js + sorter.wasm alongside sorter_wasm.js in your HTML.

#include <emscripten/emscripten.h>
#include <vector>
#include <algorithm>

// ─── Shared state ──────────────────────────────────────────────────────────────
static std::vector<int> g_arr;
static std::vector<int> g_steps_a;     // index A for each recorded step
static std::vector<int> g_steps_b;     // index B (-1 if single-element write)
static std::vector<int> g_steps_type;  // 0 = compare, 1 = swap / write
static long long        g_comparisons = 0;
static long long        g_swaps       = 0;

static inline void push_step(int a, int b, int type) {
    g_steps_a.push_back(a);
    g_steps_b.push_back(b);
    g_steps_type.push_back(type);
}

// ─── Merge Sort ────────────────────────────────────────────────────────────────
static void merge(std::vector<int>& arr, int l, int m, int r) {
    std::vector<int> L(arr.begin() + l, arr.begin() + m + 1);
    std::vector<int> R(arr.begin() + m + 1, arr.begin() + r + 1);
    int i = 0, j = 0, k = l;
    while (i < (int)L.size() && j < (int)R.size()) {
        ++g_comparisons;
        push_step(l + i, m + 1 + j, 0);        // compare
        if (L[i] <= R[j]) { arr[k] = L[i++]; }
        else               { arr[k] = R[j++]; ++g_swaps; }
        push_step(k, -1, 1);                    // write
        ++k;
    }
    while (i < (int)L.size()) { arr[k] = L[i++]; push_step(k, -1, 1); ++k; }
    while (j < (int)R.size()) { arr[k] = R[j++]; push_step(k, -1, 1); ++k; }
}

static void merge_sort(std::vector<int>& arr, int l, int r) {
    if (l >= r) return;
    int m = (l + r) / 2;
    merge_sort(arr, l, m);
    merge_sort(arr, m + 1, r);
    merge(arr, l, m, r);
}

// ─── Quick Sort ────────────────────────────────────────────────────────────────
static int partition(std::vector<int>& arr, int l, int r) {
    int pivot = arr[r], i = l - 1;
    for (int j = l; j < r; ++j) {
        ++g_comparisons;
        push_step(j, r, 0);                     // compare with pivot
        if (arr[j] <= pivot) {
            ++i;
            std::swap(arr[i], arr[j]);
            ++g_swaps;
            push_step(i, j, 1);                 // swap
        }
    }
    std::swap(arr[i + 1], arr[r]);
    ++g_swaps;
    push_step(i + 1, r, 1);
    return i + 1;
}

static void quick_sort(std::vector<int>& arr, int l, int r) {
    if (l >= r) return;
    int p = partition(arr, l, r);
    quick_sort(arr, l, p - 1);
    quick_sort(arr, p + 1, r);
}

// ─── Hybrid Merge-Quick Sort ───────────────────────────────────────────────────
// Strategy:
//   • sub-arrays <= threshold  → insertion sort  (cache-friendly, tiny overhead)
//   • sub-arrays > threshold   → quick sort partitioning on large sections
//                                merge sort on medium sections (stable, guaranteed O(n log n))
static void insertion_sort(std::vector<int>& arr, int l, int r) {
    for (int i = l + 1; i <= r; ++i) {
        int key = arr[i], j = i - 1;
        while (j >= l) {
            ++g_comparisons;
            push_step(j, j + 1, 0);
            if (arr[j] > key) { arr[j + 1] = arr[j]; ++g_swaps; push_step(j + 1, -1, 1); --j; }
            else break;
        }
        arr[j + 1] = key;
        push_step(j + 1, -1, 1);
    }
}

static void hybrid_sort(std::vector<int>& arr, int l, int r, int threshold) {
    int size = r - l + 1;
    if (size <= threshold) {
        insertion_sort(arr, l, r);
        return;
    }
    if (size <= 32) {
        // medium: use merge sort for stability
        int m = (l + r) / 2;
        hybrid_sort(arr, l, m, threshold);
        hybrid_sort(arr, m + 1, r, threshold);
        merge(arr, l, m, r);
    } else {
        // large: use quick sort partitioning
        int p = partition(arr, l, r);
        hybrid_sort(arr, l, p - 1, threshold);
        hybrid_sort(arr, p + 1, r, threshold);
    }
}

// ─── Exported C API (called from JavaScript via ccall / cwrap) ─────────────────
extern "C" {

EMSCRIPTEN_KEEPALIVE
void set_array(int* data, int len) {
    g_arr.assign(data, data + len);
    g_steps_a.clear();
    g_steps_b.clear();
    g_steps_type.clear();
    g_comparisons = 0;
    g_swaps       = 0;
}

// algorithm: 0 = Merge Sort, 1 = Quick Sort, 2 = Hybrid
EMSCRIPTEN_KEEPALIVE
void run_sort(int algorithm, int threshold) {
    g_steps_a.clear();
    g_steps_b.clear();
    g_steps_type.clear();
    g_comparisons = 0;
    g_swaps       = 0;
    int n = (int)g_arr.size();
    if (n <= 1) return;
    if      (algorithm == 0) merge_sort(g_arr, 0, n - 1);
    else if (algorithm == 1) quick_sort(g_arr, 0, n - 1);
    else                     hybrid_sort(g_arr, 0, n - 1, threshold);
}

EMSCRIPTEN_KEEPALIVE int       get_size()            { return (int)g_arr.size(); }
EMSCRIPTEN_KEEPALIVE int       get_element(int i)    { return g_arr[i]; }
EMSCRIPTEN_KEEPALIVE int       get_step_count()      { return (int)g_steps_a.size(); }
EMSCRIPTEN_KEEPALIVE int       get_step_a(int i)     { return g_steps_a[i]; }
EMSCRIPTEN_KEEPALIVE int       get_step_b(int i)     { return g_steps_b[i]; }
EMSCRIPTEN_KEEPALIVE int       get_step_type(int i)  { return g_steps_type[i]; }
EMSCRIPTEN_KEEPALIVE long long get_comparisons()     { return g_comparisons; }
EMSCRIPTEN_KEEPALIVE long long get_swaps()           { return g_swaps; }

} // extern "C"

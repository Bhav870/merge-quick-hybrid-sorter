
const Sorter = (() => {
   let _steps = [];
  let _cmp   = 0;
  let _sw    = 0;
 
  function _reset() { _steps = []; _cmp = 0; _sw = 0; }
 
  function _push(arr, i, j, isSwap) {
    _steps.push({
      arr:         arr.slice(),
      i,
      j,
      isSwap,
      comparisons: _cmp,
      swaps:       _sw,
    });
  }
 unction _merge(arr, l, m, r) {
    const L = arr.slice(l, m + 1);
    const R = arr.slice(m + 1, r + 1);
    let i = 0, j = 0, k = l;
    while (i < L.length && j < R.length) {
      ++_cmp;
      if (L[i] <= R[j]) { arr[k++] = L[i++]; }
      else               { arr[k++] = R[j++]; ++_sw; }
      _push(arr, k - 1, -1, false);
    }
    while (i < L.length) { arr[k++] = L[i++]; _push(arr, k - 1, -1, false); }
    while (j < R.length) { arr[k++] = R[j++]; _push(arr, k - 1, -1, false); }
  }
 
  function _mergeRec(arr, l, r) {
    if (l >= r) return;
    const m = l + Math.floor((r - l) / 2);
    _mergeRec(arr, l, m);
    _mergeRec(arr, m + 1, r);
    _merge(arr, l, m, r);
  }
  function _partition(arr, low, high) {
    const pivot = arr[high];
    let i = low - 1;
    for (let j = low; j < high; j++) {
      ++_cmp;
      if (arr[j] <= pivot) {
        ++i;
        [arr[i], arr[j]] = [arr[j], arr[i]]; ++_sw;
        _push(arr, i, j, true);
      } else {
        _push(arr, i + 1, j, false);
      }
    }
    [arr[i + 1], arr[high]] = [arr[high], arr[i + 1]]; ++_sw;
    _push(arr, i + 1, high, true);
    return i + 1;
  }
 
  function _quickRec(arr, low, high) {
    if (low >= high) return;
    const pi = _partition(arr, low, high);
    _quickRec(arr, low, pi - 1);
    _quickRec(arr, pi + 1, high);
  }
 function _insertionSort(arr, low, high) {
    for (let i = low + 1; i <= high; i++) {
      const key = arr[i];
      let j = i - 1;
      while (j >= low) {
        ++_cmp;
        if (arr[j] > key) {
          arr[j + 1] = arr[j]; j--; ++_sw;
          _push(arr, j + 1, i, true);
        } else break;
      }
      arr[j + 1] = key;
      _push(arr, j + 1, -1, false);
    }
  }
 function _hybridRec(arr, low, high, threshold) {
    if (low >= high) return;
    if (high - low + 1 <= threshold) {
      _insertionSort(arr, low, high);
      return;
    }
    const m = low + Math.floor((high - low) / 2);
    _hybridRec(arr, low, m, threshold);
    _hybridRec(arr, m + 1, high, threshold);
    _merge(arr, low, m, high);
  }
 
  function _result() {
    return { steps: _steps, totalComparisons: _cmp, totalSwaps: _sw };
  }
 return {
    /**
     * Run merge sort on a copy of `arr`.
     * @param {number[]} arr
     * @returns {{ steps, totalComparisons, totalSwaps }}
     */
    mergeSort(arr) {
      _reset();
      const a = arr.slice();
      _mergeRec(a, 0, a.length - 1);
      return _result();
    },
 
    /**
     * Run quick sort on a copy of `arr`.
     * @param {number[]} arr
     * @returns {{ steps, totalComparisons, totalSwaps }}
     */
    quickSort(arr) {
      _reset();
      const a = arr.slice();
      _quickRec(a, 0, a.length - 1);
      return _result();
    },
 
    /**
     * Run the hybrid merge+insertion sort on a copy of `arr`.
     * Sub-arrays of length ≤ threshold use insertion sort; larger ones use merge.
     * @param {number[]} arr
     * @param {number} threshold  (default 10, matches C++ default)
     * @returns {{ steps, totalComparisons, totalSwaps }}
     */
    hybridSort(arr, threshold = 10) {
      _reset();
      const a = arr.slice();
      _hybridRec(a, 0, a.length - 1, threshold);
      return _result();
    },
  };
})();
 if (typeof module !== 'undefined' && module.exports) {
  module.exports = Sorter;
}
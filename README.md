# Merge-Quick Hybrid Sorter

A sorting algorithm that mixes Merge Sort, Quick Sort, and Insertion Sort together to try and get better performance than just using one of them alone.

## What it does

Different sorting algorithms are good at different things:
- Insertion Sort is fast for small arrays but slow for big ones
- Quick Sort is fast on average but can get really slow on certain inputs
- Merge Sort is reliable and always O(n log n), but uses more memory

So instead of picking just one, this project switches between them depending on the size of the array:
- Small arrays → Insertion Sort
- Bigger arrays → Quick Sort
- If Quick Sort starts performing badly → falls back to Merge Sort

## Files

- `sorter.cpp` — the actual sorting logic, written in C++
- `sorter_wasm.js` — lets the C++ code run in the browser using WebAssembly
- `index.html` — a simple demo page to try it out

## How to run it

If you just want to try the demo, open `index.html` in your browser.

If you want to rebuild the WebAssembly file yourself, you'll need Emscripten installed, then run:

```bash
emcc sorter.cpp -o sorter_wasm.js -O3 -s WASM=1
```

## Why I made this

I wanted to see if combining sorting algorithms could actually give better real-world performance instead of just using Quick Sort or Merge Sort by itself.

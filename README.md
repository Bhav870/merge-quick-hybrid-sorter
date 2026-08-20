# Merge-Quick Hybrid Sorter

A hybrid sorting algorithm that intelligently combines **Merge Sort**, **Quick Sort**, and **Insertion Sort** to optimize performance across different data sizes and distributions.

## Overview

Instead of relying on a single sorting strategy, this project dynamically switches between algorithms based on the size and nature of the input data:

- **Insertion Sort** — used for small subarrays, where its low overhead outperforms more complex algorithms.
- **Quick Sort** — used for average-case partitioning on larger datasets, offering fast in-place sorting.
- **Merge Sort** — used as a fallback for worst-case scenarios (e.g., already sorted or adversarial inputs) to guarantee stable O(n log n) performance.

This hybrid approach aims to combine the strengths of each algorithm while minimizing their individual weaknesses.

## Project Structure

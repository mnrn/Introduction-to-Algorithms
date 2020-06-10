# Introduction-to-Algorithms

<a href=
    "https://www.kindaikagaku.co.jp/information/kd0408.htm">
    <img src="https://images-na.ssl-images-amazon.com/images/I/513sTP14H+L._SX356_BO1,204,203,200_.jpg"
    width="128"></a><a href=
    "https://www.amazon.com/Introduction-Algorithms-3rd-MIT-Press/dp/0262033844/ref=pd_lpo_14_t_0/130-6759562-6007824?_encoding=UTF8&pd_rd_i=0262033844&pd_rd_r=612e4ca9-871f-4308-82f1-2419900cfde3&pd_rd_w=Td2Q0&pd_rd_wg=idmJR&pf_rd_p=7b36d496-f366-4631-94d3-61b87b52511b&pf_rd_r=B3V0PB9RWJJK79EJSTDX&psc=1&refRID=B3V0PB9RWJJK79EJSTDX"><img src="https://images-na.ssl-images-amazon.com/images/I/51n+3GEQvYL._SX433_BO1,204,203,200_.jpg"
    width="128">
</a>
  
[書籍:アルゴリズムイントロダクション](https://www.kindaikagaku.co.jp/information/kd0408.htm)の C++ / Fortran / Python / Cilk Plus による実装。

## Overview

残念ながら問題の見落としや、Fibonacci Heap などのように紛失してしまったコード部分もあります。

C++ を思い出しながら書いたプログラムなのであまり洗練されたコードではありません。  
Graph アルゴリズムに関しては [こちら](https://github.com/mnrn/graph) に書き直したものがあります。

- 基礎 (Foundations)
  - [挿入ソート (Insertion sort)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/Insertionsort)
  - [Strassenのアルゴリズム (Strassen's Algorithm)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/MatrixOperations/Strassen)
  - [マージソート (Merge sort)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/Mergesort)
- ソートと順序統計量 (Sort and Order Statistics)
  - [ヒープソート (Heap sort)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/Heapsort)
  - [クイックソート (Quick sort)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/Quicksort)
  - [中央値と順序統計量 (Medians and Order Statistics)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/Selection)
- データ構造 (Data Structures)
  - [スタック (Stack)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/Stack)
  - [キュー (Queue)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/Queue)
  - [2分探索木 (Binary Search Trees)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/BinarySearchTrees)
  - [2色木 (Red Black Trees)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/RedBlackTrees)
  - [AVL木 (AVL Trees)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/AVLTrees)
- 高度な設計と解析の手法 (Advanced Design and Analysis Techniques)
  - [動的計画法 (Dynamic Programming)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/DynamicProgramming)
  - [貪欲アルゴリズム (Greedy Algorithms)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/Greedy)
- 高度なデータ構造 (Advanced Data Structures)
  - [B木 (B-Trees)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/BTrees)
  - [互いに素な集合族 (Disjoint-set)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/DisjointSet)
- 基本的グラフアルゴリズム (Elementary Graph Algorithms)
  - [幅優先探索 (Breadth-first Search)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/BFS)
  - [深さ優先探索 (Depth-first Search)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/DFS)
  - [トポロジカルソート (Topological sort)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/Topologicalsort)
  - [強連結成分 (Strongly connected components)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/SCC)
- 最小全域木 (Minimu Spanning Trees)
  - [Primのアルゴリズム (Prim's Algorithm)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/Prim)
  - [Kruskalのアルゴリズム (Kruskal's Algorithm)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/Kruskal)
- 単一始点最短路 (Single-Shource Shortest Paths)
  - [Dijkstraのアルゴリズム (Dijkstra's Algorithm)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/Dijkstra)
  - [Bellman-Fordのアルゴリズム (Bellman-Ford's Algorithm)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/BellmanFord)
- 全点対最短路 (All-Pairs Shortest Paths)
  - [Floyd-Warshallのアルゴリズム (Floyd-Warshall's Algorithm)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/FloydWarshall)
- 最大フロー (Maximum Flow)
  - [Ford-Fulkersonのアルゴリズム (Ford-Fulkerson's Algorithm)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/FordFulkerson)
  - [Edmonds-Karpのアルゴリズム (Edmonds-Karp's Algorithm)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/EdmondsKarp)
- マルチスレッドアルゴリズム (MultiThreaded Algorithms)
  - [Fibonacciのアルゴリズム (Fibonacci's Algorithm)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/MultiThreaded/Fib)
  - [行列積 (Matrix Multiply)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/MultiThreaded/MatrixMult)
  - [マージソート (Merge Sort)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/MultiThreaded/MergeSort)
- 行列演算 (Matrix Operations)
  - [LPU分解 (LUP decomposition)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/MatrixOperations/LUP)
- 線形計画法 (Linear Programming)
  - [シンプレックスアルゴリズム (The simplex algorithm)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/LinearProgramming)
- 多項式とFFT (Polinomials and the FFT)
  - [DTFとFFT (The DFT and FFT)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/FFT)
- 数論的アルゴリズム (Number-Theoric Algorithms)
  - [最大公約数 (Greatest common divisor)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/NumberTheory/GCD)
  - [剰余演算 (Modular zrthmeric)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/NumberTheory/Modular)
  - [Miller-Rabinのアルゴリズム (Miller-Rabin's Algorithm)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/NumberTheory/Prime)
- 文字列照合 (String Matching)
  - [素朴な文字列照合アルゴリズム (The naive string-matching Algorithm)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/StringMatching/Naive)
  - [Rabin-Karpのアルゴリズム (Rabin-Karp's Algorithm)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/StringMatching/RabinKarp)
  - [有限オートマトンを用いる文字列照合 (String matching with finite automata)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/StringMatching/FiniteAutomaton)
  - [Knuth-Morris-Prattのアルゴリズム (Knuth-Morris-Pratt's Algorithm)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/StringMatching/KnuthMorrisPratt)
- 計算幾何学 (Computational Geometry)
  - [線分交差判定 (Determining whether any pair of segments intersects)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/ComputationalGeometry/Segments)
  - [凸包の構成 (Finding the convex hull)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/ComputationalGeometry/ConvexHull)
  - [最近点対の発見 (Finding the closest pair of points)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/ComputationalGeometry/ClosestPair)
- 近似アルゴリズム (Approxmetry Algorithms)
  - [頂点被覆問題 (The vertex-cover problem)](https://github.com/mnrn/Introduction-to-Algorithms/tree/master/Approximation/VertexCover)

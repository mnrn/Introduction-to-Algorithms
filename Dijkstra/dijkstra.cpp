/**
 * @brief  最短経路問題(shortest paths problem)におけるDijkstraのアルゴリズム(Dijkstra's algorithm)を扱う
 * @date   作成日     : 2016/02/20
 * @date   最終更新日 : 2016/02/20
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include "Dijkstra.hpp"
#include "../PriorityQueue/pqueue.hpp"
#include <iostream>



//****************************************
// 関数の定義
//****************************************

/**
 * @brief  すべての辺重みが非負であるという仮定の下で、Dijkstra(ダイクストラ)のアルゴリズム(Dijkstra's algorithm)は
 *         重み付き有向グラフG = (V, E)上の単一始点最短路問題を解く. ここでは各辺(u, v) ∈ Eについてw(u, v) >= 0を仮定する
 *
 * @note   Dijkstraのアルゴリズムは、始点sからの最短路重みが最終的に決定された頂点の集合Sを管理する
 *         アルゴリズムは繰り返し、最小の最短路推定値を持つ頂点u ∈ V - Sを選択し、uをSに追加し、
 *         uから出るすべての辺を緩和する. ここではd値をキーとする頂点のmin優先度付きキューQを用いる
 *
 * @note   優先度付きキューの優先度更新を行わないため、優先度付きキューが空になるまでに行われる挿入の数はΟ(E)であるが、
 *         EXTRACT-MIN呼び出し時に、最短路の更新が行われないならば、無視をすることで、全体としての実行時間をΟ(ElgV)としている
 *
 * @param  const graph_t& G    非負の重み付き有向グラフG
 * @param  index_t        s    始点s
 * @return 始点sからの最短路重みが最終的に決定された頂点の集合S
 */
vertices_t dijkstra(const graph_t& G, index_t s)
{
    using pair_t = std::pair<index_t, weight_t>;        
    std::int32_t n = G.size();
    vertices_t S(n);
    struct cmp { bool operator () (const pair_t& p, const pair_t& q) { return p.second > q.second; } };
    std::size_t m = 0; for (auto& es : G) { m += es.size(); }
    pqueue<pair_t, cmp> Q(m);

    // Θ(V)の手続きによって最短経路推定値と先行点を初期化する
    auto initsinglesource = [](vertices_t& S, index_t s) -> void {
        for (auto& v : S) { v.d = graph::inf; v.pi = graph::nil; v.color = color::white; }
        S[s].d = 0; S[s].color = color::gray;
    };
    // 辺(u, v)の緩和(relaxing)はuを経由することでvへの既知の最短路が改善できるか否かを判定し、改善できるならばv.dとv.πを更新する
    // 緩和によって最短路推定値v.dが減少し、vの先行点属性v.πが更新されることがある. 以下のコードは、辺(u, v)上の緩和をΟ(1)時間で実行する
    auto relax = [](const edge& e, vertices_t& S, pqueue<pair_t, cmp>& Q) ->void {
        index_t v = e.dst, u = e.src;
        if (S[v].color != color::black &&  S[v].d > S[u].d + e.w) {
            S[v].d = S[u].d + e.w; S[v].pi = u; S[v].color = color::gray;
            Q.insert(std::make_pair(v, S[v].d)); 
        }
    };


    
    initsinglesource(S, s);               // すべての頂点のd値とπ値を初期化する
    Q.insert(std::make_pair(s, S[s].d));  // このループの最初の実行ではu = sである
    while (!Q.empty()) {
        pair_t  p = Q.extract();
        index_t u = p.first; weight_t d = p.second;
        if (S[u].d < d) { continue; }
        for (auto& e : G[u]) {  // 頂点uからでる辺(u, v)をそれぞれ緩和し、
            relax(e, S, Q);     // uを経由することでvへの最短路が改善できる場合には、推定値v.dと先行点v.piを更新する
        }
        S[u].color = color::black;  // 黒頂点は集合Sに属す
    }
    // 終了時点ではQ = φである. S = Vなので、すべての頂点u ∈ Vに対してu.d = δ(s, u)である
    // また、このとき、先行点部分グラフGπはsを根とする最短路木である
    return S;
}


/**
 * @brief  すべての辺重みが非負であるという仮定の下で、Dijkstra(ダイクストラ)のアルゴリズム(Dijkstra's algorithm)は
 *         重み付き有向グラフG = (V, E)上の単一始点最短路問題を解く. ここでは各辺(u, v) ∈ Eについてw(u, v) >= 0を仮定する
 *
 * @note   頂点v ∈ Vに対し、それぞれΟ(V)時間の操作を行うので、全体でΟ(V^2)時間を要す
 *
 * @param  const matrix_t& W    非負の重み付き有向グラフW
 * @param  index_t         s    始点s
 * @return 始点sからの最短路重みが最終的に決定された頂点の集合S
 */
vertices_t dijkstra(const matrix_t& W, index_t s)
{
    std::int32_t n = W.size();
    vertices_t S(n);
     // Θ(V)の手続きによって最短経路推定値と先行点を初期化する
    auto initsinglesource = [](vertices_t& S, index_t s) -> void {
        for (auto& v : S) { v.d = graph::inf; v.pi = graph::nil; v.visited = false; }
        S[s].d = 0;
    };
    // 辺(u, v)の緩和(relaxing)はuを経由することでvへの既知の最短路が改善できるか否かを判定し、改善できるならばv.dとv.πを更新する
    // 緩和によって最短路推定値v.dが減少し、vの先行点属性v.πが更新されることがある. 以下のコードは、辺(u, v)上の緩和をΟ(1)時間で実行する
    auto relax = [](const matrix_t& W, index_t v, index_t u,  vertices_t& S) ->void {
        if (!S[v].visited &&  S[v].d > S[u].d + W[u][v]) {
            S[v].d = S[u].d + W[u][v]; S[v].pi = u;
        }
    };
    // 集合Sに属さない"最も軽い"頂点を求める
    auto extractmin = [](const vertices_t& S, std::int32_t n) -> index_t {
        index_t u = graph::nil;
        for (index_t v = 0; v < n; v++ ) {
            u = (!S[v].visited && (u == graph::nil || S[v].d < S[u].d)) ? v : u;
        }
        return u;
    };
    


    initsinglesource(S, s);  // すべての頂点のd値とπ値を初期化する
    while(true) {     
        // 始点sからの最小の最短路推定値を持つ頂点u ∈ V - Sを選択する
        index_t u = extractmin(S, n);   
        if (u == graph::nil) { break; }  // 頂点uがNILを指すならば、探索は終了である
        for (index_t v = 0; v < n; v++) {
            relax(W, v, u, S);  // uを経由することでvへの最短路が改善できる場合には、推定値v.dと先行点v.piを更新する
        }
        S[u].visited = true;  // 黒頂点は集合Sに属す
    }
    // 終了時点では、S = Vなので、すべての頂点u ∈ Vに対してu.d = δ(s, u)である
    // また、このとき、先行点部分グラフGπはsを根とする最短路木である
    return S;
}



int main(void)
{
    using namespace std;
    matrix_t M;
    int n, k, u, v, c;
    
    cin >> n; M.resize(n);
    for (int j = 0; j < n; j++) {
        M[j].resize(n);
        for (int i = 0; i < n; i++) {
            M[j][i] = graph::inf;
        }
    }
    for (int j = 0; j < n; j++) {
        cin >> u >> k;
        for (int i = 0; i < k; i++) {
            cin >> v >> c;
            M[u][v] = c;
        }
    }
    vertices_t S = dijkstra(M, 0);
    for (int i = 0; i < n; i++) {
        cout << i << " " << S[i].d << endl; 
    }

    return 0;
}


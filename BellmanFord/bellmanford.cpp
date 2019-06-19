/**
 * @brief  最短経路問題(shortest paths problem)におけるBellman-Fordアルゴリズム(Bellman-Ford algorithm)を扱う
 * @date   作成日     : 2016/02/18
 * @date   最終更新日 : 2016/02/18
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <iostream>
#include "bellmanford.hpp"



//****************************************
// 関数の定義
//****************************************

/**
 * @brief  Bellman-Fordアルゴリズム
 *
 * @note   Bellman-Fordアルゴリズムは負辺の存在を許す一般の単一始点最短路問題を解く
 *         重み関数w: E -> Rを持つ重み付き有向グラフG= (V, E)と始点sが与えられたとき、
 *         始点から到達可能な負閉路が存在するか否かを示すブール値をBellman-Fordアルゴリズムは返す
 *         アルゴリズムは、このような負閉路が存在すれば解が存在しないと報告し、そうでなければ最短路とその重みを生成する
 *
 * @note   Bellman-Fordアルゴリズムは、辺を次々に緩和することで、始点sから各頂点v ∈ Vへの最短路重みの推定値v.dを
 *         実際の最短路重みδ(s, v)に一致するまで徐々に減らす
 *         アルゴリズムが値TRUEを返すのは、グラフの始点から到達可能な負閉路を含まないとき、かつそのときに限る
 *
 * @note   Bellman-FordアルゴリズムはΟ(VE)時間で走る
 *
 * @param  const graph_t& G グラフG
 * @param  index_t        s 始点s
 */
std::pair<bool, vertices_t> bellmanford(const graph_t& G, index_t s)
{   
    std::int32_t n = G.size();
    vertices_t V(n);

    // Θ(V)の手続きによって最短路推定値と先行点を初期化する
    auto initsinglesource = [](vertices_t& V, index_t s, std::int32_t n) -> void {
        for (std::int32_t i = 0; i < n; i++) { V[i].d  = graph::inf; V[i].pi = graph::nil;}
        V[s].d = 0;
    };
    // 辺(u, v)の緩和(relaxing)はuを経由することでvへの既知の最短路が改善できるか否か判定し、改善できるならばv.dとv.πを更新する
    // 緩和によって最短路推定値v.dが減少し、vの先行点属性v.πが更新されることがある. 以下のコードは、辺(u, v)上の緩和をΟ(1)時間で実行する
    auto relax = [](const edge& e, vertices_t& V) ->void {
        index_t v = e.dst, u = e.src;
        if (V[u].d != graph::inf && V[v].d > V[u].d + e.w) { V[v].d = V[u].d + e.w; V[v].pi = u; }
    };

    
    initsinglesource(V, s, n);  // すべての頂点のd値とπ値を初期化する
    // アルゴリズムはグラフのすべての辺を|V| - 1回走査する
    for (std::int32_t i = 0; i < n - 1; i++) {
        for (auto& es : G) { for (auto& e : es) {
                relax(e, V);  // グラフの各辺をそれぞれ1回緩和する
            }
        }
    }
    // Gがsから到達可能な負閉路を含まなければ、終了時に、すべての辺(u, v)に対して、
    //   u.d = δ(s, v)
    //      <= δ(s, u) + w(u, v) (∵ 三角不等式)
    //       = u.d + w(u, v)
    // だから、BELLMAN-FORDは値FALSEを返すことはなく、TRUEを返す
    for (auto& es : G) { for (auto& e : es) {               // 負の重みを持つ閉路の有無を判定する
            index_t v = e.dst, u = e.src;
            if (V[u].d != graph::inf && V[v].d > V[u].d + e.w) {  // Gが始点sから到達可能な負閉路を含むとき、
                return std::make_pair(false, V);       // FALSEを返す
            }
        }
    }
    // Gがsから到達可能な負閉路を含まなければ、値TRUEを返し、すべての頂点v ∈ Vに対してδ(s, v)が成り立ち、
    // 先行点部分グラフGπはsを根とする最短路木である
    return std::make_pair(true, V);
}



int main(void)
{
    using namespace std;
    int V, E, r;
    
    while (cin >> V >> E >> r) {
        graph_t G(V);
        for (int i = 0; i < E; i++) {
            int a, b, c;
            cin >> a >> b >> c;
            G[a].emplace_back(a, b, c);
        }
        auto t = bellmanford(G, r);
        if (t.first) {
            for (int i = 0; i < V; i++) {
                if (t.second[i].d > 1000000) { cout <<  "INF"; }
                else                         { cout << t.second[i].d;   }
                cout << endl;
            }
        }
        else {
            cout << "NEGATIVE CYCLE" << endl;
        }
    }
}

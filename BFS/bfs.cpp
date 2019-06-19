/**
 * @brief 幅優先探索の実装
 * @date  作成日     : 2016/02/11
 * @date  最終更新日 : 2016/02/11
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <iostream>
#include <iterator>
#include <functional>
#include <utility>
#include <algorithm>
#include <vector>

#include "../Graph/graph.hpp"
#include "../Queue/queue.hpp"



//****************************************
// 関数の定義
//****************************************

/**
 * @brief  幅優先探索を行います
 *
 * @note   幅優先探索(breadth-first-search)は最も単純なグラフ探索アルゴリズムの1つである
 *         グラフG=(V, E)を始点(source vertex)sが与えられたとき、幅優先探索はGの辺を組織的に探索して、
 *         sから到達可能なすべての頂点を"発見し"、すべての到達可能な頂点についてsからの距離(変数の最小値)を計算する
 *         さらに、sを根とし、到達可能な頂点をすべて含む「幅優先木」を構成する. sから到達可能な任意の頂点vについて、
 *         幅優先木におけるsからvへの単純道はGにおけるsからvへの「最短路」、すなわち最小数の辺を含む道に対応する
 *         有向グラフと無向グラフのどちらに対してもこのアルゴリズムは正しく動く
 *
 *         探索頂点と未探索頂点の境界を、境界の幅一杯にわたって一様に拡張することが幅優先探索という名前の由来である
 *         すなわち、このアルゴリズムはsから距離k+1にある頂点を発見する前に距離kにあるすべての頂点を発見する
 *
 * @note   手続きBFSはグラフを探索しながら幅優先木を構築する.この木はπ属性に対応する.形式的に言うと、
 *         sを始点とするグラフG=(V, E)に対して、Gの先行点部分グラフ(predecessor subgraph)をGπ=(Vπ, Eπ)として定義する.ただし、
 *           Vπ = { v ∈ V : v.π != NIL } ∪ {s}
 *         かつ
 *           Eπ = { (v.π, v) : v ∈ Vπ - {s} }
 *         である.Vπがsから到達可能な全頂点から構成され、すべてのv∈Vπに対して、sからvに至る唯一の単純道がGπに存在し、
 *         しかもこれがGにおけるsからvに至る最短路になっているとき、先行点部分グラフGπを幅優先木(breadth-first tree)と呼ぶ
 *         幅優先木は連結で|Eπ| = |Vπ| - 1を満たすから、実際に木である. Eπの辺を木辺(tree edge)と呼ぶ
 *
 * @note   BFSの総実行時間はΟ(V+E)である.したがって、幅優先探索はGの隣接リスト表現のサイズの線形時間で走る
 *
 * @param  const graph& G  グラフG
 * @param  std::int32_t s  始点s
 * @return 幅優先木
 */
vertices_t bfs(const graph_t& G, index_t s)
{
    std::int32_t n = G.size();
    vertices_t V(n);

    for (auto& u : V) {             // すべての頂点uについて、
        u.color = color::white;     // uを白に彩色し、
        u.d     = graph::inf;       // u.dを無限大に設定し、
        u.pi    = graph::nil;       // uの親をNILに設定する
    }
    // 手続き開始と同時に始点sを発見すると考え、
    V[s].color = color::gray;       // 始点sを灰色に彩色する 
    V[s].d     = 0;                 // s.dを0に初期化し、
    V[s].pi    = graph::nil;        // 始点の先行点をNILに設定する

    queue<index_t> Q(n);
    Q.enqueue(s);                   // sだけを含むようにQを初期化する

    // 以下のwhile文に対して、つぎのループ不変式が成立する
    // while文の条件判定を行う時点ではキューQはすべての灰頂点を含む
    while (!Q.empty()) {
        index_t u = Q.dequeue();
        for (auto& e : G[u]) {                   // uの隣接リストに
            index_t v = e.dst;                   // 属する各頂点vを考える
            if (V[v].color == color::white) {    // vが白ならvは未発見である
                V[v].color = color::gray;        // vを灰色に彩色し、
                V[v].d     = V[u].d + 1;         // 距離v.dをu.d+1に設定し、
                V[v].pi    = u;                  // uをその親v.piとして記録し、
                Q.enqueue(v);                    // vをキューQの末尾に置く
            }
        }
        V[u].color = color::black;  // uの隣接リストに属するすべての頂点の探索が完了すると、この頂点を黒に彩色する
    }
    // ある頂点を灰に彩色したときには、この頂点をQへ挿入し、ある頂点をQから削除したときには、この頂点を黒に彩色するので、
    // ループ不変式が保存される

    return V;
}


/**
 * @brief BFSが幅優先木を計算した後でこの手続きを用いれば、sからvへの最短路上の頂点を印刷できる
 */
void printpath(const vertices_t& V, index_t s, index_t v)
{
    if (v == s) {
        std::cout << s << " ";
    }
    else if (V[v].pi == graph::nil) {
        std::cout << "s\"から\"vへの道は存在しない\n"; 
    }
    else {
        printpath(V, s, V[v].pi);
        std::cout << v << " ";
    }
}



int main(void)
{
    using namespace graph;
    const int vs = 8;
    graph_t G;
    G.resize(vs);
    int adjs[8][3] = { { 1, 4, }, { 0, 5, }, { 3, 5, 6, }, {2, 6, 7, } ,
                       { 0, }, {1, 2, 6 }, { 2, 5, 7, }, {3, 6, } };
    int adji[8] = { 2, 2, 3, 3, 1, 3, 3, 2, };

    for (int j = 0; j < vs; j++) {
        G[j].resize(adji[j]);
        std::cout << j << " : ";
        for (int i = 0; i < adji[j] ; i++) {
            G[j][i].src = j;
            G[j][i].dst = adjs[j][i];
            std::cout << G[j][i].dst << " ";
        }
        std::cout << std::endl;
    }
  
    auto bfstree = bfs(G, 1);
    for (auto v : bfstree) {
        std::cout << v.d << " ";
    }
    std::cout << std::endl;

    printpath(bfstree, 1, 3);
    std::cout << std::endl;
    
    return 0;
}


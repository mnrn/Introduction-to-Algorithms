/**
 * @brief 強連結成分(分解)アルゴリズムの実装
 * @date  作成日     : 2016/02/14
 * @date  最終更新日  : 2016/02/14
 */


//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include "../Graph/graph.hpp"
#include "../Topologicalsort/tsort.hpp"
#include <iostream>



//****************************************
// 関数の定義
//****************************************

 /**
 * @brief  強連結成分(分解)アルゴリズム
 *
 * @note   グラフを強連結成分に分解した後、個々の強連結成分上でアルゴリズムを実行し、
 *         得られた解を成分間の連結構造にしたがって組み合わせて最終的に解を得る
 *
 * @note   グラフG = (V, E)の強連結成分を求めるアルゴリズムはGの転置を用いる
 *         Gの転置はグラフG^T = (V, E^T), E^T = { (u, v) : (v, u) ∈ E }である
 *         すなわち、E^TはGの辺の方向を逆にしたものである. Gの隣接リストが与えられると、
 *         G^TをΟ(V + E)時間で生成できる. 興味深いことに、GとG^Tはまったく同じ強連結成分を持つ
 *         G上でuとvが互いに到達可能であることと、G^T上でこれらが互いに到達可能であることとは等価である
 *
 * @note   以下の線形時間(すなわち、Θ(V + E)時間)アルゴリズムは、深さ優先探索を2回、最初にG上で、
 *         つぎにG^T上で実行することによって有向グラフG = (V, E)の強連結成分を求める
 *
 *         STRONGLY-CONNECTED-COMPONENTS(G)
 *         1 DFS(G)を呼び出し、各頂点uに対して終了時刻u.fを計算する
 *         2 G^Tを計算する
 *         3 DFS(G^T)を呼び出すが、DFSの主ループでは(第1行で計算した)u.fの降順で頂点を探索する
 *         4 第3行で生成した深さ優先森の各木の頂点を、それぞれ分離された強連結成分として出力する
 *
 * @param  const graph& G グラフG
 * @return components[v] 頂点vが含まれる連結成分の番号となるような集合
 */
std::vector<index_t> scc(const graph_t& G)
{
    std::int32_t n = G.size(); vertices_t vs(n);
    indices_t components(n, -1);
    std::vector<color> _color(n, color::white);
    graph_t GT(n);

    std::function<void(index_t, index_t)> visit = [&](index_t u, index_t k) {
        _color[u] = color::gray;
        components[u] = k;
        for (auto& e : GT[u]) {
            index_t w = e.dst;
            if (_color[w] == color::white) {
                visit(w, k);
            }
        }
        _color[u] = color::black;
    };


    // DFS(G)を呼び出し、各頂点uに対して終了時刻u.fを計算する
    array_t tlst = tsort(G);

    // G^Tを計算する
    for (auto& es : G) {
        for (auto& e : es) {
            GT[e.dst].emplace_back(e.dst, e.src);
        }
    }

    // DFS(G^T)を呼び出す
    index_t k = 0;
    for (auto u : tlst) { // 成分グラフの頂点をトポロジカルソートされた順序で訪問する
        if (_color[u] == color::white) {
            visit(u, k++);
        }
    }

    // 分離された強連結成分を出力
    return components;
}








/**
 * @brief トポロジカルソートの実装
 * @date  作成日     : 2016/02/14
 * @date  最終更新日 : 2016/02/14
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <functional>
#include "tsort.hpp"



 //****************************************
// 関数の定義
//****************************************

/**
 * @brief  トポロジカルソートを行います
 *
 * @note   有向非巡回グラフG = (V, E)のトポロジカルソート(topological sort)は頂点集合上の線形順序で、
 *         Gが辺(u, v)を含むならば、この線形順序でuがvより先に現れるものである. (グラフに巡回路があればこのような線形順序は存在しない)
 *         グラフのトポロジカルソートは、すべての有向辺が左から右へ向かう、水平線上での頂点の並べ方である
 * @param  const graph& G 有向非巡回グラフ
 * @return 既ソートリスト
 */
array_t tsort(const graph_t& G)
{
    std::int32_t n = G.size();
    std::vector<color> _color(n, color::white);
    array_t lst(n);

    // 白節点を訪れる
    std::function<bool(index_t)> visit = [&](index_t u) {
        _color[u] = color::gray;   // uを灰に彩色する
        for (auto& e : G[u]) {     // vと隣接する各頂点wを調べ、
            index_t w = e.dst;
            if (_color[w] == color::white
             && !visit(w)) { return false; }  // wが白なら再帰的にwを調べる
        }
        _color[u] = color::black;  // uを黒に彩色する
        lst.push_back(u);          // リストの末尾に挿入する
        return true;
    };


    // 各頂点vの終了時刻v.fを計算するためにDFS(G)を呼び出す
    for (index_t v = 0; v < n; v++) {
        if (_color[v] == color::white && !visit(v)) { return {}; };
    }
    reverse(lst.begin(), lst.end());  // リストが逆順にソートされているのでreverseを行う
    return lst;     // 頂点のリストを返す
}







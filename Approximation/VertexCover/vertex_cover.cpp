/**
 * @brief 与えられた無向グラフに対して、サイズ最小の頂点被覆を探す問題を頂点被覆問題(vertex-cover problem)と呼び、
 *        そのような頂点被覆を最適頂点被覆(optimal vertex cover)と呼ぶ. この問題はNP完全な決定問題の最適化版である
 *
 * @note  無向グラフG=(V, E)の頂点被覆(vertex cover)は部分集合V'⊆Vで、(u, v)がGの辺ならば、
 *        u ∈ V'かv ∈ V'(あるいは両方)を満たすものであった. 頂点被覆のサイズはそれが含む頂点の個数である
 *
 * @date  2016/03/28
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <iostream>
#include "../../Graph/graph.hpp"



//****************************************
// 型シノニム
//****************************************

using bool_t   = std::int32_t;
using stamps_t = std::vector<bool_t>;



//****************************************
// 名前空間の始点
//****************************************

GRAPH_BEGIN



//****************************************
// 関数の定義
//****************************************

/**
 * @brief  グラフGから最適頂点被覆を多項式時間で発見する方法は知られていないが、最適に近い頂点被覆は効率よく発見できる
 *         次の近似アルゴリズムは無向グラフGを入力として取り、そのサイズが最適頂点被覆の2倍を超えないことが保証できる頂点被覆を返す
 *
 * @note   APPROX-VERTEX-COVERの計算時間はΟ(V + E)でり、多項式時間2近似アルゴリズムである
 *
 */
indices_t approx_vertex_cover(const graph_t& G)
{
    indices_t C;  // Cを空集合に初期化

    index_t n = G.size();
    stamps_t visited(n, false);

    // E'から辺(u, v)を取り出し、その両端点uとvをCに付け加え、
    // uまたはvのどちらかが被覆するすべての辺をE'から削除することを繰り返す
    for (index_t u = 0; u < n; u++) {
        if (visited[u]) { continue; }
        
        for (auto& e : G[u]) {
            index_t v = e.dst;
            if (visited[v]) { continue; }

            C.push_back(u); C.push_back(v);
            visited[u] = visited[v] = true;
            break;
        }
    }
    return C;
}



//****************************************
// 名前空間の終点
//****************************************

GRAPH_END



//****************************************
// エントリポイント
//****************************************

int main()
{
    using namespace std;
    using namespace graph;

    enum {
        a, b, c, d, e, f, g, end,
    };
    const char* name[] = { "a", "b", "c", "d", "e", "f", "g", };

    graph_t G(end);
    G[a].resize(1);
    G[a][0] = edge(a, b);

    G[b].resize(2);
    G[b][0] = edge(b, a);
    G[b][1] = edge(b, c);

    G[c].resize(3);
    G[c][0] = edge(c, b);
    G[c][1] = edge(c, d);
    G[c][2] = edge(c, e);

    G[d].resize(4);
    G[d][0] = edge(d, c);
    G[d][1] = edge(d, e);
    G[d][2] = edge(d, f);
    G[d][3] = edge(d, g);

    G[e].resize(3);
    G[e][0] = edge(e, c);
    G[e][1] = edge(e, d);
    G[e][2] = edge(e, f);

    G[f].resize(2);
    G[f][0] = edge(f, d);
    G[f][1] = edge(f, e);

    G[g].resize(1);
    G[g][0] = edge(g, d);

    for (auto& es : G) {
        for (auto& e : es) {
            cout << name[e.src] << " " << name[e.dst] << endl;
        }
        cout << endl;
    }

    indices_t C = approx_vertex_cover(G);

    for (auto& v : C) {
        cout << name[v] << endl;
    }

    return 0;
}


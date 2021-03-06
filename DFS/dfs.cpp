/**
 * @brief 深さ優先探索のテストプログラム
 * @date  作成日     : 2016/02/13
 * @date  最終更新日  : 2016/02/13
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************


#include <iterator>
#include <functional>
#include <utility>
#include <algorithm>
#include <vector>
#include <iostream>

#include "../Graph/graph.hpp"
#include "../Stack/stack.hpp"



//****************************************
// 関数の定義
//****************************************

/**
 * @brief  深さ優先探索を行います
 * 
 * @note   深さ優先探索は、その名が示すように、可能ならば常にそのグラフの"より深い部分を"探索するという戦略に従う
 *         未探索の外向辺が残る頂点の中で、最後に発見した頂点vから出る辺を深さ優先探索は探索する. vの辺をすべて探索し終えると、
 *         vを発見したときに通った辺を「バックトラック(逆戻り)」し、vの直前の頂点を出る(未探索の)辺の探索に戻る
 *         この処理は視点から到達可能なすべての頂点を発見するまで続く. 未発見の頂点が残されていれば、その1つを新たな始点として
 *         探索を繰り返す. アルゴリズムはすべての頂点を発見するまでこのプロセスを繰り返す
 *
 *         幅優先探索と同様、発見済みの頂点uの隣接リストを走査中に頂点vを発見すると、深さ優先探索はvの先行点属性v.πをuに設定し、
 *         この事象を記録する. 先行点部分グラフが木である幅優先探索と違い、深さ優先探索では複数の始点から探索を繰り返すことがあるから、
 *         先行点部分グラフが複数の木から構成されることがある. そこで、深さ優先探索の先行点部分グラフ(predecessor subgraph)を
 *         幅優先探索と少し違って、Gπ = (V, Eπ)と定義する. ここで
 *           Eπ = { (v.π, v) : v ∈ V かつ v.π != NIL }
 *         である. 深さ優先探索の先行点部分グラフは複数の深さ優先木(depth-first tree)から構成される深さ優先森(depth-first forest)
 *         を形成する. Eπに属する辺を木辺(tree edge)と呼ぶ
 *
 *         幅優先探索と同様、深さ優先探索は頂点を探索状態で示す色で彩色する. 初期状態では各頂点は白であり、探索の中で発見されれば(discovered)
 *         灰に変わり、探索が終了すれば(finished)、すなわちこの頂点の隣接リストが完全に黒に変わる. 深さ優先探索では、
 *         各頂点が1つの深さ優先木の中にだけ現れることが保証されるので、これらの木は互いに共通部分を持たない
 *
 *         深さ優先探索は深さ優先森を構成するとともに各頂点に時刻印(timestamp)を押す. 各頂点vは2種類の時刻印を持つ
 *         第1の時刻印v.dはvを最初に発見し、灰に彩色した時刻を記録する. 第2の時刻印v.fはvの隣接リストを調べ終えて黒に彩色した時刻を記録する
 *         これらの時刻印はグラフ構造に関する重要な情報を供給し、深さ優先探索の振る舞いを理解するのに役立つ
 *
 *         以下に示す手続きDFSでは、頂点uを発見した時刻を属性u.dに記録し、uの処理を終了した時刻を属性u.fに記録する
 *         |V|個の各頂点について発見事象と終了事象はそれぞれ1個しか生起しないから、時刻印は1から2|V|の範囲の整数である
 *         任意の頂点uについて
 *           u.d < u.f
 *         が成立する. 頂点uは時刻u.d以前はWHITE, 時刻u.dと時刻u.fの間はGRAY, 時刻u.f以降はBLACKである
 *
 * @note   DFSの実行時間はΘ(V + E)である
 *
 * @note   深さ優先探索の興味深い性質の1つに入力されたグラフG = (V, E)の辺の分類が探索を使ってできることがある
 *         辺の分類からグラフに関する重要な情報が収集できる. たとえば、有向グラフが非巡回であるための必要十分条件が、
 *         深さ優先探索によって「後退」辺が生じないことである
 *
 *         G上の深さ優先探索が生成する深さ優先森Gπを用いて4種類の辺を定義できる
 *
 *         1. 深さ優先森Gπを構成する辺を木辺(tree edge)という. 辺(u, v)を探索することでvを始めて発見したならば、辺(u, v)は木片である
 *
 *         2. ある深さ優先木のある頂点uとその祖先vを結ぶ辺(u, v)を後退辺(back edge)という
 *            有向グラフが自己ループを含むことがあるが、これは後退辺とみなす
 *
 *         3. ある深さ優先木のある頂点uとその子孫vを結ぶ辺(u, v)で木片ではないものを前進辺(forward edge)という
 *
 *         4. 上記以外のすべての辺を横断辺(cross edge)という. 一方の頂点が祖先でない限り、同じ深さ優先木の頂点を結ぶものであってよいし、
 *            また、2つの深さ優先木を結ぶものであっても良い
 *
 *
 * @note   DFSアルゴリズムは辺に出会ったときに、手持ちの情報だけからこの辺を分類できることがある
 *         辺(u, v)を最初に探索した時、頂点vの色がこの辺について何かを教えてくれるというのがキーとなるアイデアである
 *
 *         1. WHITEは木片であることを示す
 *         2. GRAYは後退辺であることを示す
 *         3. BLACKは前進辺あるいは横断辺であることを示す
 *
 *
 * @param  グラフG(無向でも有向でもよい)
 * @return 深さ優先森
 */
std::pair<vertices_t, array_t> dfs(const graph_t& G)
{
    std::int32_t n = G.size();
    vertices_t vs(n);
    array_t f(n);
    weight_t time;

    // 再帰的に白頂点uを訪問する
    std::function <void(index_t)> visit = [&] (index_t u) {  // 白頂点を発見した...
        time = time + 1;             // timeを1進め、
        vs[u].d = time;              // timeの値を発見時刻u.dとして記録し、
        vs[u].color = color::gray;   // uを灰に彩色する
        // 各頂点v ∈ Adj[u]を吟味するので、深さ優先探索は辺(u, v)を探索する(explore)という
        for (auto& e : G[u]) {                  // uと隣接する各頂点vを調べ、
            index_t v = e.dst;
            if (vs[v].color == color::white) {  // vが白なら再帰的にvを訪問する
                vs[v].pi = u;
                visit(v);
            }
        }
        // uから出るすべての辺の探索が終了すると、
        vs[u].color = color::black;  // uを黒に彩色し、
        time = time + 1;             // timeを進め、
        f[u] = time;                 // 終了時刻をu.fに記録する
    };


    // スタックを用いて白頂点uを訪問する
    auto _visit = [&](index_t u) -> void {
        stack<index_t> S(n); S.push(u);
        time = time + 1;            // timeを1進め、
        vs[u].d = time;             // timeの値を発見時刻u.dとして記録し、
        vs[u].color = color::gray;  // uを灰に彩色する
        // 各頂点v ∈ Adj[u]を吟味するので、深さ優先探索は辺(u, v)を探索する(explore)という
        while (!S.empty()) {
            u = S.top();            // スタックの先頭の要素を取得
            std::size_t i = 0, m = G[u].size();  // 隣接リストの走査
            while (i < m && vs[G[u][i].dst].color != color::white) { i++; }
            if (i != m) {  // uの隣接リストの中でまだ調べていない頂点が存在する場合、
                index_t v = G[u][i].dst;
                S.push(v);                  // vをスタックにプッシュし、
                time = time + 1;            // timeを1進め、 
                vs[v].d = time;             // timeの値を発見時刻u.dとして記録し、
                vs[v].color = color::gray;  // vを灰に彩色する
            }
            else { // uの隣接リストを全て調べている場合、
                S.pop();                     // スタックから先頭の要素をポップし、
                time = time + 1;             // timeを進め、
                f[u] = time;                 // 終了時刻をu.fに記録する
                vs[u].color = color::black;  // uを黒に彩色する
            }
        }
    };
    

    for (auto& u : vs) {
        u.color = color::white;                // 頂点をすべて白に彩色し、
        u.pi    = graph::nil;                  // π属性をNILに初期化する
    }
    time = 0;                                  // 時刻カウンターを初期化
    for (auto u = 0; u < n; u++) {             // Vの各頂点を順番に調べ、
        if (vs[u].color == color::white) {     // 白頂点を発見すると、
            _visit(u);                         // visitを呼び出して訪問する
            // visitを呼び出すたびに、頂点uが深さ優先森の新しい木の根になる
        }
    }
    // DFSが終了した時、各頂点uには発見時刻(discovery time)u.dと終了時刻(finishing time)u.fが割り当てられている

    return std::make_pair(vs, f);
}


int main(void)
{
    const int vs = 6;
    graph_t G;
    G.resize(vs);

    int adjs[6][2] = { { 1, 3, }, { 4, }, { 4, 5, },
                       { 1, }, { 3, }, { 5,} };
    int adji[6] = { 2, 1, 2, 1, 1, 1, };

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
  
    auto dfsforest = dfs(G);
    for (int i = 0; i < vs; i++) {
        std::cout << dfsforest.first[i].d << " ";
        std::cout << dfsforest.second[i] << std::endl;
    }
    
    return 0;
}

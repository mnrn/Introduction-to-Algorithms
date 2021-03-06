/**
 * @brief  最小全域木問題(minimum-spanning tree problem)における
 *         Primのアルゴリズム(Prim's algorithm)の実装を行う
 * @date   作成日     : 2016/02/17
 * @date   最終更新日 : 2016/02/17
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <iostream>
#include "prim.hpp"
#include "../PriorityQueue/pqueue.hpp"



//****************************************
// 関数の定義
//****************************************



/**
 * @brief  Primのアルゴリズム
 *
 * @note   Primのアルゴリズムは、グラフの最短路を求めるDijkstraのアルゴリズムとほとんど同じように動作する
 *         Primのアルゴリズムは集合Aの辺が常に1つの木を形成するという性質を持つ. この木は任意の根rから開始し、Vの頂点全体を張るまで成長する
 *         各ステップでは、Aの頂点とある孤立点(Aの辺と接続していない頂点)を連結する軽い辺を木Aに加える
 *         Aに対して安全な辺だけがこの規則によってAに加えられるから、アルゴリズムが終了したとき、Aの辺は最小全域木を形成する
 *         各ステップでは木の重みの増加を限りなく小さく抑える辺を用いて木を成長させるので、これは貪欲戦略である
 *
 * @note   優先度付きキューの優先度更新を行わないため、優先度付きキューが空になるまでに行われる挿入の回数はΟ(E)であるが、
 *         EXTRACT-MIN呼び出し時に、黒頂点であれば無視をすることで、全体としての実行時間をΟ(ElgV)としている
 * 
 * @param  const graph_t& G グラフG
 * @param  index_t        r 最小全域木の根
 */
std::pair<edges_t, weight_t> prim(const graph_t& G, index_t r)
{
    std::int32_t n = G.size();
    std::size_t m = 0;
    for (auto& es : G) { m += es.size(); }
    //struct cmp { bool operator()(const edge& e, const edge& f) { return e.w > f.w; } };
    std::vector<std::int32_t> visited(n);
    edges_t A; weight_t w;

    
    for (index_t u = 0; u < n; u++) {
        visited[u] = false;  // 各頂点を白色に初期化
    }
    pqueue<edge> Q(m);
    Q.insert(edge(graph::nil, r, w = 0));  // 根rはキーを0に設定する
    while (!Q.empty()) {
        edge e = Q.extract();              // 軽い辺を取り出す
        index_t u = e.dst;
        if (visited[u]) { continue; }      // 取り出した辺が安全な辺ではない場合、再びループに戻り条件判定を行う
        
        for (auto&& f : G[u]) {            // uと隣接し、木に属さない各頂点vの更新を行う
            if (!visited[f.dst/*頂点v*/]) { Q.insert(f); }
        }
        visited[u] = true;                 // 頂点uを黒色に彩色し、
        w += e.w;                          // 最小重みを更新する
        if (e.src != graph::nil) {  // アルゴリズムが終了したとき、min優先度付きキューは空であり、
            A.emplace_back(e);      // Gに対する最小全域木AはA = { (v, v.π) : v ∈ V - { r } }である  
        }
    }
    return std::make_pair(A, w);
}


/**
 * @brief  Primのアルゴリズム
 *
 * @note   Primのアルゴリズムは、グラフの最短路を求めるDijkstraのアルゴリズムとほとんど同じように動作する
 *         Primのアルゴリズムは集合Aの辺が常に1つの木を形成するという性質を持つ. この木は任意の根rから開始し、Vの頂点全体を張るまで成長する
 *         各ステップでは、Aの頂点とある孤立点(Aの辺と接続していない頂点)を連結する軽い辺を木Aに加える
 *         Aに対して安全な辺だけがこの規則によってAに加えられるから、アルゴリズムが終了したとき、Aの辺は最小全域木を形成する
 *         各ステップでは木の重みの増加を限りなく小さく抑える辺を用いて木を成長させるので、これは貪欲戦略である
 *
 * @note   頂点vの属性v.keyはvと木に属するある頂点とを結ぶ辺の最小重みである. ただし、このような辺が存在しなければ、v.key = ∞と定義する
 *         属性v.πは木におけるvの親を示す
 *
 * @note   グラフG = (V, E)が隣接行列によって与えられたとき、Ο(V^2)で走るPrimのアルゴリズムは以下のように実現できる
 * 
 * @param  const matrix_t& W 隣接行列W
 * @param  index_t         r 最小全域木の根
 */
std::pair<vertices_t, weight_t> prim(const matrix_t& W, index_t r)
{
    std::int32_t n = W.size();
    vertices_t A(n); weight_t w;
    auto extractmin = [](const vertices_t& A, std::int32_t n) -> index_t {
        index_t u = graph::nil;
        for (index_t v = 0; v < n; v++) {
            u = (!A[v].visited && (u == graph::nil || A[v].key < A[u].key)) ? v : u;
        }
        return u;
    };

    
    
    for (auto& u : A) {
        u.key = graph::inf;  // 各頂点のキーを∞に設定する
        u.pi  = graph::nil;  // 各頂点の親をNILに設定する
        u.visited = false;
    }
    A[r].key = w = 0;       // 根rは例外で最初に処理されるようにキーの値を0に設定する
    while (true) {
        // 最小全域木Aに属さないある孤立点(Aの辺と接続していない頂点)を連結する軽い辺を探す
        index_t u = extractmin(A, n);
        if (u == graph::nil) { break; }  // 頂点uがNILを指すならば、探索は終了であり、最小全域木Aは A = {(v, v.π) : v ∈ V - { r } }である
        for (index_t v = 0; v < n; v++) {  // 頂点uの隣接行列の走査を行う
            if (W[u][v] < A[v].key) {      // 白または、灰頂点に対しては、属性の更新確認を行う必要がある
                A[v].pi  = u;              // vのπ属性と
                A[v].key = W[u][v];        // key属性を更新する
            }
        }
        A[u].visited = true;  // 辺(u, u.π)を最小全域木Aに加える(ただし、根rは例外)
        w += A[u].key;        // 最小重みを更新
    }
    return std::make_pair(A, w);
}



int main(void)
{
    const int n = 9;
    using namespace std;
    matrix_t M; M.resize(n);
    // graph_t G; G.resize(n);

    int adjmtx[n][n] = {
        // 0,  1,  2,  3,  4,  5,  6,  7,  8
        { -1,  4, -1, -1, -1, -1, -1,  8, -1, },  // 0
        {  4, -1,  8, -1, -1, -1, -1, 11, -1, },  // 1
        { -1,  8, -1,  7, -1,  4, -1, -1,  2, },  // 2
        { -1, -1,  7, -1,  9, 14, -1, -1, -1, },  // 3
        { -1, -1, -1,  9, -1, 10, -1, -1, -1, },  // 4
        { -1, -1,  4, 14, 10, -1,  2, -1, -1, },  // 5
        { -1, -1, -1, -1, -1,  2, -1,  1,  6, },  // 6
        {  8, 11, -1, -1, -1, -1,  1, -1,  7, },  // 7
        { -1, -1,  2, -1, -1, -1,  6,  7, -1, },  // 8
    };

    // for (int i = 0; i < n; i++) {
    //     M[i].resize(n);
    //     for (int j = 0; j < n; j++) {
    //         int k = adjmtx[i][j];
    //         if (k == -1) k = graph::inf;
    //         M[i][j] = k;
    //     }
    // }

    for (int i = 0; i < n; i++) {
        M[i].resize(n);
        for (int j = 0; j < n; j++) {
            int k = adjmtx[i][j];
            if (k == -1) k = graph::inf;
            M[i][j] = k;
        }
    }

    auto mst = prim(M);
    cout << mst.second << endl;
    
    return 0;
}



/**
 * @brief  Primのアルゴリズムの詳細
 *
 * @note   Primのアルゴリズムは、グラフの最短路を求めるDijkstraのアルゴリズムとほとんど同じように動作する
 *         Primのアルゴリズムは集合Aの辺が常に1つの木を形成するという性質を持つ. この木は任意の根rから開始し、Vの頂点全体を張るまで成長する
 *         各ステップでは、Aの頂点とある孤立点(Aの辺と接続していない頂点)を連結する軽い辺を木Aに加える
 *         Aに対して安全な辺だけがこの規則によってAに加えられるから、アルゴリズムが終了したとき、Aの辺は最小全域木を形成する
 *         各ステップでは木の重みの増加を限りなく小さく抑える辺を用いて木を成長させるので、これは貪欲戦略である
 *
 * @note   Primのアルゴリズムを効率よく実現するには、Aの辺から形成される木に加える新たな辺を簡単に選択できなければならない
 *         以下の疑似コードでは、連結グラフGと成長させる最小全域木の根rがアルゴリズムへの入力である
 *         アルゴリズムの実行中、木が含ま「ない」すべての頂点をkey属性に基づくmin優先度付きキューQに置く
 *         頂点vの属性v.keyはvと木に属するある頂点とを結ぶ辺の最小重みである. ただし、このような辺が存在しなければ、v.key = ∞と定義する
 *         属性v.πは木におけるvの親を示す. アルゴリズムの実行中、集合Aは(明示しないが)
 *           A = { (v, v.π) : v ∈ V - { r } - Q }
 *         として維持される. アルゴリズムが終了したときmin優先度付きキューQは空であり、Gに対する最小全域木Aは
 *           A = { (v, v.π) : v ∈ V - { r } }
 *         である
 *
 * @note   MST-PRIM(G, w, r)
 *         1 for each u ∈ G.V
 *         2     v.key = ∞
 *         3     v.π = NIL
 *         4 r.key = 0
 *         5 Q = G.V
 *         6 while Q != φ
 *         7 u = EXTRACT-MIN(Q)
 *         8 for each u ∈ G.Adj[u]
 *         9     if v ∈ Q and w(u, v) < v.key
 *        10         v.π = u
 *        11         v.key = w(u, v)
 *
 * @note   第1~5行で各頂点のキーを∞に設定する. また、各頂点の親をNILに設定し、min優先度付きキューQにすべての頂点を挿入して初期化する
 *         アルゴリズムは3つの部分からなるループ不変式を維持する
 *
 *         第6~11行のwhile文の繰り返しが開始される直前ではつぎの3つの性質が成立する
 *           1. A = { (v, v.π) : v ∈ V - { r } - Q } である
 *           2. 最小全域木の中ですでに置かれた頂点はV - Qに属する
 *           3. すべての頂点v ∈ Qに対して、v.π != NILならば、
 *              v.key < ∞ かつ v.keyはvと最小全域木にすでに置かれた頂点とを結ぶ軽い辺(v, v.π)の重みである
 *
 * @note   第7行ではカット(V - Q, Q)と交差する軽い辺に接続する頂点u ∈ Qを識別する(最初の繰り返しだけは例外であり、第4行からu = rである)
 *         Qからのuの削除は木の頂点集合V - Qへのuの挿入であり、Aへの(u, u.π)の挿入である. 第8~11行のfor文では、uと隣接し、
 *         木に属さない各頂点vのkey属性とπ属性を更新する. これによってループ不変式の3番目の性質が保存される
 *
 * @note   Primのアルゴリズムの実行時間はmin優先度付きキューQの実現方法に依存する
 *         Qを2分minヒープとして実現すれば、手続きBUILD-MIN-HEAPを利用して第1~5の初期化をΟ(V)時間で実行できる
 *         while文の本体は|V|回繰り返され、各EXTRACT-MIN操作にはΟ(lgV)時間かかるから、EXTRACT-MINの呼び出しにかかる総時間はΟ(VlgV)である
 *         全隣接リストの長さの合計は2|E|だから、第8~11行のfor文は合計Ο(E)回繰り返される
 *         このfor文の中で第9行が行うQに関する所属テスト(与えられた頂点がQに属するか否かの判定)は、各頂点についてQに属するか否かを表示するための
 *         1ビットを用意し、頂点をQから削除したときにそのビットを更新すれば、定数時間で実行できる
 *         第11行の代入文にはminヒープに対するDECREASE-KEY操作が暗黙のうちに含まれている. そしてこの操作は2分ヒープ上でΟ(lgV)時間で実行できる
 *         したがって、Primのアルゴリズムで要する総時間はΟ(VlgV + ElgV) = Ο(ElgV)となり、これはKruskalのアルゴリズムの実現と漸近的に等しい
 *
 * @note   フィボナッチヒープを用いるとPrimのアルゴリズムの漸近的な実行時間を改善できる. |V|個の要素をフィボナッチヒープが蓄えられているとき、
 *         EXTRACT-MIN操作をΟ(lgV)ならし時間、DECREASE-KEY操作をΟ(1)ならし時間で実行できる
 *         したがって、min優先度付きキューQをフィボナッチヒープを用いて実現すれば、Primのアルゴリズムの実行時間をΟ(E + VlgV)に改善できる
 *
 * @note   入力グラフG = (V, E)が疎、すなわち|E| = Θ(V)ならば、
 *         Primのアルゴリズムのフィボナッチヒープによる実現および、2分ヒープによる実現の総実行時間は、
 *           2分ヒープ         : Ο(ElgV) = Ο(VlgV)
 *           フィボナッチヒープ : Ο(E + VlgV) = Ο(VlgV)
 *         グラフが密、すなわち|E| = Θ(V^2)ならば、
 *           2分ヒープ         : Ο(ElgV) = Ο((V^2)lgV)
 *           フィボナッチヒープ : Ο(E + VlgV) = Ο(V^2)
 *         また、|E| = ω(V)のとき、|E| = Ο(VlgV)ゆえ、Ο(E + VlgV) = Ο(VlgV)であるが、|E| = ω(V)ゆえ、Ο(ElgV) = ω(VlgV)である.
 *         したがって、|E| = ω(VlgV)のとき、フィボナッチヒープによる実現は2分ヒープによる実現に漸近的に速くなる
 *
 */



/**
 * @brief   おまけ...boostのサイトに存在するPrimのアルゴリズムに対する疑似コード
 *
 * @details PRIM-MST(G, s, w)
 *          1  for each vertex u in V[G]
 *          2      color[u] := WHITE
 *          3      d[u] := INFINITY
 *          4  color[s] := GRAY
 *          5  d[s] := 0
 *          6  ENQUEUE(PQ, s)
 *          7  p[s] := s
 *          8  while (PQ != φ)
 *          9      u := DEQUEUE(PQ)
 *         10      for each v in Adj[u]
 *         11          if (w(u, v) < d[v])
 *         12              d[v] := w(u, v)
 *         13              p[v] := u
 *         14              if (color[v] = WHITE)
 *         15                  ENQUEUE(PQ, v)
 *         16                  color[v] := GRAY
 *         17              else if (color[v] = GRAY)
 *         18                  UPDATE(PQ, v)
 *         19          else
 *         20              do nothing
 *         21      end for
 *         22      color[u] := BLACK
 *         23  end while
 *         24  return (p, d)
 *
 * @note   上記の疑似コードは比較的わかりやすく、重要な部分を含んでいる
 *         問題は、18行目のUPDATE呼び出しである. これは頂点vが灰色に彩色されている場合のみ起こりうる
 *         UPDATEをサポートしていない優先度付きキュー(priority-queue)を用いる場合、14~18行目の操作は、
 *         ex1    if (color[v] != BLACK)
 *         ex2        ENQUEUE(PQ, v)
 *         として構わない. なぜなら、優先度順にDEQUEUEされるため、UPDATE前にENQUEUEした頂点が
 *         黒に彩色されたのち、DEQUEUEされるが、黒に彩色されているため、ex1行のif文の条件を満たさない
 *         そのため、DEQUEUEされたのち、そのまま8行目のwhile文の条件判定を行う
 *         この場合、優先度付きキューが空になるまでに挿入される頂点の数はΟ(E)であると考えられるため、
 *         第8行目のループの回数もΟ(E)であり、PRIM-MSTの総実行時間はΟ(ElgE)となることがわかる
 */


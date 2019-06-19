/**
 * @brief 幅優先探索のテスト
 * @date  作成日     : 2016/02/13
 * @date  最終更新日 : 2016/02/13
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
#include <cassert>



//****************************************
// 構造体の定義
//****************************************

using weight_t = std::int32_t;  /**< 辺(u, v)への重みwを表す型 */
using index_t  = std::int32_t;  /**< 頂点vの添字を表す型       */

/**
 * @brief  頂点色列挙構造体(scoped enum)
 * @detail (u, v) ∈ Eで頂点uが黒ならば頂点vは灰か黒である
 *         すなわち、黒頂点に隣接する全ての頂点は発見済みである
 *         灰頂点は白頂点に隣接することがあり、これらの頂点が既発見頂点と未発見頂点の境界をなす
 */
enum struct color : std::int32_t {
    white,  /**< 未発見頂点 */
    black,  /**< 既発見頂点 */
    gray,   /**< 既見済頂点 */
};

/**
 * @brief グラフ用ノード(頂点) 
 */
struct vertex {
    union {
        weight_t d;       /**< 始点sからの距離  */
        weight_t key;     /**< Primのアルゴリズムにおいて木に属するある頂点とを結ぶ重み */
    };
    index_t pi;           /**< 先行頂点(の添字) */
    union {
        color _color;     /**< 頂点の色        */
        bool  visited;    /**< 発見済みか?     */
    };
    weight_t f;           /**< 終了時刻印(DFSにおいて、黒色に彩色されたとき、刻まれる)     */
    vertex() : d(0), pi(0), _color(color::white), f(0) {}
};

/**
 * @brief グラフ用エッジ(辺)
 * @note  G = (V, E)を重み関数wを持つ重み付きグラフとすると、
 *        辺(u, v) ∈ Eの重みはw(u, v)と表される
 */
struct edge {
    index_t  src;   /**< 辺の始点u */
    index_t  dst;   /**< 辺の終点v */
    weight_t w;     /**< 辺(u, v)への重み(コスト) */
    edge() = default;
    edge(index_t src, index_t dst) : src(src), dst(dst), w(1) {}
    edge(index_t src, index_t dst, weight_t w) : src(src), dst(dst), w(w) {}

    bool operator < (const edge& e) const { return w < e.w; }
    bool operator > (const edge& e) const { return w > e.w; }
};



using edges_t    = std::vector<edge>;      /**< グラフG=(V, E)の辺集合E   */
using vertices_t = std::vector<vertex>;    /**< グラフG=(V, E)の頂点集合V */
using array_t    = std::vector<weight_t>;  /**< 重みwの配列  */
using indices_t  = std::vector<index_t>;   /**< 頂点の添字配列 */
using adjmtx_t   = std::vector<array_t>;   /**< グラフGの隣接行列表現 */
using graph_t    = std::vector<edges_t>;   /**< グラフGの隣接リスト表現(こちらを主に使用する) */



namespace graph {  // グラフ用名前空間...そのうちこのファイルを覆い尽くす予定
    enum {  // namespace graphによってscopedではあるが強く型付けされたenum(strongly-typed enum)ではない
    inf = std::numeric_limits<weight_t>::max() / 3,  /**< @brief 辺が存在しない場合に使用される値     */
    nil = std::numeric_limits<index_t>::min() / 3,   /**< @brief 先行点が存在しない場合に使用される値 */
};
}



/**
 * @brief placement newを使用した生成用ヘルパ関数
 */
template <class T, class... Args>
void construct(T& t, Args&&... args)
{
    new(&t)T(std::forward<Args>(args)...);
}

/**
 * @brief 明示的にデストラクタを呼び出す破棄用ヘルパ関数
 */
template <class T>
void destroy(const T& t) noexcept
{
    t.~T();
}


/**
 * @brief キュー
 * @tparam class T         キューの要素の型
 * @tparam class Allocator Tのアロケータ 
 */
template <class T>
struct queue {
    // std::vector<T, Allocator> Q;
    std::int32_t head, tail;
    std::int32_t _max;
    T* Q;
    
    explicit queue(std::int32_t n) 
    : head(0), tail(0), _max(n+1), Q(static_cast<T*>(::operator new(sizeof(T) * (n + 1))))  { };
    ~queue() noexcept
    {
        if (head < tail) {
            std::for_each(Q + head, Q + tail, destroy<T>);
        }
        else if (head > tail) {
            std::for_each(Q, Q + tail, destroy<T>);
            std::for_each(Q + head, Q + _max, destroy<T>);
        }
        ::operator delete(Q);
    };

    /**< @brief キューが空かどうか判定 */
    bool empty()
    {
        return head == tail;
    }

    /**< @brief キューが満杯かどうか判定 */
    bool full()
    {
        return ((tail + 1) % _max == head);
    }

    /**< @brief キューに要素xを格納する */
    template<class... Args>
    void enqueue(Args&&... args)
    {
        assert(!full());        // オーバーフローチェック
        construct(Q[tail], T(std::forward<Args>(args)...));
        tail = (tail + 1) % _max;  // 循環処理
    }

    /**< @brief キューから一番上の要素を取り出す */
    T dequeue()
    {
        assert(!empty());       // アンダーフローチェック
        T x = Q[head];
        destroy(Q[head]);
        head = (head + 1) % _max;  // 循環処理
        return x;
    }

    /**< @brief キューの一番上の要素を返す */
    T top()
    {
        return Q[head];
    }
};



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
        u._color = color::white;    // uを白に彩色し、
        u.d     = graph::inf;       // u.dを無限大に設定し、
        u.pi    = graph::nil;       // uの親をNILに設定する
    }
    // 手続き開始と同時に始点sを発見すると考え、
    V[s]._color = color::gray;      // 始点sを灰色に彩色する 
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
            if (V[v]._color == color::white) {   // vが白ならvは未発見である
                V[v]._color = color::gray;       // vを灰色に彩色し、
                V[v].d     = V[u].d + 1;         // 距離v.dをu.d+1に設定し、
                V[v].pi    = u;                  // uをその親v.piとして記録し、
                Q.enqueue(v);                    // vをキューQの末尾に置く
            }
        }
        V[u]._color = color::black;  // uの隣接リストに属するすべての頂点の探索が完了すると、この頂点を黒に彩色する
    }
    // ある頂点を灰に彩色したときには、この頂点をQへ挿入し、ある頂点をQから削除したときには、この頂点を黒に彩色するので、
    // ループ不変式が保存される

    return V;
}




int main()
{
    graph_t G;
    int u, k, v, n;
    std::cin >> n;
    G.resize(n);
    for (int i = 0; i < n; i++) {
        std::cin >> u >> k;
        u--;
        G[i].resize(k);
        for (int j = 0; j < k; j++) {
            std::cin >> v;
            v--;
            G[i].emplace_back(i, v);
        }
    }

    auto bfstree = bfs(G, 0);
    for (int i = 0; i < n; i++) {
        std::cout << i + 1 << " "
                  << ((bfstree[i].d == graph::inf)
                      ? -1 : bfstree[i].d) << std::endl;
    }
    return 0;
}


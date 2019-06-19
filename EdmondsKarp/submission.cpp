/**
 * @brief  最大フローを求めるEdmondsKarpアルゴリズムを扱う
 * @date   2016/03/03
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <iostream>
#include <utility>
#include <algorithm>
#include <vector>
#include <type_traits>
#include <cassert>



//****************************************
// 構造体の定義
//****************************************



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
        if (!std::is_trivially_destructible<T>::value) {
            if (head < tail) {
                std::for_each(Q + head, Q + tail, destroy<T>);
            }
            else if (head > tail) {
                std::for_each(Q, Q + tail, destroy<T>);
                std::for_each(Q + head, Q + _max, destroy<T>);
            }
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



using weight_t   = std::int32_t;  /**< 辺(u, v)への重みwを表す型 */
using index_t    = std::int32_t;  /**< 頂点vの添字を表す型       */
using capacity_t = weight_t;      /**< 辺(u, v)の容量を表す型    */

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
        color _color;      /**< 頂点の色        */
        bool  visited;    /**< 発見済みか?     */
    };
    // weight_t f;           /**< 終了時刻印(DFSにおいて、黒色に彩色されたとき、刻まれる)     */
    vertex() : d(0), pi(0), _color(color::white)/*, f(0)*/ {}
};

/**
 * @brief グラフ用エッジ(辺)
 * @note  G = (V, E)を重み関数wを持つ重み付きグラフとすると、
 *        辺(u, v) ∈ Eの重みはw(u, v)と表される
 */
struct edge {
    union {
        index_t  src;   /**< 辺の始点u */
    };
    index_t  dst;       /**< 辺の終点v */
    union {
        weight_t w;     /**< 辺(u, v)への重み(コスト) */
        capacity_t c;   /**< 辺(u, v)の容量 */
    };
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
using matrix_t   = std::vector<array_t>;   /**< グラフGの隣接行列表現(および表行列表現) */
using graph_t    = std::vector<edges_t>;   /**< グラフGの隣接リスト表現(こちらを主に使用する) */



namespace graph {  // グラフ用名前空間...そのうちこのファイルを覆い尽くす予定
    enum {  // namespace graphによってscopedではあるが強く型付けされたenum(strongly-typed enum)ではない
    inf = std::numeric_limits<weight_t>::max() / 3,  /**< @brief 辺が存在しない場合に使用される値     */
    nil = std::numeric_limits<index_t>::min() / 3,   /**< @brief 先行点が存在しない場合に使用される値 */
};
}



/**
 * @brief Edmonds-Karpのアルゴリズム
 * @note  Ford-Fulkerson法において、増加可能経路を幅優先探索を用いて探索することでFORD-FULKERSONの計算時間の上界を改善できる
 *        すなわち、残余ネットワークの中でsとtを結ぶ最短路を増加可能経路として選択するのである. ただし、残余ネットワークの各辺(u, v)の距離(重み)は1である
 *        Ford-Fulkerson法をこのように実現したものをEdmonds-Karpアルゴリズム(Edmonds-Karp algorithm)と呼ぶ
 */
struct edmondskarp {
    using bool_t = std::int32_t;  /**< ブール値は整数型で、{ false 0 true それ以外 }とする  */
    indices_t pi;                 /**< 頂点vの先行点属性 */
    std::vector<bool_t> visited;  /**< すでに訪問済みか？ */
    matrix_t c, f;                /**< 辺(u, v) ∈ Eの容量属性(u, v).cとフロー属性(u, v).f */
    std::vector<indices_t> Gf;    /**< 残余ネットワークGf */
    std::int32_t n;               /**< 頂点v ∈ Vの数 */

    explicit edmondskarp(std::size_t size) :
        pi(size), visited(size), c(size, array_t(size)), f(size, array_t(size)),
        Gf(size, indices_t()), n(size) { }
    explicit edmondskarp(const graph_t& G)
    {
        *this = edmondskarp(G.size());
        for (index_t i = 0; i < n; i++) { for (auto& e : G[i]) { addedge(e.src, e.dst, e.c); } }
    }

    /**
     * @brief  容量cおよびフローfの初期化と残余ネットワークGfの生成部分
     * @note   入口sと出口tを持つフローネットワークをG = (V, E)とする. fをGのフローとし、頂点対u, v ∈ Vを考える
     *         (u, v)の残余容量(residul capacity)cf(u, v)を
     *           cf(u, v) = { c(u, v) - f(u, v)  (u, v) ∈ Eのとき
     *                        f(v, u)            (v, u) ∈ Eのとき
     *                        0                  それ以外         }
     *         と定義する. 定義から(u, v) ∈ Eならば、(v, u) !∈ Eだから、任意の頂点対に対して上記の式のちょうど1つの場合が対応する
     *
     *         フローネットワークG = (V, E)とフローfが与えられたとき、fによって誘導される残余ネットワーク(residual network)は
     *           Ef = { (u, v) ∈ V x V : cf(u, v) > 0 }
     *         によって定義されるGf = (V, Ef)である
     *         残余ネットワークの各辺、すなわち残余辺(residual edge)には正のフローを流すことができる
     *         Efの辺はEの辺かその逆向きの辺であり、したがって |Ef| <= 2|E|である
     */
    void addedge(index_t u, index_t v, capacity_t cap)
    {
        c[u][v] = cap; f[u][v] = 0;              // cおよびfの初期化
        c[v][u] = 0;   f[v][u] = 0;
        Gf[u].push_back(v); Gf[v].push_back(u);  // Gfの生成
    }

    /**
     * @brief  Edmonds-Karpのアルゴリズムを実行する
     * @note   アルゴリズムの実行時間はΟ(VE^2)であるが、コンストラクタで隣接行列を生成しているので全体の実行時間はΟ(VE^2 + V^2)
     * @param  index_t s フローネットワークの入口(source) s
     * @param  index_t t フローネットワークの出口(sink) t
     * @return フローネットワークの最大フロー
     */
    capacity_t execute(index_t s, index_t t)
    {
        capacity_t flow = 0;  // execute終了時にsからtへの最大フローとなる値

        // NOTE: コンストラクタ呼び出し時にFORD-FULKERSONの第1~3行の初期化と同様の操作は終了している
        
        while (bfs(s, t)) { flow += path(s, t); }  // BFSでpを探し、pが存在したならば、フローを更新する
        return flow;
    }

    /**
     * @brief  幅優先探索を用いて残余ネットワークGfにsからtへの道(増加可能経路(augment path))pを探索する
     * @param  index_t s フローネットワークの入口(source) s
     * @param  index_t s フローネットワークの出口(sink) t
     * @return 増加可能経路pが存在するか否か
     */
    bool_t bfs(index_t s, index_t t)
    {
        pi.assign(n, graph::nil);
        visited.assign(n, false);
        queue<index_t> Q(n);

        // 手続き開始と同時に始点sを発見したと考え、
        pi[s] = graph::nil;  // 始点の先行点をNILで初期化する
        visited[s] = true;   // 訪問印を刻む
        
        Q.enqueue(s);  // sだけを含むようにキューを初期化する
        while (!Q.empty()) {
            index_t u = Q.dequeue();
            for (auto& v : Gf[u]) {
                // vが白でない、または残余容量がゼロならば、辺(u, v)を調べる必要がない
                if (visited[v] || cf(u, v) == 0) { continue; }
                
                // 上記の条件にいずれも当てはまらない場合、
                visited[v] = true;  // 訪問印を刻み、
                pi[v] = u;          // uをその親v.piとして記録し、
                Q.enqueue(v);       // vをキューQの末尾に置く
            }
        }
        return visited[t];  // 最終的な結果は出口節点tを訪問するか、pが空のどちらかである
    }

    /**
     * @brief  増加可能経路pに沿ってフローfを残余容量cf(p)だけ増やす
     * @param  index_t s フローネットワークの入口(source) s
     * @param  index_t t フローネットワークの出口(sink) t
     * @return capacity_t cf_p
     */
    capacity_t path(index_t s, index_t t)
    {
        capacity_t cf_p = graph::inf;
        index_t v = t;

        // フローを増やす量を決定する. sinkからsourceへと計算した経路で最小のものと等しい
        while (v != s) {
            cf_p = std::min(cf_p, cf(pi[v], v));
            v = pi[v];
        }

        // 経路上で得られた最小値分だけ増加させる
        v = t;
        while (v != s) {
            f[pi[v]][v] += cf_p;  // 元のネットワークの辺(前方辺)のフローを加え、
            f[v][pi[v]] -= cf_p;  // 逆向き辺(後方辺)のフローを引く
            v = pi[v];
        }
        
        return cf_p;
    }

    /**< @brief 頂点対(u, v) ∈ Vにおける残余容量cf(u, v)を返す */
    capacity_t cf(index_t u, index_t v)
    {
        return c[u][v] - f[u][v];
    }
};




int main(void)
{
    using namespace std;
    int V, E;

    cin >> V >> E;
    
    graph_t G(V);
    for (int i = 0; i < E; i++) {
        int a, b, c;
        cin >> a >> b >> c;
        G[a].emplace_back(a, b, c);
    }
    edmondskarp ek(G);

    // ek.debug();
    auto max_flow = ek.execute(0,V-1);

    cout << max_flow << endl;

    return 0;
}

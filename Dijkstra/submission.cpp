/**
 * @brief  最短経路問題(shortest paths problem)におけるDijkstraのアルゴリズム(Dijkstra's algorithm)を扱う
 * @date   作成日     : 2016/02/20
 * @date   最終更新日 : 2016/02/20
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <iostream>
#include <utility>
#include <algorithm>
#include <vector>
#include <cassert>
#include <functional>



//****************************************
// 構造体の定義
//****************************************

/**
 * @brief  2分ヒープ
 * @tparam class T         ヒープ木の要素の型(copyおよびmove可能であることを要求する)
 * @tparam class Compare   比較用関数オブジェクト(デフォルトでmaxヒープを構築する)
 * @tparam class Allocator アロケータ
 */
template <class T,
          class Compare   = std::less<T>,
          class Allocator = std::allocator<T>>
struct binheap {

    std::size_t size;             /**< 配列Aに格納されているヒープの要素数 */
    std::size_t length;           /**< 配列Aが含む要素数 */
    std::vector<T, Allocator> A;  /**< およそ完全2分木とみなせる配列A */
    Compare cmp;                  /**< 比較述語 */

    explicit binheap(std::size_t n) : size(0), length(n), A(n) {}
    template <class Iterator>
    binheap(Iterator a0, std::size_t n) : size(0), length(n), A(n)
    {
        std::copy(a0, a0 + n, A.begin());  // A[0..n-1]にコピー
    }
    template <class Iterator>
    binheap(Iterator a0, Iterator aN) : size(0)
    {
        typedef typename std::iterator_traits<Iterator>::difference_type dif_t;
        dif_t n = std::distance(a0, aN);
        length = static_cast<std::size_t>(n);
        A.resize(length);                  // Aの記憶領域確保
        std::copy(a0, aN, A.begin());      // A[0..n-1]にコピー
    }
    explicit binheap(const std::vector<T>&& v)
        : size(0), length(v.size()), A(std::move(v)) /* A[0..n-1]にムーブ */ {}

    

    /**
     * @brief max(min)ヒープ条件を回復する
     * @note  heapifyはA[i]の値をmax(min)ヒープの中に”滑り落とし”、
     *        添字iを根とする部分木がmax(min)ヒープ条件を満たすようにする
     * @note  実行時間はΟ(lgn)
     * @param std::size_t i ヒープ木の節点i
     */
    void heapify(std::size_t i)
    {
        std::size_t l = left(i);   // 節点iの左の子を取り出す
        std::size_t r = right(i);  // 節点iの右の子を取り出す

        // A[i], A[l], A[r]の中から最大値または最小値の添字を格納する
        std::size_t lhs = i;
        if (l < size && cmp(A[lhs], A[l])) {
            lhs = l;
        }
        if (r < size && cmp(A[lhs], A[r])) {
            lhs = r;
        }
        // lhsが左右の子であるか判定
        if (lhs != i) {  // 左右の子であるならば、
            std::swap(A[i], A[lhs]);  // A[i]とA[lhs]を交換する
            
            // lhsを根とする部分木がmax(min)ヒープ条件に違反する可能性がある
            heapify(lhs);  // そこで、max(min)ヒープ条件を回復するため再帰呼び出しを行う
        }
    }

    /**
     * @brief max(min)ヒープの構築を行う
     * @note  nをA.lengthとする.ボトムアップ的に手続きheapifyを利用することで
     *        配列A[0..n-1]をmax(min)ヒープに変換できる.部分配列A[floor(n/2)..n-1]の
     *        要素は全てこの木の葉であり、それぞれ1要素だけからなるヒープである
     *        手続きbuildは木の残りの節点に進み、それぞれの節点においてheapifyを実行する
     * @note  実行時間はΟ(n)
     */
    void build()
    {
        size = length;
        // for文の各繰り返しが開始される時点では、
        // 各節点i,i+1,..n-1はあるmax(min)ヒープの根である
        for (std::int32_t i = length / 2;
             i >= 0;     // i = -1で手続き終了
             --i) {      // iを1減らすことで次の繰り返しに対するループ不変式が再び成立する
            heapify(i);  // 節点iをmax(min)ヒープの根に修正する
            // また、このheapify呼び出しは節点i,i+1,..,n-1がmax(min)ヒープの根であるという性質を保存する
        }
        // 手続きはi = -1で終了する.ループ不変式から、各節点0,1,..n-1はmax(min)ヒープの根である
        // 特に節点0はmax(min)ヒープの根である
    }

    /**
     * @brief ヒープソート
     * @note  n = A.lengthとする.ヒープソートアルゴリズムは以下のように働く
     *
     *        最初に、buildを用いて入力配列A[0..n-1]上でmax(min)ヒープを構築する
     *        配列の最大(最小)要素は根A[0]に格納されているので、A[n-1]と交換することで
     *        最大(最小)要素を正しい最終位置に置く.次に、A.heap-sizeを1だけ減らして
     *        ヒープから節点n-1を削除する.このとき、根の左右の子はともにmax(min)ヒープを形成するが、
     *        更新された根はmax(min)ヒープ条件に違反する可能性がある.そこで、heapify(0)を
     *        呼び出してmax(min)ヒープ条件を修復すると、A[0..(n-2)]上にmax(min)ヒープが構築される
     *        この操作をサイズn-1のmax(min)ヒープからサイズ2のヒープまで繰り返す
     *
     * @note  最悪実行時間はΩ(nlgn)
     */
    void sort()
    {
        // 最初に、buildを用いて入力配列A[0..n-1]上でmax(min)ヒープを構築する
        build();
        // for文の各繰り返しの直前では、部分配列A[0..i]はA[0..n-1]の小さい(大きい)ほうから
        // i個の要素を含むmax(min)ヒープであり、部分配列A[i+1..n-1]はA[0..n-1]の大きい(小さい)ほうから
        // n - i個の要素をソートされた順序で含む
        for (std::size_t i = length - 1; i > 0; --i) {
            // 最大(最小)要素を正しい最終位置に置く
            std::swap(A[0], A[i]);
            // A.heap-sizeを1だけ減らしてヒープから節点iを削除する
            size = size - 1;
            // 更新された根はmax(min)ヒープ条件に違反する可能性があるので修復する
            heapify(0);
        }
    }

    /**
     * @brief []演算子オーバーオード
     *        配列Aに関する参照を返します
     * @param std::size_t i 配列インデックス
     */
    T& operator[](std::size_t i)
    {
        return A[i];
    }


    //**********************************************************************
    // 優先度付きキューに関する実装
    // 優先度付きキューはキーと呼ぶ値を持つ要素の集合Sを管理するためのデータ構造である
    //**********************************************************************

    /**
     * @brief Sから最大または最小のキーを持つSの要素を削除し、その要素を返す
     * @note  実行時間はΟ(lgn)
     */
    T extract()
    {
        assert(size > 0);    // ヒープアンダーフローチェック
        T root = A[0];       // 根を取り出す
        A[0] = A[size - 1];  // 根を更新
        size = size - 1;     // ヒープサイズの更新
        heapify(0);          // max(min)ヒープの再構築
        return root;         // 最大または最小のキーを持つ要素を返す
    }

    /**
     * @brief 要素iのキーの値を新しいキー値keyに変更する
     * @note  ただしmaxヒープにおいてkey > A[i],
     *        minヒープにおいてkey < A[i]を仮定する
     *        実行時間はΟ(lgn)
     * @param std::size_t i 節点i
     * @param T key         節点iの新しいキー値
     * @param bool check    キーの値をチェックするか？
     */
    void update(std::size_t i, const T& key, bool check = true)
    {   
        // 新しいキーは現在のヒープにふさわしいか判定する
        if (check && cmp(key, A[i])) {
            return;
        }
        A[i] = key;  // キー値A[i]を新しい値に変更

        // 節点から根に至る道を辿る
        // 要素のキー値が親より小さくなる(大きくなる)とmax(min)ヒープ条件が成立し,
        // 手続きを終了する
        while (i > 0 && cmp(A[parent(i)], A[i])) {
            std::swap(A[i], A[parent(i)]);  // 親のキー値との交換
            i = parent(i);                  // 根に至る道を辿る
        }
    }

    /**
     * @brief 集合Sに要素xを挿入する
     * @note  実行時間はΟ(lgn)
     * @param T key 要素x
     */
    void insert(const T& key)
    {
        assert(size < length);         // ヒープオーバーフローチェック
        size = size + 1;               // A.heap-sizeを1つ増やす
        update(size - 1, key, false);  // A[size]のキー値を変更する
    }

    /**
     * @brief ヒープから節点iの要素を削除する
     * @note  実行時間はΟ(lgn)
     * @param std::size_t i 節点i
     */
    void erase(std::size_t i)
    {
        assert(size > 0);    // ヒープアンダーフローチェック
        A[i] = A[size-1];    // A[i]にA[size-1]を代入
        size = size - 1;     // A.heap-sizeを1つ減らす
        heapify(i);          // max(min)ヒープの再構築
    }

    
private:
    /**< @brief 節点の添字iから親を示す添字を計算して返す */
    constexpr std::size_t parent(std::size_t i) const noexcept {
        return ((i - 1) >> 1);
    }
    /**< @brief 節点の添字iから左の子を示す添字を計算して返す*/
    constexpr std::size_t left(std::size_t i) const noexcept {
        return ((i << 1) + 1);
    }
    /**< @brief 節点の添字iから右の子を示す添字を計算して返す*/
    constexpr std::size_t right(std::size_t i) const noexcept {
        return ((i + 1) << 1);
    }
};





/**
 * @brief  優先度付きキュー
 * @tparam class T         優先度付きキューに格納する要素の型
 * @tparam class Compare   比較用関数オブジェクト(デフォルトでminヒープを構築する)
 * @tparam class Allocator アロケータ
 */
template <class T,
          class Compare   = std::greater<T>,
          class Allocator = std::allocator<T>
          >
struct pqueue {
    binheap<T, Compare, Allocator> H;

    explicit pqueue(std::size_t size) : H(size) { }
    explicit pqueue(const std::vector<T>&& v) : H(std::move(v))
    {
        H.build();  // ヒープを構築
    }

    /**
     * @brief 集合Sに要素xを挿入する
     * @note  実行時間はΟ(lgn)
     * @param T key 要素x
     */
    void insert(const T& key)
    {
        H.insert(key);
    }

    /**
     * @brief 要素iのキーの値を新しいキー値keyに変更する
     * @note  ただしmaxヒープにおいてkey > A[i],
     *        minヒープにおいてkey < A[i]を仮定する
     *        実行時間はΟ(lgn)
     * @param std::size_t i 節点i
     * @param T key         節点iの新しいキー値
     */
    void update(std::size_t i, const T& key)
    {
        H.update(i, key);
    }
    
    /**
     * @brief Sから最大または最小のキーを持つSの要素を削除し、その要素を返す
     * @note  実行時間はΟ(lgn)
     */
    T extract()
    {
        return H.extract();
    }

     /**
     * @brief ヒープから節点iの要素を削除する
     * @note  実行時間はΟ(lgn)
     * @param std::size_t i 節点i
     */
    void erase(std::size_t i)
    {
        H.erase(i);
    }
    
    /**
     * @brief []演算子オーバーオード
     *        配列Aに関する参照を返します
     * @param std::size_t i 配列インデックス
     */
    T& operator[](std::size_t i)
    {
        return H[i];
    }

    /**
     * @brief キューが空かどうかを返す
     */
    bool empty()
    {
        return H.size == 0;
    }

    template<class... Args>
    void emplace(Args&&... args)
    {
        assert(H.size < H.length);
        H.size = H.size + 1;
        H.update(H.size - 1, T(std::forward<Args>(args)...), false);
    }
};






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
        color _color;      /**< 頂点の色        */
        bool  visited;    /**< 発見済みか?     */
    };
    weight_t f;           /**< 時刻印(DFSにおいて、黒色に彩色されたとき、刻まれる)     */
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



//****************************************
// 関数の定義
//****************************************

/**
 * @brief  すべての辺重みが非負であるという仮定の下で、Dijkstra(ダイクストラ)のアルゴリズム(Dijkstra's algorithm)は
 *         重み付き有向グラフG = (V, E)上の単一始点最短路問題を解く. ここでは各辺(u, v) ∈ Eについてw(u, v) >= 0を仮定する
 *
 * @note   Dijkstraのアルゴリズムは、始点sからの最短路重みが最終的に決定された頂点の集合Sを管理する
 *         アルゴリズムは、繰り返し、最小の最短路推定値を持つ頂点u ∈ V - Sを選択し、uをSに追加し、
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
        for (auto& v : S) { v.d = graph::inf; v.pi = graph::nil; v._color = color::white; }
        S[s].d = 0; S[s]._color = color::gray;
    };
    // 辺(u, v)の緩和(relaxing)はuを経由することでvへの既知の最短路が改善できるか否かを判定し、改善できるならばv.dとv.πを更新する
    // 緩和によって最短路推定値v.dが減少し、vの先行点属性v.πが更新されることがある. 以下のコードは、辺(u, v)上の緩和をΟ(1)時間で実行する
    auto relax = [](const edge& e, vertices_t& S, pqueue<pair_t, cmp>& Q) ->void {
        index_t v = e.dst, u = e.src;
        if (S[v]._color != color::black &&  S[v].d > S[u].d + e.w) {
            S[v].d = S[u].d + e.w; S[v].pi = u; S[v]._color = color::gray;
            Q.emplace(std::make_pair(v, S[v].d)); 
        }
    };


    
    initsinglesource(S, s);                // すべての頂点のd値とπ値を初期化する
    Q.emplace(std::make_pair(s, S[s].d));  // このループの最初の実行ではu = sである
    while (!Q.empty()) {
        pair_t  p = Q.extract();
        index_t u = p.first; weight_t d = p.second;
        if (S[u].d < d) { continue; }
        for (auto& e : G[u]) {  // 頂点uからでる辺(u, v)をそれぞれ緩和し、
            relax(e, S, Q);     // uを経由することでvへの最短路が改善できる場合には、推定値v.dと先行点v.piを更新する
        }
        S[u]._color = color::black;  // 黒頂点は集合Sに属す
    }
    // 終了時点ではQ = φである. S = Vなので、すべての頂点u ∈ Vに対してu.d = δ(s, u)である
    return S;
}



int main(void)
{
    using namespace std;
    graph_t G;
    int n, k, u, v, c;
    
    cin >> n; G.resize(n);
    for (int i = 0; i < n; i++) {
        cin >> u >> k;
        G[u].resize(k);
        for (int j = 0; j < k; j++) {
            cin >> v >> c;
            G[u][j].src = u;
            G[u][j].dst = v;
            G[u][j].w   = c;
        }
    }

    vertices_t S = dijkstra(G, 0);
    for (int i = 0; i < n; i++) {
        cout << i << " " << S[i].d << endl; 
    }

    return 0;
}


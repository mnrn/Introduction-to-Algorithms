/**
 * @brief 深さ優先探索のテスト
 * @date  作成日     : 2016/02/15
 * @date  最終更新日 : 2016/02/15
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



/**
 * @brief グラフアルゴリズムに関する物置
 *
 * @note  グラフG = (V, E)には標準的な表現方法が2つある.隣接リストの集合による表現と隣接行列による表現である
 *        どちらを使っても有向グラフと無向グラフの両方を表現できる.グラフが疎(sparse)で|E|が|V|^2よりも
 *        ずっと小さいときには、隣接リストの表現はグラフをコンパクトに表現できる.これが表現を選択する際の基準になる
 *        グラフが密(dense)で|E|が|V|^2にほぼ等しいときや、2つの指定された頂点間に辺があるか否かを
 *        高速に判断する必要があるときには、隣接行列表現が好ましい場合もある
 *
 *        グラフG = (V, E)の隣接リスト表現(adjacency-list representation)は、Vの各頂点に対して1個、
 *        全部で|V|個のリストの配列Adjから構成される.各u ∈ Vに対して、隣接リストAdj[u]は辺(u, v) ∈ Eが存在する
 *        すべての頂点から構成される.(代わりに、隣接リストがこれらの頂点へのポインタを含むこともある.)
 *
 *        Gが有向グラフのとき、有向辺(u, v)はvが隣接リストAdj[u]の要素として現れることで、表現されるので、
 *        隣接リストの長さの合計は|E|である. Gが無向グラフのとき、(u, v)が無向辺ならば、uはvの隣接リストの要素であると
 *        同時にvはuの隣接リストの要素だから、隣接リストの長さの合計は2|E|である.有向グラフと無向グラフのどちらに対しても、
 *        隣接リスト表現はΘ(V + E)の記憶量しか必要としない望ましい性質を持っている
 *
 *        重み付きグラフ(weighted graph)では、重み関数(weight function) w:E->R(Rは実数体)によって、各辺に対して
 *        その重みを定義する. 重み付きグラフを表現できるように隣接リストを変更するのは簡単である
 *        たとえば、G = (V, E)を重み関数wを持つ重み付きグラフとすると、辺(u, v) ∈ Eの重みw(u, v)をuの隣接リストに
 *        頂点vとともに格納するだけでよい. 多くのグラフの変形に提要できるように修正できるという点で、
 *        隣接リスト表現は非常に柔軟性に富んでいる
 *
 *        隣接リストが潜在的に持つ欠点は、与えられた辺(u, v)がグラフに属するか否かを決定するには、隣接リストAdj[i]の中から
 *        vを探索するより速い方法がないことである.この欠点は隣接行列表現を用いると救済できるが、
 *        それには漸近的により多くの記憶量が必要になる
 *
 *        グラフG = (V, E)の隣接行列表現(adjacency-matrix representation)では、ある方法で、番号1, 2,...,|V|(one-based)が
 *        振られていると仮定する. グラフGの隣接行列表現は|V|x|V|型行列A = (aij)であり、
 *        aij = { 1 (i, j) ∈ Eのとき | 0 それ以外のとき }を満たす
 *        グラフの隣接行列はグラフの変数に関係なくΘ(V^2)の記憶量が必要である
 *
 *        隣接行列は主対角線に関して対称である. 無向グラフでは(u, v)と(v, u)は同じ辺を表すから、向こうグラフの隣接行列は
 *        その転置行列と等しい. すなわちA = Atである. いくつかの応用では、隣接行列の主対角線およびそれより上の要素だけを
 *        蓄え、グラフを格納するのに必要な記憶量をほぼ半分に減らすことができる
 *
 *        グラフの隣接リスト表現と同様、隣接行列表現でも重み付きグラフを表現できる. たとえば、 G = (V, E)を辺重み関数wを
 *        持つ重み付きグラフとするとき、辺(u, v) ∈ Eの重みw(u, v)を隣接行列のu行v列要素とすればよい
 *        辺が存在しないときは、NILを対応する行列要素の値としてもよいが、0や∞などの値を使ったほうが便利な場合も多い
 *
 *        隣接リスト表現の記憶効率は漸近的には隣接行列表現より悪くなることはないが、グラフが小さいときには
 *        隣接行列の単純さは捨てがたい. グラフが重み付きではない場合には1行列要素当たり1ビットで表現できるので、
 *        隣接行列表現はさらに有利になる
 *
 * @date  作成日     : 2016/02/08
 * @date  最終更新日 : 2016/02/20
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

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
    // weight_t f;           /**< 終了時刻印(DFSにおいて、黒色に彩色されたとき、刻まれる)     */
    vertex() : d(0), pi(0), _color(color::white)/*, f(0)*/ {}
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
 * @brief  スタック
 * @tparam class   T スタックの要素の型
 */
template <class T>
struct stack {
    std::int32_t _top;
    std::int32_t _max;
    T* S;

    stack(std::size_t size)
     : _top(-1),  _max(size),
      S(static_cast<T*>(::operator new(sizeof(T) * size))) { } 
    ~stack() noexcept
    {
        std::for_each(S, S+_top+1, destroy<T>);
        ::operator delete(S);
    }

    /**< @brief スタックが空かどうかを返す */
    bool empty()
    {
        return _top == -1;
    }

    /**< @brief スタックが満杯かどうか返す */
    bool full()
    {
        return _top >= (_max - 1);
    }

    /**< @brief スタックにxをプッシュする  */
    template<class... Args>
    void push(Args&&... args)
    {
        assert(!full());   // オーバーフローチェック
        _top++;
        construct(S[_top], T(std::forward<Args>(args)...));
    }

    /**< @brief スタックから一番上の要素をポップして返す */
    T pop()
    {
        assert(!empty());  // アンダーフローチェック
        T x = S[_top];
        _top--;
        destroy(S[_top+1]);
        return x;
    }

    /**< @brief スタックから一番上の要素を返す */
    T top()
    {
        assert(!empty());
        return S[_top];
    }

    /**< @brief スタックのサイズを返す */
    std::size_t size()
    {
        return _top + 1;
    }
};



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
        time = time + 1;              // timeを1進め、
        vs[u].d = time;               // timeの値を発見時刻u.dとして記録し、
        vs[u]._color = color::gray;   // uを灰に彩色する
        // 各頂点v ∈ Adj[u]を吟味するので、深さ優先探索は辺(u, v)を探索する(explore)という
        for (auto& e : G[u]) {                   // uと隣接する各頂点vを調べ、
            index_t v = e.dst;
            if (vs[v]._color == color::white) {  // vが白なら再帰的にvを訪問する
                vs[v].pi = u;
                visit(v);
            }
        }
        // uから出るすべての辺の探索が終了すると、
        vs[u]._color = color::black;  // uを黒に彩色し、
        time = time + 1;              // timeを進め、
        f[u] = time;                  // 終了時刻をu.fに記録する
    };


    // スタックを用いて白頂点uを訪問する
    auto _visit = [&](index_t u) -> void {
        stack<index_t> S(n); S.push(u);
        time = time + 1;             // timeを1進め、
        vs[u].d = time;              // timeの値を発見時刻u.dとして記録し、
        vs[u]._color = color::gray;  // uを灰に彩色する
        // 各頂点v ∈ Adj[u]を吟味するので、深さ優先探索は辺(u, v)を探索する(explore)という
        while (!S.empty()) {
            u = S.top();            // スタックの先頭の要素を取得
            std::size_t i = 0, m = G[u].size();  // 隣接リストの走査
            while (i < m && vs[G[u][i].dst]._color != color::white) { i++; }
            if (i != m) {  // uの隣接リストの中でまだ調べていない頂点が存在する場合、
                index_t v = G[u][i].dst;
                S.push(v);                   // vをスタックにプッシュし、
                time = time + 1;             // timeを1進め、 
                vs[v].d = time;              // timeの値を発見時刻u.dとして記録し、
                vs[v]._color = color::gray;  // vを灰に彩色する
            }
            else { // uの隣接リストを全て調べている場合、
                S.pop();                      // スタックから先頭の要素をポップし、
                time = time + 1;              // timeを進め、
                f[u] = time;                  // 終了時刻をu.fに記録する
                vs[u]._color = color::black;  // uを黒に彩色する
            }
        }
    };
    

    for (auto& u : vs) {
        u._color = color::white;               // 頂点をすべて白に彩色し、
        u.pi    = graph::nil;                  // π属性をNILに初期化する
    }
    time = 0;                                  // 時刻カウンターを初期化
    for (auto u = 0; u < n; u++) {             // Vの各頂点を順番に調べ、
        if (vs[u]._color == color::white) {    // 白頂点を発見すると、
            _visit(u);                         // visitを呼び出して訪問する
            // visitを呼び出すたびに、頂点uが深さ優先森の新しい木の根になる
        }
    }
    // DFSが終了した時、各頂点uには発見時刻(discovery time)u.dと終了時刻(finishing time)u.fが割り当てられている

    return std::make_pair(vs, f);
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
            G[i][j].src = i;
            G[i][j].dst = v;
        }
    }

    auto dfsforest = dfs(G);
    for (int i = 0; i < n; i++) {
        std::cout << i + 1 << " "
                  << dfsforest.first[i].d << " " << dfsforest.second[i] << std::endl;
    }

    return 0;
}


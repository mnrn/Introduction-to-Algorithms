/**
 * @brief  単一始点最短路問題(single-source shortest paths problem)における
 *         緩和(relaxation)を扱う
 * @date   2016/03/13
 */



//****************************************
// インクルードガード
//****************************************

#ifndef __RELAX_HPP__
#define __RELAX_HPP__



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include "graph.hpp"
#include <functional>



//****************************************
// 名前空間の始端
//****************************************

GRAPH_BEGIN



//****************************************
// 関数の宣言
//****************************************

/**
 * @brief  Θ(V)の手続きによって最短路推定値と先行点を初期化する
 * @note   初期化の後、すべてのv ∈ Vについてv.π = NIL、すべてのv ∈ V - {s}についてv.d = ∞である
 */
static inline void initialize_single_source(vertices_t& V, index_t s)
{
    for (auto& v : V) {
        v.d     = limits::inf;
        v.pi    = limits::nil;
    }
    V[s].d = 0;
}


/**
 * @brief  Θ(V)の手続きによって最短路推定値と先行点および頂点色を初期化する
 * @note   初期化の後、すべてのv ∈ Vについてv.π = NIL、
 *         すべてのv ∈ V - {s}についてv.d = ∞、 v.color = WHITEである　
 */
static inline void initialize_single_source_with_color(vertices_t& V, index_t s)
{
    for (auto& v : V) {
        v.d     = limits::inf;
        v.pi    = limits::nil;
        v.color = vcolor::white;
    }
    V[s].d = 0;
    V[s].color = vcolor::gray;
}


/**
 * @brief  Θ(V)の手続きによって最短路推定値と先行点および訪問済みフラグを初期化する
 * @note   初期化の後、すべてのv ∈ Vについてv.π = NIL、v.visited = false
 *         すべてのv ∈ V - {s}についてv.d = ∞である　
 */
static inline void initialize_single_source_with_visitor(vertices_t& V, index_t s)
{
    for (auto& v : V) {
        v.d       = limits::inf;
        v.pi      = limits::nil;
        v.visited = false;
    }
    V[s].d = 0;
}



/**
 * @brief  辺(u, v)の緩和(relaxing)は、uを経由することでvへの既知の最短路を
 *         改善できるか否かを判定し、改善できるならばv.dとv.πを更新する
 *         緩和によって最短路推定値v.dが減少し、vの先行点属性v.πが更新されることがある
 *
 * @note   以下のコードは、辺(u, v)上の緩和をΟ(1)時間で実行する
 *         ただし、手続きpred(V, u)がΟ(1)で実行されることを仮定する
 *
 * @param  vertices_t& V   頂点集合V
 * @param  index_t u       辺(u, v)の始点u (ただし、u ∈ V)
 * @param  index_t v       辺(u, v)の終点v (ただし、v ∈ V)
 * @param  weight_t w      辺(u, v)の重みw
 * @param  Predicate pred  relax可能な前提条件を記述した述語
 */
static void relax(vertices_t& V,
           index_t u, index_t v, weight_t w,
           std::function<bool(const vertices_t&, index_t)> pred)
{
    if (pred(V, u) && V[v].d > V[u].d + w) {
        V[v].d = V[u].d + w;
        V[v].pi = u;
    }
}


// オーバーロードされたrelax関数群

static inline void relax(vertices_t& V, const edge& e,
           std::function<bool(const vertices_t&, index_t)> pred)
{
    relax(V, e.src, e.dst, e.w, pred);
}
static inline void relax(vertices_t& V, const matrix_t& W,
           index_t u, index_t v,
           std::function<bool(const vertices_t&, index_t)> pred)
{
    relax(V, u, v, W[u][v], pred);
}


/**
 * @brief  辺(u, v)を緩和すると同時に、頂点vおよび道s~>vの重みをmin優先度付きキューQに挿入する
 * 
 * @tparam PriorityQueue min優先度付きキューの型
 * @param vertices_t&    V 頂点集合V
 * @param const edge&    e 辺(u, v)
 * @param PriorityQueue& Q min優先度付きキュー
 */
template<class PriorityQueue>
void relax_with_heap(vertices_t& V, const edge& e, PriorityQueue& Q)
{
    index_t u = e.src, v = e.dst;
    if (V[v].color != vcolor::black && V[v].d > V[u].d + e.w) {
        V[v].d     = V[u].d + e.w;
        V[v].pi    = u;
        V[v].color = vcolor::gray;
        Q.emplace(v, V[v].d);
    }
}



//****************************************
// 名前空間の終端
//****************************************

GRAPH_END



#endif  // end of __RELAX_HPP__


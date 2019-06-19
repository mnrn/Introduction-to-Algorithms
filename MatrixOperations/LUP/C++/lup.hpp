/**
 * @bfief LUP分解について扱う
 * @date  2016/03/09
 */



 //****************************************
// インクルードガード
//****************************************

#ifndef __LUP_HPP__
#define __LUP_HPP__



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <vector>
#include <functional>



//****************************************
// 型シノニム
//****************************************

using elem_t    = double;
using vec_t     = std::vector<elem_t>;
using mat_t     = std::vector<vec_t>;
using index_t   = std::int32_t;
using indices_t = std::vector<index_t>;



//****************************************
// 関数の宣言
//****************************************

/**
 * @brief  正則行列AのLUP分解を行う
 *
 * @note   置換行列Pは配列πとして動的に管理する. ここでπ[i] = jはPの第i行は第j列に1を持つことを意味する
 *         さらに、A上でLとUを「その場で」計算する. したがって、手続きが終了したときは
 *           aij = { lij i >  jのとき
 *                   uij i <= jのとき }
 *         が成立する
 *
 * @note   LUP-DECOMPOSITIONの実行時間はΘ(n^3)である
 *
 * @param[inout] mat_t& A  nxn型の正則行列A
 * @return indices_t pi    置換行列Pの配列表現
 */
indices_t lup_decomposition(mat_t& A);



/**
 * @brief  与えられたP, L, Uおよびbに対して、手続きLUP-SOLVEは前進代入と後進代入を組み合わせて解xを得る
 * @note   ただし、置換行列Pは配列πで表現される
 *
 * @note   LUP-SOLVEは引数にLおよびUの2つをとる必要があった. そこで、Aに直接LおよびUを計算した値を代入した行列Aのみを
 *         引数にとるように修正を施す
 *
 * @note   手続きの実行時間はΘ(n^2)である
 *
 * @param  const mat_t& A       aij = { lij (i > j)、 uij (i <= j) }を満たすnxn型行列A
 * @param  const indices_t& pi  置換行列Pの配列表現(i=0,1,..n-1に対してpi[i]はP(i,pi[i])=1で、j!=pi[i]ならばPij=0)
 * @param  const vec_t& b       入力ベクトルb
 * @return vec_t x              LUx = Pbを満たすようなx
 */
vec_t lup_solve(const mat_t& A, const indices_t& pi, const vec_t& b);



#endif  // end of __LUP_HPP__


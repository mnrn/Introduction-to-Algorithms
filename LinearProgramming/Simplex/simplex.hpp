/**
 * @brief 線形計画問題(linear-programming problem)における
 *        シンプレックスアルゴリズム(simplex algorithm)を扱います
 * @date  2016/03/14
 */



//****************************************
// インクルードガード
//****************************************

#ifndef __SIMPLEX_HPP__
#define __SIMPLEX_HPP__



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <tuple>
#include <vector>
#include <cstdint>



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



std::tuple<indices_t, indices_t, mat_t, vec_t, vec_t, elem_t>
pivot(const indices_t& N, const indices_t& B, const mat_t& A,
      const vec_t& b,const vec_t& c, elem_t v, index_t l, index_t e);


vec_t simplex_no_init(indices_t& N, indices_t& B, mat_t& A, vec_t& b, vec_t& c, elem_t v);



#endif  // end of __SIMPLEX_HPP__


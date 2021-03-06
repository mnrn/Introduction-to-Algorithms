/**
 * @brief  Knuth, Morris, Prattによる線形時間の文字列照合アルゴリズムを扱う
 *
 * @note  このアルゴリズムは遷移関数δを一度に計算するのを避け、補助関数πだけを用いて照合時間Θ(n)を達成する
 *        ここでは、πは事前にパターンからΘ(m)時間をかけて計算し、配列π[0..m-1]に保存しておく
 *        遷移関数δは必要になった時点で"動的に"配列πから(ならし時間の意味で)効率よく計算する
 *        大雑把に言えば、任意の状態q = 0,1,...,mと任意の文字a ∈ Σに対して、値π[q]はδ(q, a)の計算に必要な
 *        すべての情報を含むが、この情報はaに依存しない. 配列πはm個の要素しか持たず、一方、δはΘ(m|Σ|)個の
 *        要素を持つから、δではなくπを計算することで、前処理時間を1/|Σ|に削減できる
 *
 * @date   2016/04/28
 */



//****************************************
// インクルードガード
//****************************************

#ifndef __KMP_HPP__
#define __KMP_HPP__



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include "../StringMatching/string_matching.hpp"



//****************************************
// 関数の宣言
//****************************************


void kmp_matcher(string_t T, string_t P);



#endif  // end of __KMP_H__

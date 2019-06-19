/**
 * @brief  モジュロ算(modular arthmetic)に関するアルゴリズムを扱います
 *
 * @note   モジュロ算とは、形式ばらずにいうと、モジュロnについて考えているとき、すべての結果xはnを法として
 *         xに等しい{0,1,...,n-1}の要素で置き換えられる(すなわち、xはx mod nで置き換えられる)ことを除いて、
 *         整数に関する普通通りの算術と考えることができる. 加算、減算、および乗算の演算に固執するならば、
 *         この略式モデルでも十分である. これから与えるモジュロ算に対する形式的なモデルは、群論の枠内で記述するのが最善である
 *
 * @date   2016/03/30
 */



//****************************************
// インクルードガード
//****************************************

#ifndef __MODULAR_HPP__
#define __MODULAR_HPP__



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include "../NumberTheory/number_theory.hpp"



//****************************************
// 関数の宣言
//****************************************

/**
 * @brief  1次合同式ax ≡ b (mod n)の解をすべて出力する
 *
 * @note   入力aとnは任意の正整数でbは任意の整数である
 *
 * @note   EXTENDED-EUCLIDが算術演算をΟ(lgn)回実行し、for文の各繰り返しが算術演算を定数回実行するから、
 *         MODULAR-LINEAR-EQUATION-SOLVERは算術演算をΟ(lgn + gcd(a, n))回実行する
 */
intset_t modular_linear_equation_solver(int_t a, int_t b, int_t n);



#endif  // end of __MODULAR_HPP__

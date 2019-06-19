/**
 * @brief  有限オートマトンを用いる文字列照合を扱います
 *
 * @note   有限オートマトン(finite automaton)Mは5項組(Q, q0, A, Σ, δ)である. ここで、
 *
 *         * Qは状態(state)の有限集合
 *         * q0 ∈ Qは開始状態(start state)
 *         * A ⊆ Qは受理状態(accepting state)の集合
 *         * Σは有限入力アルファベット(input alphabet)
 *         * δは、Mの遷移関数(transition function)と呼ぶ、Q x ΣからQへの関数である
 *
 * @note   有限オートマトンは状態q0から開始し、入力文字列を1文字ずつ読む. オートマトンの状態がqのときに文字aを読めば、
 *         状態はδ(q, a)に移る("遷移する"). 現在の状態qがAの要素ならば、機械Mは、これまで読み込んだ文字列を受理する(accepted)
 *         受理されなかった入力は却下される(rejected)
 *
 * @note   有限オートマトンMは最終状態関数(final-state function)と呼ぶ、Σ*からQへの関数φを誘導する
 *         ここで、φ(w)はMが文字列wを操作し終わった時に到達する状態である. したがって、φ(w)∈Aのとき、
 *         かつそのときに限り、Mはwを受理する. 関数φを
 *           φ(ε)  = q0
 *           φ(wa) = δ(φ(w),a), 任意のw∈Σ*, a∈Σに対して
 *         と、遷移関数を用いて再帰的に定義する
 *
 * @date   2016/03/31
 */



//****************************************
// インクルードガード
//****************************************

#ifndef __FINITE_AUTOMATON_HPP__
#define __FINITE_AUTOMATON_HPP__



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include "../StringMatching/string_matching.hpp"



//****************************************
// 関数の宣言
//****************************************

void finite_automaton_matcher(string_t T, const delta_t& delta, strlen_t m);



#endif  // end of __FINITE_AUTOMATON_HPP__


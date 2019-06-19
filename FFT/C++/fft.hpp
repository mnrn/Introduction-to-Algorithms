/**
 * @bfief FFT(高速フーリエ変換)ついて扱う
 * @date  2016/03/08
 */


//****************************************
// インクルードガード
//****************************************

#ifndef __FFT_HPP__
#define __FFT_HPP__



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <vector>
#include <complex>
#include <cmath>



//****************************************
// 型シノニム
//****************************************

using complex_t = std::complex<double>;
using vec_t = std::vector<complex_t>;



//****************************************
// 関数の宣言
//****************************************

/**
 * @brief  再帰型FFT(高速フーリエ変換)アルゴリズム
 *
 * @note   このアルゴリズムはn要素ベクトルa = (a0, a1, ..., a(n-1))のDFT(離散フーリエ変換)を計算する
 *         ただし、nは2の冪乗であると仮定する
 *
 * @note   高速フーリエ変換の実行時間はΘ(nlgn)である
 *
 * @param  const vec_t& a 入力ベクトルa
 * @return 入力ベクトルaのDTF
 *
 *
 * @note   (*1)式において、各y0, y1, ..., y(n/2 - 1)については、
 *         yk = yk^[0] + ωn^k * yk^[1]
 *            = A^[0](ωn^2k) + ωn^k * A^[1](ωn^2k)
 *            = A(ωn^k)
 *         が成立する
 *
 * @note   (*2)式において、各y(n/2), y(n/2 + 1), ..., y(n-1)については、k = 0, 1, ..., n/2 - 1とすると、
 *         y(k+(n/2)) = yk^[0] - ωn^k * yk^[1]
 *                    = yk^[0] + ωn^(k+(n/2)) * yk^[1]                      (∵ ωn^(k+(n/2)) = -ωn^k)
 *                    = A^[0](ωn^2k) + ωn^(k+(n/2)) * A^[1](ωn^2k) 
 *                    = A^[0](ωn^(2k+n)) + ωn^(k+(n/2)) * A^[1](ωn^(2k+n))  (∵ ωn^(2k+n) = ωn^2k)
 *                    = A(ωn^(k+(n/2)) 
 *         が成立する
 */
vec_t recursive_fft(const vec_t& a);



/**
 * @brief  繰り返し型FFTの実現
 * 
 * @note   このアルゴリズムはn要素ベクトルa = (a0, a1, ..., a(n-1))のDFT(離散フーリエ変換)を計算する
 *         ただし、nは2の冪乗であると仮定する
 *
 * @note   繰り返し版FFTアルゴリズムの時間計算量はΘ(nlgn)であるが、このΘ記法に隠された係数は再帰版よりも小さい
 *         しかし、実現によっては再帰版の方がハードウェアキャッシュをより効果的に利用できるかもしれない
 *
 * @param  const vec_t& a 入力ベクトルa
 * @param  bool regular true..順変換, false..逆変換
 * @return 入力ベクトルaのDTF
 */
vec_t iterative_fft(const vec_t& a, bool regular = true);



/**
 * @brief  修正版繰り返し型FFT
 * 
 * @note   このアルゴリズムはn要素ベクトルa = (a0, a1, ..., a(n-1))のDFT(離散フーリエ変換)を計算する
 *         ただし、nは2の冪乗であると仮定する
 *
 * @note   ITERATIVE-FFTが各ステージで回転因子を計算する回数は2^(lgn - 1)つまり、n / 2回であった
 *         そこで、各ステージsで回転因子を2^(s - 1)回しか計算しないように修正を施す
 *
 * @param  const vec_t& a 入力ベクトルa
 * @param  bool regular true..順変換, false..逆変換
 * @return 入力ベクトルaのDTF
 */
vec_t modified_fft(const vec_t& a, bool regular = true);



#endif  // end of __FFT_HPP__


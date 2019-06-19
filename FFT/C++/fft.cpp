/**
 * @bfief FFT(高速フーリエ変換)について扱う
 * @note  FFTアルゴリズムにおいて、aとyの役割を入れ替え、ωnをωn^-1で置き換え、
 *        得た結果の各要素をnで割ることにより、逆DFTが計算できる
 * @date  2016/03/08
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <iostream>
#include "fft.hpp"



//****************************************
// 関数の宣言
//****************************************

std::size_t bit_length(std::size_t x);
std::size_t bit_reverse(std::size_t x, std::size_t lgn);
void bit_reverse_copy(const vec_t& a, vec_t& A, std::size_t lgn);



//****************************************
// 関数の定義
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
vec_t recursive_fft(const vec_t& a)
{
    std::size_t n = a.size();  // nは2の冪乗である
    if (n == 1) { return a; }  // 再帰の基底. 1要素のDFTはy0 = a0 * ω1^0 = a0 * 1 = a0だから、それ自身である

    const complex_t omega_n(std::cos(2.0 * M_PI / n), std::sin(2.0 * M_PI / n));  // ωn = e^(2πi/n)
    complex_t omega =  1.0;                                                       // ω = 1

    vec_t a_0_(n / 2), a_1_(n / 2);  // 多項式A^[0]とA^[1]の係数ベクトルを定義する
    for (std::size_t i = 0; i < n / 2; i++) {
        a_0_[i] = a[i * 2];      // a^[0] = (a0, a2, ..., a(n-2))
        a_1_[i] = a[i * 2 + 1];  // a^[1] = (a1, a3, ..., a(n-1))
    }
    // 再帰型DFT(n/2)計算を実行し、k = 0, 1, ..., n/2 - 1に対して、以下のように置く
    vec_t y_0_ = recursive_fft(a_0_);  // yk^[0] = A^[0]((ω(n/2))^k) = A^[0]((ωn)^(2k))
    vec_t y_1_ = recursive_fft(a_1_);  // yk^[1] = A^[1]((ω(n/2))^k) = A^[1]((ωn)^(2k))

    vec_t y(n);  // 再帰型DFT(n/2)の計算結果を結合する
    for (std::size_t k = 0; k < n / 2; k++) {
        y[k]         = y_0_[k] + omega * y_1_[k];  // ...(*1)
        y[k + (n/2)] = y_0_[k] - omega * y_1_[k];  // ...(*2)
        omega = omega * omega_n;  // ωの更新
    }

    return y;  // yは列ベクトルであると仮定する
}


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
vec_t iterative_fft(const vec_t& a, bool regular)
{
    std::size_t n = a.size();  // nは2の冪乗を仮定する
    
    vec_t A(n);
    std::size_t lgn = bit_length(n) - 1;
    const double PI = regular ? M_PI : -M_PI;  // 逆変換の場合、ωnをωn^-1に置き換える

    bit_reverse_copy(a, A, lgn);  // ベクトルaの各要素を配列Aの望ましい位置に格納する
    
    // sは1(葉で2要素DFTを計算するために結合しているとき)から開始し、
    // lgn(根で最終結果を得るために2つの(n/2)要素DFTを結合しているとき)まで増加する
    for (std::size_t s = 1; s <= lgn; s++) {
        
        // 各バタフライ演算が使用する回転因子はsに依存し、m = 2^sとするとき、ωmの冪乗である
        std::size_t m = 1 << s;  
        complex_t omega_m(std::cos(2.0 * PI / m), std::sin(2.0 * M_PI / m));

        // A[k..k+2^(s-1)-1]とA[k+2^(s-1)..k+2^s-1]に属する2^(s-1)要素DFT結合し、
        // A[k..k + 2^s-1]に属する2^s要素DFTを計算する
        for (std::size_t k = 0; k < n; k += m) {
            
            complex_t omega = 1.0;
            
            for (std::size_t j = 0; j < m / 2; j++) {
                complex_t t = omega * A[k + j + m / 2];  // 共通部分式を一時変数tとし、その値を1度だけ計算するようにする
                complex_t u = A[k + j];                  // 一時変数uをバタフライ演算を"その場"で行うために導入する
                // バタフライ演算
                A[k + j]         = u + t;
                A[k + j + m / 2] = u - t;
                // ωの更新
                omega = omega * omega_m;
            }
        }
    }
    // 逆変換の場合、得た結果の各要素をnで割る必要がある
    if (!regular) { for (std::size_t i = 0; i < n; i++) { A[i] /= n; } }
    
    return A;
}


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
vec_t modified_fft(const vec_t& a, bool regular)
{
    std::size_t n = a.size();  // nは2の冪乗を仮定する
    
    vec_t A(n);
    std::size_t lgn = bit_length(n) - 1;
    const double PI = regular ? M_PI : -M_PI;  // 逆変換の場合、ωnをωn^-1に置き換える

    bit_reverse_copy(a, A, lgn);  // ベクトルaの各要素を配列Aの望ましい位置に格納する
    
    // sは1(葉で2要素DFTを計算するために結合しているとき)から開始し、
    // lgn(根で最終結果を得るために2つの(n/2)要素DFTを結合しているとき)まで増加する
    for (std::size_t s = 1; s <= lgn; s++) {
        
        // 各バタフライ演算が使用する回転因子はsに依存し、m = 2^sとするとき、ωmの冪乗である
        std::size_t m = 1 << s;  
        complex_t omega_m(std::cos(2.0 * PI / m), std::sin(2.0 * M_PI / m));
        complex_t omega = 1.0;

        // A[k..k+2^(s-1)-1]とA[k+2^(s-1)..k+2^s-1]に属する2^(s-1)要素DFT結合し、
        // A[k..k + 2^s-1]に属する2^s要素DFTを計算する
        for (std::size_t j = 0; j < m / 2; j++) {        // ITERATIVE-FFTでは内側にあったループを外側に持ってくることでωを2^(s-1)回更新できるようにする
            for (std::size_t k = 0; k < n; k += m) {     // k + jに着目して、k = jとしてもよい
                complex_t t = omega * A[k + j + m / 2];  // 共通部分式を一時変数tとし、その値を1度だけ計算するようにする
                complex_t u = A[k + j];                  // 一時変数uをバタフライ演算を"その場"で行うために導入する
                // バタフライ演算
                A[k + j]         = u + t;
                A[k + j + m / 2] = u - t;
            }
            omega = omega * omega_m;  // ωの更新を外側のループで行う
        }
    }

    // 逆変換の場合、得た結果の各要素をnで割る必要がある
    if (!regular) { for (std::size_t i = 0; i < n; i++) { A[i] /= n; } }
    
    return A;
}


int main()
{
    const int N = 8;
    vec_t input(N);

    std::cout << "input: " << std::endl;
    for (int i = 0; i < N; i++) {
        input[i] = static_cast<float>(i);
        std::cout << input[i].real() << " " << input[i].imag() <<std::endl;
    }
    std::cout << std::endl;

    vec_t output = recursive_fft(input);
    std::cout << "再帰版FFT: ";
    for (auto& a : output) {
        std::cout << a << " ";
    }
    std::cout << std::endl;

    output = iterative_fft(input, true);
    std::cout << "繰り返し型FFT: ";
    for (auto& a : output) {
        std::cout << a << " ";
    }
    std::cout << std::endl;

    output = iterative_fft(output, false);
    std::cout << "逆FFTで復元: ";
    for (auto& a : output) {
        std::cout << a << " ";
    }
    std::cout << std::endl;

    output = modified_fft(input, true);
    std::cout << "修正版FFT: ";
    for (auto& a : output) {
        std::cout << a << " ";
    }
    std::cout << std::endl;

    output = modified_fft(output, false);
    std::cout << "修正版逆FFTで復元: ";
    for (auto& a : output) {
        std::cout << a << " ";
    }
    std::cout << std::endl;
    

    return 0;
}



std::size_t bit_length(std::size_t x)
{
    std::size_t n = 0;
    while (x > 0) { n++; x >>= 1; }
    return n;
}


std::size_t bit_reverse(std::size_t x, std::size_t lgn)
{
    if (x == 0) { return x; }

    std::size_t rev = 0;

    // 長さ奇数の場合、中央のbitを取得しておく
    if (lgn & 0x01) { rev |= (((x >> (lgn >> 1)) & 0x01) << (lgn >> 1)); }

    // 前後の順序を入れ替えて取得
    for (std::size_t i = 0; i < (lgn >> 1); i++) {
        rev |= ((x >> i) & 0x01) <<  (lgn - 1 - i);
        rev |= ((x >> (lgn - 1 - i)) & 0x01) << i;
    }
    return rev;
}


void bit_reverse_copy(const vec_t& a, vec_t& A, std::size_t lgn)
{
    for (std::size_t i = 0; i < (1u << lgn); i++) {
        A[bit_reverse(i, lgn)] = a[i];
    }
}


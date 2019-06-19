/**
 * @brief RabinとKarpのアルゴリズムを扱います
 * @date  2016/03/31
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <iostream>
#include <cstdint>
#include <cstring>



//****************************************
// 型シノニム
//****************************************

using string_t = const char*;
using strlen_t = std::int32_t;
using radix_t  = std::int32_t;
using prime_t  = std::int32_t;
using uint_t   = std::uint64_t;



//****************************************
// 関数の定義
//****************************************

/**
 * @brief 反復2乗法(repeated squaring)はbの2進表現を用いてベキ乗剰余(modular exponentiation)を解く
 *
 * @note  aとbを非負整数, nを正整数とするとき、a^b mod nを効率良く計算する
 *
 * @note  次の手続きでは、2倍にする操作と1を加える操作を繰り返してcを0からbまで増加させるのにしたがって、
 *        対応するa^c mod nを計算する
 *
 * @note  入力a, b, nがβビットの数ならば、必要な算術演算の総数はΟ(β)であり、したがって、必要なビット演算の総数はΟ(β^3)である
 */
uint_t modular_exponentiation(uint_t a, uint_t b, uint_t n)
{
    uint_t d = 1;
    while (b > 0) {
        if (b & 0x01) {
            d = (d * a) % n;
        }
        a = (a * a) % n; 
        b >>= 1;
    }
    
    return d;
}



/**
 * @brief  Rabin-Karpアルゴリズム
 *
 * @note   手続きへの入力はテキストT、パターンP、使用する基数d(通常は|Σ|である)、そして、使用する素数qである
 *
 * @note   RABIN-KARP-MATHCERは前処理にΘ(m)時間かかる. RabinとKarpのアルゴリズムは、すべての正当なシフトを明示的に
 *         検証するから、最悪時には照合にΘ((n-m+1)m)時間かかる. たとえば、P=a^m, T=a^nのときには、可能なn-m+1個のシフトが
 *         すべて正当だから、この検証にΘ((n-m+1)m)時間かかる
 *
 * @note   わずかな回数、ある定数回数cしか正当なシフトがないと予想できる多くの応用がある. このような応用では、このアルゴリズムの
 *         期待照合時間はΟ((n-m+1) + cm) = Ο(n + m)に疑似ヒットの処理時間を加えたものである. qの剰余計算が、Σ*からZqへの
 *         ランダム写像のように働くという仮定に基づいて、発見的な解析が行える. このとき、任意のtsが法qの元でpに等しい確率を1/qと
 *         評価できるので、疑似ヒット数の期待値はΟ(n/q)である. テストが失敗に終わるΟ(n)個の位置があり、ヒットした場合には
 *         Ο(m)時間が必要になるので、Rabin-Karpアルゴリズムの期待照合時間は
 *           Ο(n) + Ο(m(v + n/q))
 *         である. ここでvは正当なシフトの数である. v = Ο(1)であって、q >= mであるようにqを選択すれば、実行時間はΟ(n)になる
 *         すなわち、正当なシフト数の期待値が小さいとき(Ο(1)のとき)、パターン長よりも大きい素数qを選択すればRabin-Karp手続きは
 *         照合時間がΟ(n+m)で動作することが期待できる. m<=nだから、この期待照合時間はΟ(n)である
 */
void rabin_karp_matcher(string_t T, string_t P, radix_t d, prime_t q)
{
    strlen_t n = strlen(T);
    strlen_t m = strlen(P);

    strlen_t h = modular_exponentiation(d, m-1, q);
    
    strlen_t p = 0, t = 0;
    for (strlen_t i = 0; i < m; i++) {       // 前処理
        p = (d * p + P[i]) % q;
        t = (d * t + T[i]) % q;
    }
    

    for (strlen_t s = 0; s <= n - m; s++) {  // 照合
        // このfor文は、以下の不変式を保存しながら、すべての可能なシフトsに対して繰り返す
        //   ts = T[s..s+m-1] mod qである
        
        if (p == t) {                  // p = ts("ヒット"した)ならば、疑似ヒットを排除するために
            strlen_t r;
            for (r = 0; r < m; r++) {  // P[0..m-1] = T[s..s+m-1]であるか否かを判定する
                if (T[s + r] != P[r]) { break; }
            }
            if (r == m) {
                std::cout << s << std::endl;  // 発見した正当なシフトを印刷する
            }
        }

        if (s < n - m) {  // s < n - mならば、少なくともfor文をもう1回繰り返すので、
            t = (((d * (t - T[s] * h) + T[s + m]) % q) + q) % q;  // ループ不変式を保存されるようにする
        }
    }
}



//****************************************
// エントリポイント
//****************************************

int main()
{
    using namespace std;

    const radix_t d = 256;
    const prime_t q = 101;

    string T, P;
    cin >> T >> P;

    rabin_karp_matcher(T.c_str(), P.c_str(), d, q);
    
    return 0;
}


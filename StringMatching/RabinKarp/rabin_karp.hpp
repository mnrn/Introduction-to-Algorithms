/**
 * @brief RabinとKarpのアルゴリズムを扱います
 * @date  2016/03/31
 */



//****************************************
// インクルードガード
//****************************************

#ifndef __RABIN_KARP_HPP__
#define __RABIN_KARP_HPP__



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include "../StringMatching/string_matching.hpp"



//****************************************
// 関数の宣言
//****************************************

/**
 * @brief  Rabin-Karpアルゴリズム
 *
 * @note   手続きへの入力はテキストT、パターンP、使用する基数d(通常は|Σ|である)、そして、使用する素数qである
 */
void rabin_karp_matcher(string_t T, string_t P, radix_t d, prime_t q);



#endif  // end of __RABIN_KARP_HPP__

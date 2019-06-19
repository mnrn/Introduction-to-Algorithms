/**
 * @bfief Xorshift法による疑似乱数生成
 * @note  正則なnxnのバイナリ行列Tが
 *        任意の0でない1xnのバイナリベクトルβに対し、
 *        β, βT, βT^2,...が0でないありうる全ての1xnのバイナリベクトルを
 *        生成するのはTのOrderが2^n - 1のときであり、かつそのときに限る
 *        という定理に基づいたrandom number generatorである
 * @note  コードを見たら分かる通りseed値固定です
 * @date  作成日     : 2016/01/31
 * @date  最終更新日 : 2016/01/31
 */


//****************************************
// インクルードガード
//****************************************

#ifndef __XORSHIFT_HPP__
#define __XORSHIFT_HPP__



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <cstdint>



//****************************************
// 関数の定義
//****************************************

/**
 * @brief 周期2^32 - 1の擬似乱数を生成する
 */
uint32_t xorshift32()
{
    static uint32_t y = 2463534242;
    y^=(y<<13); y^=(y>>17);
    return (y^=(y<<5));
}

/**
 * @brief 周期2^64 - 1の擬似乱数を生成する
 */
uint64_t xorshift64()
{
    static uint64_t x = 88172645463325252;
    x^=(x<<13); x^=(x>>7);
    return (x^=(x<<17));
}

/**
 * @brief 周期2^96 - 1の擬似乱数を生成する
 */
uint32_t xorshift96()
{
    static uint32_t x = 123456789, y = 362436069, z = 521288629;
    uint32_t t = (x^(x<<3))^(y^(y>>19))^(z^(z<<6));
    x = y; y = z;
    return (z = t);
}

/**
 * @brief 周期2^128 - 1の擬似乱数を生成する
 */
uint32_t xorshift128()
{
    static uint32_t x = 123456789, y = 362436069, z = 521288629, w = 88675123;
    uint32_t t = (x^(x<<11));
    x = y; y = z; z = w;
    return (w=(w^(w>>19))^(t^(t>>8)));
}



#endif  // end of __XORSHIFT_H__


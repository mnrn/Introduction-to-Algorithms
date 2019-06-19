/**
 * @bfief 選択問題を扱います
 * @note  受け取るイテレータは基本的にランダムアクセスイテレータを想定しています
 * @date  作成日     : 2016/02/02
 * @date  最終更新日 : 2016/02/02
 */


//****************************************
// インクルードガード
//****************************************

#ifndef __SELECTION_HPP__
#define __SELECTION_HPP__



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <iterator>
#include <tuple>
#include "../quicksort/quicksort.hpp"



//****************************************
// 関数プロトタイプ
//****************************************

template <class Iterator, class Compare>
static Iterator _randselect(Iterator p, Iterator r, std::ptrdiff_t i, Compare cmp);



//****************************************
// 関数の定義
//****************************************



////////////////////////////////////////////////////////////////////////
// まず、最大最小問題を扱う
////////////////////////////////////////////////////////////////////////


/**
 * @brief  要素数nの配列Aに対し、最小値を求める
 *
 * @note   最小値を決定する任意のアルゴリズムを要素間のトーナメントと考える
 *         各比較はトーナメントの試合に相当し、2者の小さいほうが試合に勝つ
 *         優勝者以外は少なくとも1回は試合に負けることに注目すると、
 *         最小値を決定するには少なくともn - 1回の比較が必要であり、
 *         アルゴリズムminimumは必要な比較回数の点で最適である
 *
 * @tparam Iterator イテレータ
 * @param  Iterator a0  先頭イテレータ
 * @param  Iterator aN  末尾の次を指すイテレータ
 * @return Iterator min 最小値を指すイテレータ
 */
template <class Iterator>
Iterator minimum(Iterator a0, Iterator aN)
{
    Iterator min = a0;
    Iterator i = a0; ++i;
    for (; i != aN; ++i) {
        if (*i < *min) { min = i; }
    }
    return min;
}

/**
 * @brief  要素数nの配列Aに対し、最大値を求める
 *
 * @note   最大値を決定する任意のアルゴリズムを要素間のトーナメントと考える
 *         各比較はトーナメントの試合に相当し、2者の大きいほうが試合に勝つ
 *         優勝者以外は少なくとも1回は試合に負けることに注目すると、
 *         最大値を決定するには少なくともn - 1回の比較が必要であり、
 *         アルゴリズムmaximumは必要な比較回数の点で最適である
 *
 * @tparam Iterator イテレータ
 * @param  Iterator a0  先頭イテレータ
 * @param  Iterator aN  末尾の次を指すイテレータ
 * @return Iterator max 最大値を指すイテレータ
 */
template<class Iterator>
Iterator maximum(Iterator a0, Iterator aN)
{
    Iterator max = a0;
    Iterator i = a0; ++i;
    for (; i != aN; ++i) {
        if (*i > *max) { max = i; }
    }
    return max;
}


/**
 * @brief  要素数nの配列Aに対し、最小(最大)値を求める
 *
 * @note   最小(最大)値を決定する任意のアルゴリズムを要素間のトーナメントと考える
 *         各比較はトーナメントの試合に相当し、2者の小さい(大きい)ほうが試合に勝つ
 *         優勝者以外は少なくとも1回は試合に負けることに注目すると、
 *         最小(最大)値を決定するには少なくともn - 1回の比較が必要であり、
 *         アルゴリズムminmaxは必要な比較回数の点で最適である
 *
 * @tparam Iterator イテレータ
 * @tparam Compare  比較用関数オブジェクト
 * @param  Iterator a0 先頭イテレータ
 * @param  Iterator aN 末尾の次を指すイテレータ
 * @param  Compare cmp 比較述語
 * @return Iterator minmax 最小(最大)値を指すイテレータ
 */
template <class Iterator, class Compare>
Iterator minmaxel(Iterator a0, Iterator aN, Compare cmp)
{
    Iterator minmax = a0;
    Iterator i = a0; ++i;
    for (; i != aN; ++i) {
        if (cmp(*i, *minmax)) { minmax = i; }
    }
    return minmax;
}


/**
 * @brief  最小値と最大値を同時に求める
 * @tparam Iterator イテレータ
 * @param  Iterator a0 先頭を指すイテレータ
 * @param  Iterator aN 末尾の次を指すイテレータ
 * @return 最小値を指すイテレータと最大値を指すイテレータの対(pair)
 */
template <class Iterator>
std::pair<Iterator, Iterator> minmaxpr(Iterator a0, Iterator aN)
{
    using dif_t = typename std::iterator_traits<Iterator>::difference_type;

    Iterator min, max;
    const dif_t n = std::distance(a0, aN);
    Iterator aM = a0;  std::advance(aM, n>>1);  // aM = a0 + n/2
    
    Iterator i = a0, j = aM; // 入力要素の列を2要素の対A[0..m-1], A[m..n-1]に区切る
    
    if (n & 0x01) {          // nが奇数ならば、
        min = j, max = j;    // 最大値と最小値をともにA[m]に設定し、
        ++j;                 // 残りの要素を対にして処理する 
    }
    else {                   // nが偶数ならば、
        // 現時点での最大値と最小値の初期値を決定するために最初の2要素間で1回の比較を行い、
        if (*i < *j) { min = i, max = j; }
        else         { min = j; max = i; }
        ++i, ++j;            // 残りの要素を対にして処理する
    }

    for (;j != aN; ++i, ++j) {
        // まずこの対をなす要素を互いに比較し、
        if (*i < *j) {  if (*i < *min) { min = i; }   // 次に、小さいほうを現時点の最小値と比較し、
                        if (*j > *max) { max = j; }   // 最後に大きいほうを現時点の最大値と比較する
        }
        else {          if (*j < *min) { min = j; }
                        if (*i > *max) { max = i; }
        }
    }

    return std::make_pair(min, max);
}



////////////////////////////////////////////////////////////////////////
// 次に、一般の選択問題を扱う
////////////////////////////////////////////////////////////////////////



/**
 * @brief  乱択版選択アルゴリズムの本体呼び出し
 * @detail 配列A[p..r]のi番目に小さい(大きい)要素を返します
 * @tparam Iterator イテレータ
 * @param  Iterator a0      先頭イテレータ
 * @param  Iterator aN      末尾イテレータ
 * @param  std::ptrdiff_t i 整数i(1 <= i <= n)
 */
template <class Iterator, class Compare>
Iterator randselect(Iterator a0, Iterator aN, std::ptrdiff_t i, Compare cmp)
{
    return _randselect(a0, --aN, i, cmp);
}


/**
 * @brief  乱択版選択アルゴリズム
 * @detail 配列A[p..r]のi番目に小さい(大きい)要素を返します
 * @note   期待実行時間はΘ(n)で抑えられる
 * @tparam Iterator イテレータ
 * @param  Iterator p       先頭イテレータ
 * @param  Iterator r       末尾イテレータ
 * @param  std::ptrdiff_t i 整数i(1 <= i <= n)
 */
template <class Iterator, class Compare>
static Iterator _randselect(Iterator p, Iterator r, std::ptrdiff_t i, Compare cmp)
{
    using dif_t = typename std::iterator_traits<Iterator>::difference_type;
    const dif_t dN = std::distance(p, r);
    if (dN == 0) {  // 再帰が基底かどうか、すなわち、部分配列A[p..r]を構成する要素数が1かどうかを判断する
        return p;   // 基底ならばiは必ず1だから、pをi番目に小さい(大きい)要素を指すイテレータとして返す
    }

    Iterator q = randpart(p, r, cmp);            // ピボットをランダムに選択
    const dif_t dM = std::distance(p, q);
    const dif_t k  = dM + 1;                     // 部分配列A[p..q]の要素数k, すなわち、
                                                 // 分割数の下側の要素数にピボット要素数1を加えた数を計算する
    // A[q]がi番目の要素か判断し、
    if (i == k) {                                // A[q]がi番目の要素ならば、
        return q;                                // qを返す(ピボット値が答え)
    }
    // A[q]がi番目の要素でなければ、アルゴリズムはi番目の要素が2つの部分配列A[p..q-1]とA[q+1..r]のどちらに属するか決定する
    else if (i < k) {                            // i < kならば目的の要素は分割の下側に属するので、
        return _randselect(p, --q, i, cmp);      // 再帰的にその部分配列から選択する
    }
    else {                                       // i > kならば目的の要素は分割の上側に属する
        // A[p..r]の中でi番目に小さい要素より小さいk個の要素、すなわちA[p..q]に属する要素を既に知っているので、
        return _randselect(++q, r, i - k, cmp);  // 目標の要素はA[q+1..r]の中でi-k番目に小さい(大きい)要素であり、これを再帰的に選択する
    }
}



#endif  // end of __SELECTION_HPP__


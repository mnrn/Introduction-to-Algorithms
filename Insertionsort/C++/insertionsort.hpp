/**
 * @bfief 教育的な挿入ソート
 * @date  作成日     : 2016/01/27
 * @date  最終更新日 : 2016/01/27
 */


//****************************************
// インクルードガード
//****************************************

#ifndef __INSERTIONSORT_HPP_
#define __INSERTIONSORT_HPP_



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <iterator>
#include <functional>



//****************************************
// 関数の定義
//****************************************

/**
 * @brief  挿入ソートを行う
 * @tparam class Iterator イテレータ
 * @tparam class Compare  比較用パラメタ
 * @param  Iterator a0    先頭イテレータ
 * @param  Iterator aN    末尾の次を指すイテレータ
 * @param  Compare  cmp   比較述語
 */
template <class Iterator, class Compare>
void inssort(Iterator a0, Iterator aN, Compare cmp)
{
    using val_t = typename std::iterator_traits<Iterator>::value_type;
    
    Iterator j = a0;
    // for文の各繰り返しが開始されるときには、部分配列A[0..j-1]には
    // 開始時点でA[0..j-1]に格納されていた要素がソートされた状態で格納されている
    for (++j; j != aN; ++j) {
        // for文の本体が行っていることはA[j]を入れるべき場所が見つかるまでA[j-1], A[j-2],..を
        // それぞれ1つ右に移し、空いた場所にA[j]の値を挿入することである.
        const val_t key = *j;  // 比較用のキーを取り出す
        // a[j]をソート済みの列a[0..j-1]に挿入する
        Iterator i = j; --i; // i = j - 1
        Iterator k = j;      // k = i + 1
        while (k != a0 && cmp(key, *i)) {
            *k = *i;
            --i; --k;
        }
        *k = key;
    }
    // for文が停止するのはj >= A.length = nを満たすときである.ループの各繰り返しはjの値を1だけ増加させるから、
    // 停止時にj = nが成立する.ループ不変式のjにnを代入すると、部分配列A[0..n-1]には、開始時点でA[0..n-1]に
    // 格納されていた要素全体が格納されているが、これらの要素は既にソートされている. 部分配列A[0..n-1]が
    // 全体配列であることに注意すると、配列全体がソート済みであると結論できる
}


/**
 * @brief  挿入ソートを行う
 * @note   比較器を引数に指定しなかった場合この手続きが呼ばれる
 * @tparam class Iterator イテレータ
 * @param  Iterator a0    先頭イテレータ
 * @param  Iterator aN    末尾の次を指すイテレータ
 */
template <class Iterator>
inline void inssort(Iterator a0, Iterator aN)
{
    using val_t = typename std::iterator_traits<Iterator>::value_type;
    inssort(a0, aN, std::less<val_t>());
}



#endif  // end of __INSERTIONSORT_HPP__


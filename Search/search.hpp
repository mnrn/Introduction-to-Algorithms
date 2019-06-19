/**
 * @bfief 探索問題を扱います
 * @note  受け取るイテレータは基本的にランダムアクセスイテレータを想定しています
 * @date  作成日     : 2016/02/03
 * @date  最終更新日 : 2016/02/03
 */


//****************************************
// インクルードガード
//****************************************

#ifndef __SEARCH_HPP__
#define __SEARCH_HPP__



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <iterator>
#include <utility>



//****************************************
// 関数プロトタイプ
//****************************************

template <class Iterator, class Key, class Compare>
static Iterator _binsearch(Iterator x, Iterator z, const Key& k, Compare cmp);



//****************************************
// 関数の定義
//****************************************

/**
 * @brief  線形探索を行います
 * @tparam Iterator  イテレータ
 * @tparam Predicate イテレータのtype_value(の参照でもよい)を引数にとり、真偽値boolを返す述語(関数オブジェクト)
 * @param  Iterator  a0   先頭を指すイテレータ
 * @param  Iterator  aN   末尾の次を指すイテレータ
 * @param  Predicate pred 述語関数
 * @return 述語を満たす要素を指すイテレータ
 *         述語を満たす要素が存在しないときは末尾の次を指すイテレータを返します
 */
template <class Iterator, class Predicate>
Iterator linearsearch(Iterator a0, Iterator aN, Predicate pred)
{
    // for文の各繰り返しの直前において、部分配列A[0..i-1]に
    // 含まれるおのおのの要素は述語を満たさない
    for (Iterator i = a0; i != aN; ++i) {
        if (pred(*i)) {  // 述語を満たす要素が存在した場合、
            return i;    // その要素を指すイテレータを返す
        }
    }
    return aN;
}


/**
 * @brief  既ソート配列Aに対して2分探索を行います
 * @note   配列Aの用素数をnとすると、最悪実行時間はΘ(lgn)
 * @tparam Iterator イテレータ
 * @tparam Key      キーの型(Iteratorのtype_value)
 * @tparam Compare  比較用関数オブジェクト
 * @param  Iterator   x 先頭イテレータ 
 * @param  Iterator   z 末尾の次を指すイテレータ
 * @param  const Key& k 探したいキー
 * @param  Compare  cmp 比較述語
 * @return Keyを指すイテレータ
 *         Keyと同じ値が存在しない場合、入力配列Aの末尾の次を指すイテレータを返します
 */
template <class Iterator, class Key, class Compare>
Iterator binsearch(Iterator x, Iterator z, const Key& k, Compare cmp)
{
    Iterator nil = z;
    Iterator retval = _binsearch(x, z, k, cmp);
    return (*retval == k) ? retval : nil;
}


/**
 * @brief  2分探索の本体呼び出し
 * @note   第4引数を省略した場合、こちらが呼ばれます
 * @tparam Iterator イテレータ
 * @tparam Key      キーの型(Iteratorのtype_value)
 * @param  Iterator   x 先頭イテレータ 
 * @param  Iterator   z 末尾の次を指すイテレータ
 * @param  const Key& k 探したいキー
 * @return Keyを指すイテレータ
 *         Keyと同じ値が存在しない場合、配列Aの末尾の次を指すイテレータが返されます
 */
template <class Iterator, class Key>
Iterator binsearch(Iterator a0, Iterator aN, const Key& k)
{
    return binsearch(a0, aN, k, std::less<Key>());
}


/**
 * @brief  既ソート部分配列Aに対して2分探索を行います
 * @tparam Iterator イテレータ
 * @tparam Key      キーの型(Iteratorのtype_value)
 * @tparam Compare  比較用関数オブジェクト
 * @param  Iterator   x 先頭イテレータ 
 * @param  Iterator   z 末尾の次を指すイテレータ
 * @param  const Key& k 探したいキー
 * @param  Compare  cmp 比較述語
 */
template <class Iterator, class Key, class Compare>
Iterator _binsearch(Iterator x, Iterator z, const Key& k, Compare cmp)
{    
    using dif_t = typename std::iterator_traits<Iterator>::difference_type;
    const dif d = std::distance(x, z);
    if (d < 1) {  　// 入力配列の要素数が0のとき(先頭イテレータxと末尾の次を指すイテレータzの距離が0のとき、入力配列の要素数は0)
        return z;　 // 再帰は底をつく
    }
    
    Iterator  y = p; std::advance(y, d >> 1);
    if (!(cmp(*y, k) || cmp(k, *y))) {      // キーkを発見した場合、
        return y;                           // キーkを指すイテレータを返す
    }
    else if (cmp(*y, k)) {                  // 部分配列A[x..z]の中央値A[y]がキーkよりも小さい(大きい)場合、
        return _binsearch(x, --y, k, cmp);  // 部分配列A[x..y-1]に対して再び探索を行う
    }
    else {                                  // 部分配列A[x..z]の中央値A[y]がキーkよりも大きい(小さい)場合、
        return _binsearch(++y, z, k, cmp);  // 部分配列A[y+1..z]に対して再び探索を行う
    }
}




#endif  // end of __SEARCH_HPP__


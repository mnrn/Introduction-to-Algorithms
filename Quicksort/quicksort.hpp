/**
 * @bfief 教育的なクイックソート
 * @note  受け取るイテレータは基本的にランダムアクセスイテレータを想定しています
 * @date  作成日     : 2016/01/31
 * @date  最終更新日 : 2016/02/02
 */



//****************************************
// インクルードガード
//****************************************

#ifndef __QUICKSORT_HPP__
#define __QUICKSORT_HPP__



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <iterator>
#include <functional>
#include <utility>
#include <algorithm>
#include "xorshift.hpp"
#include "../insertionsort/insertionsort.hpp"
#include "../Heapsort/heapsort.hpp"
#include "../BitOp/bitop.hpp"



//****************************************
// 関数プロトタイプ
//****************************************

template <class Iterator, class Compare>
static void _qsort(Iterator p, Iterator r, Compare cmp);

template <class Iterator, class Compare>
static void _randqsort(Iterator p, Iterator r, Compare cmp);

template <class Iterator, class Compare>
static void _hoareqsort(Iterator p, Iterator r, Compare cmp);

template <class Iterator, class Compare>
static void _modifiedqsort(Iterator p, Iterator r, Compare cmp, std::ptrdiff_t k);

template <class Iterator, class Compare>
static void __modifiedqsort(Iterator p, Iterator r, Compare cmp, std::ptrdiff_t k);

template <class Iterator, class Compare>
static void _trqsort(Iterator p, Iterator r, Compare cmp);

template <class Iterator, class Compare>
static void _introsort(Iterator p, Iterator r, Compare cmp, std::size_t limit);



template <class Iterator, class Compare>
static Iterator part(Iterator p, Iterator r, Compare cmp);

template <class Iterator, class Compare>
static Iterator hoarepart(Iterator p, Iterator r, Compare cmp);

template <class Iterator, class Compare>
Iterator randpart(Iterator p, Iterator r, Compare cmp);

template <class Iterator, class Compare>
static Iterator med3part(Iterator p, Iterator r, Compare cmp);

template <class Iterator, class Compare>
static Iterator rdhoarepart(Iterator p, Iterator r, Compare cmp);



template <class T, class Compare>
static constexpr T med3(const T& x, const T& y, const T& z, Compare cmp);



//****************************************
// 関数の定義
//****************************************


/**
 * @brief  クイックソートの本体呼び出し
 * @tparam Iterator イテレータ
 * @rparam Compare  比較用関数オブジェクト
 * @param  Iterator a0 先頭イテレータ
 * @param  Iterator aN 末尾の次を指すイテレータ
 * @param  Compare cmp 比較述語
 */
template <class Iterator, class Compare>
void qsort(Iterator a0, Iterator aN, Compare cmp)
{
    _qsort(a0, --aN, cmp);
}

/**
 * @brief  クイックソートの本体呼び出し
 * @note   第3引数を省略した場合こちらが呼ばれます
 * @tparam Iterator イテレータ
 * @param  Iterator a0 先頭イテレータ
 * @param  Compare cmp 比較述語
 */
template <class Iterator>
void qsort(Iterator a0, Iterator aN)
{
    using val_t = typename std::iterator_traits<Iterator>::value_type;
    qsort(a0, aN, std::less<val_t>());
}

/**
 * @brief  乱択版クイックソートの本体呼び出し
 * @tparam Iterator イテレータ
 * @rparam Compare  比較用関数オブジェクト
 * @param  Iterator a0 先頭イテレータ
 * @param  Iterator aN 末尾の次を指すイテレータ
 * @param  Compare cmp 比較述語
 */
template <class Iterator, class Compare>
void randqsort(Iterator a0, Iterator aN, Compare cmp)
{
    _randqsort(a0, --aN, cmp);
}

/**
 * @brief  乱択版クイックソートの本体呼び出し
 * @note   第3引数を省略した場合こちらが呼ばれます
 * @tparam Iterator イテレータ
 * @param  Iterator a0 先頭イテレータ
 * @param  Compare cmp 比較述語
 */
template <class Iterator>
void randqsort(Iterator a0, Iterator aN)
{
    using val_t = typename std::iterator_traits<Iterator>::value_type;
    randqsort(a0, aN, std::less<val_t>());
}

/**
 * @brief  HOARE版クイックソートの本体呼び出し
 * @tparam Iterator イテレータ
 * @rparam Compare  比較用関数オブジェクト
 * @param  Iterator a0 先頭イテレータ
 * @param  Iterator aN 末尾の次を指すイテレータ
 * @param  Compare cmp 比較述語
 */
template <class Iterator, class Compare>
void hoareqsort(Iterator a0, Iterator aN, Compare cmp)
{
    _hoareqsort(a0, --aN, cmp);
}

/**
 * @brief  HOARE版クイックソートの本体呼び出し
 * @note   第3引数を省略した場合こちらが呼ばれます
 * @tparam Iterator イテレータ
 * @param  Iterator a0 先頭イテレータ
 * @param  Iterator aN 末尾の次を指すイテレータ
 */
template <class Iterator>
void hoareqsort(Iterator a0, Iterator aN)
{
    using val_t = typename std::iterator_traits<Iterator>::value_type;
    hoareqsort(a0, aN, std::less<val_t>());
}


/**
 * @brief  修正版クイックソートの本体呼び出し
 * @tparam Iterator イテレータ
 * @tparam Compare  比較用関数オブジェクト
 * @param  Iterator a0      先頭イテレータ
 * @param  Iterator aN      末尾の次を指すイテレータ
 * @param  Compare cmp      比較述語
 * @param  std::ptrdiff_t k 部分配列の要素数がk以下のとき、挿入ソートに切り替わります
 */
template <class Iterator, class Compare>
void modifiedqsort(Iterator a0, Iterator aN, Compare cmp, std::ptrdiff_t k)
{
    _modifiedqsort(a0, --aN, cmp, k);
}


/**
 * @brief  修正版クイックソートの本体呼び出し
 * @note   第3引数を省略した場合こちらが呼ばれます
 * @tparam Iterator イテレータ
 * @param  Iterator a0      先頭イテレータ
 * @param  Iterator aN      末尾の次を指すイテレータ
 * @param  std::ptrdiff_t k 部分配列の要素数がk以下のとき、挿入ソートに切り替わります
 */
template <class Iterator>
void modifiedqsort(Iterator a0, Iterator aN, std::ptrdiff_t k)
{
    using val_t = typename std::iterator_traits<Iterator>::value_type;
    _modifiedqsort(a0, --aN, std::less<val_t>(), k);
}


/**
 * @brief  修正版クイックソートの本体呼び出し
 * @note   第4引数を省略した場合こちらが呼ばれます
 * @tparam Iterator イテレータ
 * @tparam Compare  比較用関数オブジェクト
 * @param  Iterator a0      先頭イテレータ
 * @param  Iterator aN      末尾の次を指すイテレータ
 * @param  Compare cmp      比較述語
 */
template <class Iterator, class Compare>
void modifiedqsort(Iterator a0, Iterator aN, Compare cmp)
{
    _modifiedqsort(a0, --aN, cmp, 16);
}

/**
 * @brief  修正版クイックソートの本体呼び出し
 * @note   第3引数と第4引数を省略した場合こちらが呼ばれます
 * @tparam Iterator イテレータ
 * @tparam Compare  比較用関数オブジェクト
 * @param  Iterator a0      先頭イテレータ
 * @param  Iterator aN      末尾の次を指すイテレータ
 * @param  Compare cmp      比較述語
 */
template <class Iterator>
void modifiedqsort(Iterator a0, Iterator aN)
{
     using val_t = typename std::iterator_traits<Iterator>::value_type;
     _modifiedqsort(a0, --aN, std::less<val_t>(), 16);
}

/**
 * @brief  末尾再帰版クイックソートの本体呼び出し
 * @tparam Iterator イテレータ
 * @rparam Compare  比較用関数オブジェクト
 * @param  Iterator a0 先頭イテレータ
 * @param  Iterator aN 末尾の次を指すイテレータ
 * @param  Compare cmp 比較述語
 */
template <class Iterator, class Compare>
void trqsort(Iterator a0, Iterator aN, Compare cmp)
{
    _trqsort(a0, --aN, cmp);
}

/**
 * @brief  末尾再帰版クイックソートの本体呼び出し
 * @note   第3引数を省略した場合こちらが呼ばれます
 * @tparam Iterator イテレータ
 * @param  Iterator a0 先頭イテレータ
 * @param  Iterator aN 末尾の次を指すイテレータ
 */
template <class Iterator>
void trsort(Iterator a0, Iterator aN)
{
     using val_t = typename std::iterator_traits<Iterator>::value_type;
     trqsort(a0, aN, std::less<val_t>());
}


/**
 * @brief  イントロソートの本体呼び出し
 * @tparam Iterator イテレータ
 * @tparam Compare  比較用関数オブジェクト
 * @param  Iterator p        先頭を指すイテレータ
 * @param  Iterator r        末尾の次を指すイテレータ
 * @param  Compare cmp       比較述語
 */
template <class Iterator, class Compare>
void introsort(Iterator a0, Iterator aN, Compare cmp)
{
    using dif_t = typename std::iterator_traits<Iterator>::difference_type;
    const dif_t n = std::distance(a0, aN);
    std::size_t limit = (31 - nlz(n)) << 1;  // 再帰の深さの限界はfloor(lg(A.length)) * 2に設定
    _introsort(a0, --aN, cmp, limit);
}

/**
 * @brief  イントロソートの本体呼び出し
 * @note   第3引数を省略した場合、こちらが呼ばれます
 * @tparam Iterator イテレータ
 * @param  Iterator p        先頭を指すイテレータ
 * @param  Iterator r        末尾の次を指すイテレータ
 */
template <class Iterator>
void introsort(Iterator a0, Iterator aN)
{
    using val_t = typename std::iterator_traits<Iterator>::value_type;
    introsort(a0, aN, std::less<val_t>());
}










/**
 * @brief  クイックソート
 * @note   要素数nの入力配列上でのクイックソートアルゴリズムの最悪実行時間はΘ(n^2)である
 *         ただし、期待実行時間はΘ(nlgn)であり、表記に隠されている定数部分は極めて小さい
 * @tparam Iterator イテレータ
 * @tparam Compare  比較用関数オブジェクト
 * @param  Iterator p  先頭イテレータ
 * @param  Iterator r  末尾イテレータ
 * @param  Compare cmp 比較述語
 */
template <class Iterator, class Compare>
static void _qsort(Iterator p, Iterator r, Compare cmp)
{
    using dif_t = typename std::iterator_traits<Iterator>::difference_type;
    const dif_t d = std::distance(p, r);
    if (d < 1) {  // 入力配列の要素数が1のとき
        return;   // 再起は底をつく
    }
    
    // 分割: 配列A[p..r]を2つの(空の可能性もある)部分配列A[p..q-1]と
    // A[q+1..r]にA[p..q-1]のどの要素もA[q]以下となり、A[q+1..r]の
    // どの要素もA[q]以上となるように分割(再配置)する
    Iterator q = part(p, r, cmp);

    // 統治: 2つの部分配列A[p..q-1]とA[q+1..r]をクイックソートを
    // 再帰的に呼び出すことでソートする
    Iterator _q1 = q; --_q1; Iterator q1 = q; ++q1;
    _qsort(p, _q1, cmp);
    _qsort(q1, r, cmp);
}


/**
 * @brief  要素xを常にピボットとして選択し、部分配列A[p..r]を
 *         ピボットより大きい要素の集合と小さい要素の集合に2分割する
 * @note   部分配列Aの要素数をnとするとき、実行時間はΘ(n)で表される
 * @tparam Iterator イテレータ
 * @tparam Compare  比較用関数オブジェクト
 * @param  Iterator p  先頭イテレータ
 * @param  Iterator r  末尾イテレータ
 * @param  Compare cmp 比較述語
 * @return Iterator    ピボットとして選択されたイテレータ
 */
template <class Iterator, class Compare>
static Iterator part(Iterator p, Iterator r, Compare cmp)
{
    using val_t = typename std::iterator_traits<Iterator>::value_type;
    const val_t x = *r;                   // 要素x=A[r]をピボットとして選ぶ
    Iterator i = p; std::advance(i, -1);  // i = p - 1
    Iterator j = p;                       // j = p
    
    // for文の各繰り返しの直前では、任意の配列添字kに対して以下の命題が成立する
    // 1. p <= k < iならば、A[k] <= xである
    // 2. i + 1 <= k <= j - 1ならば、A[k] > xである
    // 3. k = rならば、A[k] = xである
    for (; j != r; ++j) {
        // A[j] > xの場合
        // ループはjに1を加えるだけである.jに1を加えると、A[j-1]に対して
        // 条件2が成り立ち、他の部分は変化しない
        
        // A[j] <= xの場合
        if (!cmp(x, *j)) {
            // この場合、iに1を加え、A[i]とA[j]を交換し、jに1を加える
            // 交換によってA[i] <= xが成立するので、条件1が成立する
            // 同様に交換によってA[j-1]に格納した要素がxより大きいことを
            // ループ不変式が保証するのでA[j-1] > xが成立する
            ++i;
            std::swap(*i, *j);
        }
    }
    // 終了時にはj = rである.したがって、配列のすべての要素は不変式に示された
    // 3つの集合のいずれかに属しており、配列内のすべての値は3つの集合,
    // x以下の値からなる集合、xより大きい値からなる集合、xのみからなる集合に分割された
    
    // ピボットxより大きい要素の中で最左のものをピボットと交換することによって、
    // ピボットをその正しい位置である配列の中央に移す
    ++i;
    std::swap(*i, *r);
    return i;  // そして、ピボットの新しい添字を返す
}


/**
 * @brief  乱択版クイックソート
 * @note   部分配列A[p..r]から要素を無作為に抽出し、それをピボットとして用いる
 * @tparam Iterator イテレータ
 * @tparam Compare  比較用関数オブジェクト
 * @param  Iterator p  先頭イテレータ
 * @param  Iterator r  末尾イテレータ
 * @param  Compare cmp 比較述語
 */
template <class Iterator, class Compare>
static void _randqsort(Iterator p, Iterator r, Compare cmp)
{
    using dif_t = typename std::iterator_traits<Iterator>::difference_type;
    const dif_t d = std::distance(p, r);
    if (d < 1) {  // 入力配列の要素数が1のとき
        return;   // 再起は底をつく
    }

    // 分割: 配列A[p..r]を2つの(空の可能性もある)部分配列A[p..q-1]と
    // A[q+1..r]にA[p..q-1]のどの要素もA[q]以下となり、A[q+1..r]の
    // どの要素もA[q]以上となるように分割(再配置)する
    Iterator q = randpart(p, r, cmp);

    // 統治: 2つの部分配列A[p..q-1]とA[q+1..r]をクイックソートを
    // 再帰的に呼び出すことでソートする
    Iterator _q1 = q; --_q1; Iterator q1 = q; ++q1;
    _randqsort(p, _q1, cmp);
    _randqsort(q1, r, cmp);
}


/**
 * @brief  乱択版partition
 * @note   ピボット要素を無作為に抽出するので平均的には
 *         入力配列はそれなりにうまく2分割されると期待できる
 * @tparam Iterator イテレータ
 * @tparam Compare  比較用関数オブジェクト
 * @param  Iterator p  先頭イテレータ
 * @param  Iterator r  末尾イテレータ
 * @param  Compare cmp 比較述語
 * @return Iterator    ピボットとして選択されたイテレータ
 */
template <class Iterator, class Compare>
Iterator randpart(Iterator p, Iterator r, Compare cmp)
{
    using dif_t = typename std::iterator_traits<Iterator>::difference_type;
    const dif_t d = xorshift128() % std::distance(p, r);
    Iterator    x = p; std::advance(x, d);
    std::swap(*x, *r);       // 最初に無作為に抽出した要素とA[r]を交換する
    return part(p, r, cmp);
}


/**
 * @brief  HOARE版クイックソート
 * @note   要素数nの入力配列上でのクイックソートアルゴリズムの最悪実行時間はΘ(n^2)である
 *         ただし、期待実行時間はΘ(nlgn)であり、表記に隠されている定数部分は極めて小さい
 * @tparam Iterator イテレータ
 * @tparam Compare  比較用関数オブジェクト
 * @param  Iterator p  先頭イテレータ
 * @param  Iterator r  末尾イテレータ
 * @param  Compare cmp 比較述語
 */
template <class Iterator, class Compare>
static void _hoareqsort(Iterator p, Iterator r, Compare cmp)
{
    using dif_t = typename std::iterator_traits<Iterator>::difference_type;
    const dif_t d = std::distance(p, r);
    if (d < 1) {  // 入力配列の要素数が1のとき、
        return;   // 再起は底をつく
    }
    
    // 分割: partitionは(元々、A[r]にあった)ピボット値を構成する2つの分割から分離する
    //      一方、hoare-partitionは(元々、A[p]にあった)ピボット値を2つの分割
    //      A[p..j]とA[j+1..r]のどちらかに置く. p <= j < rだから、
    //      この分割は常に自明(すなわち、片方が空になる状況)ではない
    Iterator q = hoarepart(p, r, cmp);

    // 統治: 2つの部分配列A[p..q]とA[q+1..r]をクイックソートを
    // 再帰的に呼び出すことでソートする
    _hoareqsort(p, q, cmp);
    _hoareqsort(++q, r, cmp);
}


/**
 * @brief C.A.Hoareによる元々の分割アルゴリズム
 *        部分配列A[p..r]を再配置する
 *
 * @note  部分配列A[p..r]が少なくとも2つの要素を含むとき、以下の3つの命題が成立する
 *        1. 部分配列A[p..r]の外側の要素を参照しないように添字iとjは動く
 *        2. hoare-partitionは、終了するとp <= j < rを満たす値jを返す
 *        3. hoare-partitionが終了したとき、A[p..j]の各要素はA[j+1..r]の各要素の値以下である
 *
 * @tparam Iterator イテレータ
 * @tparam Compare  比較用関数オブジェクト
 * @param  Iterator p  先頭イテレータ
 * @param  Iterator r  末尾イテレータ
 * @param  Compare cmp 比較述語 
 * @return Iterator    ピボットとして選択されたイテレータ
 */
template <class Iterator, class Compare>
static Iterator hoarepart(Iterator p, Iterator r, Compare cmp)
{
    using val_t = typename std::iterator_traits<Iterator>::value_type;
    const val_t x = *p;   // ピボットxにA[p]を選ぶ
    Iterator    i = --p;  // i = p - 1
    Iterator    j = ++r;  // j = r + 1
    
    while (true) {
        do { --j; } while(cmp(x, *j));
        do { ++i; } while(cmp(*i, x));
        // if文によるiとjの比較がなされる直前において、配列の任意の要素について、
        // A[p..i-1] <= x かつ A[j+1..r] >= xが成り立つ
        if (i < j) {   // i < jのとき、
            // i <= rかつj >= pを満たすので、while文の繰り返し直前において、
            // 添字iとjは部分配列A[p..r]の外側を参照しない
            std::swap(*i, *j);
        }
        else {         // i >= jのとき、
            return j;  // p <= j < rを満たす値jを返す
        }
    }
}


/**
 * @brief  3要素x, y, zの中央値(median-of-3)を取得する
 * @tparam T       要素の型
 * @tparam Compare 比較用関数オブジェクト
 * @param  const T& x  要素x
 * @param  const T& y  要素y
 * @param  const T& z  要素z
 * @param  Compare cmp 比較述語
 * @return constexpr T 3要素x, y, zの中央値(返り値は右辺値であることに注意)
 */
template <class T, class Compare>
static constexpr T med3(const T& x, const T& y, const T& z, Compare cmp)
{
    if (cmp(x, y)) {
        if (cmp(y, z)) { return y; }
        else { if (cmp(z, x)) { return x; } else { return z; } }
    }
    else {
        if (cmp(z, y)) { return y; }
        else { if (cmp(x, z)) { return x; } else { return z; } }
    }
}


/**
 * @brief  3要素の中央値による分割(3要素中央値(median-of-3)法)
 * @note   この方法では部分配列から無作為に3個の要素を抽出し、
 *         その中央値(真ん中の要素)をピボットとして採用する
 * @note   3要素中央値法はクイックソートの実行時間の下界Ω(nlgn)を漸近的に改善できない
 * @tparam Iterator イテレータ
 * @tparam Compare  比較用関数オブジェクト
 * @param  Iterator p  先頭イテレータ
 * @param  Iterator r  末尾イテレータ
 * @param  Compare cmp 比較述語
 * @return Iterator    ピボットとして選択されたイテレータ
 */
template <class Iterator, class Compare>
static Iterator med3part(Iterator p, Iterator r, Compare cmp)
{
    using val_t = typename std::iterator_traits<Iterator>::value_type;
    using dif_t = typename std::iterator_traits<Iterator>::difference_type;
    const Iterator& A = p;
    const dif_t n0 = xorshift128() % std::distance(p, r);
    const dif_t n1 = xorshift128() % std::distance(p, r);
    const dif_t n2 = xorshift128() % std::distance(p, r);

    // 3要素A[n0], A[n1], A[n2]の中央値をピボットxに選ぶ
    const val_t x  = med3(A[n0], A[n1], A[n2], cmp);

    // 以下、Hoareの分割アルゴリズムを用いる
    Iterator    i = --p;  // i = p - 1
    Iterator    j = ++r;  // j = r + 1
    
    while (true) {
        do { --j; } while(cmp(x, *j));
        do { ++i; } while(cmp(*i, x));
        // if文によるiとjの比較がなされる直前において、配列の任意の要素について、
        // A[p..i-1] <= x かつ A[j+1..r] >= xが成り立つ
        if (i < j) {   // i < jのとき、
            // i <= rかつj >= pを満たすので、while文の繰り返し直前において、
            // 添字iとjは部分配列A[p..r]の外側を参照しない
            std::swap(*i, *j);
        }
        else {         // i >= jのとき、
            return j;  // jを返す
        }
    }
}


/**
 * @brief  修正版クイックソート
 * @detail 入力が"ほとんど"ソートされているとき、高速な挿入ソートの長所を用いることで、
 *         クイックソートの実行時間を実質的に改善できる.要素数がk以下の部分配列上で
 *         クイックソートが呼ばれたときには、その配列をソートせず、そのまま返すことにする
 *         代わりに、クイックソートの最上位レベルの呼び出しが終了した後、ソートを完成するために
 *         挿入ソートを実行する
 * @note   このソーティングアルゴリズムの期待実行時間はΟ(nk + nlgn(n/k))である
 * @tparam Iterator イテレータ
 * @tparam Compare  比較用関数オブジェクト
 * @param  Iterator p        先頭イテレータ
 * @param  Iterator r        末尾イテレータ
 * @param  Compare cmp       比較用関数オブジェクト
 * @param  std::ptrdiff_t k  部分配列の要素数がk以下のとき、挿入ソートに切り替わります
 */
template <class Iterator, class Compare>
static void _modifiedqsort(Iterator p, Iterator r, Compare cmp, std::ptrdiff_t k)
{
    __modifiedqsort(p, r, cmp, k);
    // クイックソートの最上位レベルの呼び出しが終了した後、ソートを完成するために
    inssort(p, ++r, cmp);  // 挿入ソートを実行する
}


/**
 * @brief  修正版クイックソート
 * @detail 入力が"ほとんど"ソートされているとき、高速な挿入ソートの長所を用いることで、
 *         クイックソートの実行時間を実質的に改善できる.要素数がk以下の部分配列上で
 *         クイックソートが呼ばれたときには、その配列をソートせず、そのまま返すことにする
 *         代わりに、クイックソートの最上位レベルの呼び出しが終了した後、ソートを完成するために
 *         挿入ソートを実行する
 * @tparam Iterator イテレータ
 * @tparam Compare  比較用関数オブジェクト
 * @param  Iterator p        先頭イテレータ
 * @param  Iterator r        末尾イテレータ
 * @param  Compare cmp       比較用関数オブジェクト
 * @param  std::ptrdiff_t k  部分配列の要素数がk以下のとき、挿入ソートに切り替わります
 */
template <class Iterator, class Compare>
static void __modifiedqsort(Iterator p, Iterator r, Compare cmp, std::ptrdiff_t k)
{
    using dif_t = typename std::iterator_traits<Iterator>::difference_type;
    const dif_t d = std::distance(p, r);
    if (d < k) {  // 要素数がk以下の部分配列上でクイックソートが呼ばれたときには、
        return;   // その配列をソートせず、そのまま返すことにする
    }

    // 3要素中央値法を用いてピボットを選択する
    Iterator q = med3part(p, r, cmp);

    // 2つの部分配列A[p..q]とA[q+1..r]を修正版クイックソートを
    // 再帰的に呼び出すことでソートする
    __modifiedqsort(p, q, cmp, k);
    __modifiedqsort(++q, r, cmp, k);
}


/**
 * @brief  末尾再帰版クイックソート
 * @note   qsortの2つ目の再帰呼び出しは実際には必要なく、
 *         繰り返し制御構造を用いて避けることができる
 *         優れたコンパイラはこの末尾再帰(tail recursion)と呼ばれる技法を自動的に適用する
 * @note   最悪スタック深さ(stack depth)はΘ(lgn)
 * @tparam Iterator イテレータ
 * @tparam Compare  比較用関数オブジェクト
 * @param  Iterator p  先頭イテレータ
 * @param  Iterator r  末尾イテレータ
 * @param  Compare cmp 比較述語
 */
template <class Iterator, class Compare>
static void _trqsort(Iterator p, Iterator r, Compare cmp)
{
    using dif_t = typename std::iterator_traits<Iterator>::difference_type;
    dif_t d0 = std::distance(p, r);
    while (d0 > 0) {  // 部分配列の要素数が1になるまでループし続ける
        Iterator q = part(p, r, cmp);
        // _trqsort(p, q-1, cmp);
        // p = q+1; d0 = std::distance(p, r);

        const dif_t d1 = std::distance(p, q);  // q - p
        const dif_t d2 = std::distance(q, r);  // r - q
        Iterator _q1 = q; --_q1; Iterator q1 = q; ++q1;  // q+1とq-1を用意
        
        if (d1 < d2) {  // q - p < r - qの場合
            _trqsort(p, _q1, cmp);     // 部分配列A[p..q-1]に対して再帰呼び出しを行う
            p = q1;                    // pはq+1に更新され、ループを繰り返す
            d0 = std::distance(p, r);
        }
        else {          // q - p >= r - qの場合
            _trqsort(q1, r, cmp);      // 部分配列A[q+1..r]に対して再帰呼び出しを行う
            r = _q1;                   // rはq-1に更新され、ループを繰り返す
            d0 = std::distance(p, r);
        }
    }

    // 以下、3要素中央値法を用いた場合
    
    // while (d0 > 0) {  // 部分配列の要素数が1になるまでループし続ける
    //     Iterator q = med3part(p, r, cmp);  // 3要素中央値法を用いてピボットを選択

    //     const dif_t d1 = std::distance(p, q);    // q - p
    //     const dif_t d2 = std::distance(q, r);    // r - q
    //     Iterator q1 = q; ++q1;             // q+1を用意
        
    //     if (d1 < d2) {  // q - p < r - qの場合
    //         _trqsort(p, q, cmp);     // 部分配列A[p..q]に対して再帰呼び出しを行う
    //         p = q1;                    // pはq+1に更新され、ループを繰り返す
    //         d0 = std::distance(p, r);
    //     }
    //     else {          // q - p >= r - qの場合
    //         _trqsort(q1, r, cmp);      // 部分配列A[q+1..r]に対して再帰呼び出しを行う
    //         r = q;                     // rはqに更新され、ループを繰り返す
    //         d0 = std::distance(p, r);
    //     }
    // }
}



/**
 * @brief  イントロソート
 * @note   再帰の深さが全体配列Aの要素数nの対数lgnに達した場合、クイックソートからヒープソートに切り替わる
 * @note   したがって、最悪実行時間をΘ(nlgn)に抑えることができる
 * @tparam Iterator イテレータ
 * @tparam Compare  比較用関数オブジェクト
 * @param  Iterator p        先頭を指すイテレータ
 * @param  Iterator r        末尾の次を指すイテレータ
 * @param  Compare cmp       比較述語
 * @param  std::size_t limit 再帰の深さ
 */
template <class Iterator, class Compare>
static void _introsort(Iterator p, Iterator r, Compare cmp, std::size_t limit)
{
    using dif_t = typename std::iterator_traits<Iterator>::difference_type;
    const dif_t d = std::distance(p, r);
    if (d < 1) {                // 入力配列の要素数が1のとき、
        return;                 // 再起は底をつく
    }
    if (limit < 1) {            // 再帰のレベルが限界に達したとき、
        heapsort(p, ++r, cmp);  // ヒープソートに切り替わる
        return;                 // そして再帰は底をつく
    }    
    // 分割: partitionは(元々、A[r]にあった)ピボット値を構成する2つの分割から分離する
    //      一方、hoare-partitionは(元々、A[p]にあった)ピボット値を2つの分割
    //      A[p..j]とA[j+1..r]のどちらかに置く. p <= j < rだから、
    //      この分割は常に自明(すなわち、片方が空になる状況)ではない
    Iterator q = hoarepart(p, r, cmp);

    // 統治: 2つの部分配列A[p..q]とA[q+1..r]をイントロソートを
    // 再帰的に呼び出すことでソートする
    limit = limit-1;
    _introsort(p, q, cmp, limit);
    _introsort(++q, r, cmp, limit);
}



#endif  // end of __QUICKSORT_HPP__


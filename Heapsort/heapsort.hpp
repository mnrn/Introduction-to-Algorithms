/**
 * @bfief 教育的なヒープソート
 * @note  受け取るイテレータは基本的にランダムアクセスイテレータを想定しています
 * @date  作成日     : 2016/02/09
 * @date  最終更新日 : 2016/02/10
 */



//****************************************
// インクルードガード
//****************************************

#ifndef __HEAPSORT_HPP__
#define __HEAPSORT_HPP__



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <utility>
#include <iterator>
#include "../Selection/selection.hpp"



//****************************************
// 関数の定義
//****************************************

/**
 * @brief  ヒープソートアルゴリズム
 * @note   最悪実行時間はΘ(nlgn)
 * @tparam Iterator イテレータ
 * @tparam Compare  比較用オブジェクト
 * @param  Iterator a0  先頭を指すイテレータ
 * @param  Iterator aN  末尾の次を指すイテレータ
 * @param  Compare  cmp 比較述語
 */
template <class Iterator, class Compare>
void heapsort(Iterator a0, Iterator aN, Compare cmp)
{
    using dif_t = typename std::iterator_traits<Iterator>::difference_type;
    if (std::distance(a0, aN) < 2) { return; }           // 要素数が1以下の配列はソートしません
    std::swap(*minmaxel(a0, aN, cmp), *a0);              // one-basedで考えるため、先に最小(最大)の値を配列の先頭に置く
    Iterator a1 = a0, a  = a0, i = ++a1;                 // イテレータを準備
    for (++i; i != aN; ++i) {                            // ヒープの構築を行う
        dif_t c = std::distance(a0, i), p = c / 2;       // 挿入対象となる節点cと その親pを取得する
        while (c > 1 && cmp(a[p], a[c])) {               // ヒープ木を根に向かって辿ってゆく
            std::swap(a[c], a[p]); c = p; p = p / 2;     // ヒープ条件に違反しているのでa[c]とa[p]を交換し、cをp, pをp.pに更新して、再びループする(木を上る)
        }                                                // ヒープ構築後、節点c0, c1のうち小さい(大きい)ほうをrに代入するラムダ式を準備し、ソートを行う
    }auto set = [=](auto c, auto&r, auto s) { for(int i=0;i<2;i++) { if (c[i] <= s && cmp(a[r], a[c[i]])) { r = c[i]; } } };
    for (i = aN, --i; i != a1; --i) {                    // ループの最初でA[n-1..i]は常にソートされていることに注意
        std::swap(a[1], *i);                             // 最大(最小)要素を正しい最終位置に置く
        dif_t hpsize = std::distance(a1, i), p = 1;      // ヒープサイズを1つ減らしてヒープから節点iを削除する
        while (true) {                                   // ヒープ条件に違反した可能性があるので、ヒープ条件を回復する
            dif_t c[2] = { p * 2, p * 2 + 1 };           // 節点pの左右の子を取得
            dif_t lst = p; set(c, lst, hpsize);          // lstに最も大きい(小さい)keyを持つ節点の添字を代入する
            if (lst == p) { break; }                     // このとき、lstが左右の子でないならば、ヒープ条件に違反はない
            std::swap(a[lst], a[p]); p = lst;            // lstが左右の子ならば、a[lst]とa[p]を交換し、pを適切な子に更新し、再びループする(木を下る)
        }
    }
}



/**
 * @brief  ヒープソートアルゴリズム
 * @note   第3引数を省略した場合こちらが呼ばれます
 * @tparam Iterator イテレータ
 * @param  Iterator a0  先頭を指すイテレータ
 * @param  Iterator aN  末尾の次を指すイテレータ
 */
template <class Iterator>
void heapsort(Iterator a0, Iterator aN)
{
     using val_t = typename std::iterator_traits<Iterator>::value_type;
     heapsort(a0, aN, std::less<val_t>());
}





/**
 * @brief  ヒープソートアルゴリズム
 * @note   最悪実行時間はΘ(lgn)
 * @note   zero-basedです.わかりやすさを殺さないために最適化はある程度コンパイラに任せています(たとえばhpszなんかは直接値入れればいい)
 * @tparam T       ソート対象の配列要素の型
 * @tparam Compare 比較用関数オブジェクト
 * @param  T a              配列A
 * @param  std::ptrdiff_t n 配列Aのサイズ
 * @param  Compare cmp      比較述語
 */
template <class T, class Compare>
void _heapsort(T a, /*Iterator aN,*/std::ptrdiff_t n, Compare cmp)
{
    if (n < 2) { return; }   // 要素数が1以下の配列はソートしません
    using dif_t = std::ptrdiff_t;

    auto heapify = [](T a, dif_t p, dif_t hpsz, Compare cmp) {
        while (true) {                                  // ヒープ条件に違反した可能性があるので、ヒープ条件を回復する
            dif_t l = (p<<1)+1, r = (p<<1)+2, lst = p;  // 節点pの左右の子を取得
            if (l < hpsz && cmp(a[lst], a[l]))  { lst = l; }  // lstに最も大きい(小さい)keyを持つ節点の添字を代入する
            if (r < hpsz && cmp(a[lst], a[r]))  { lst = r; }
            if (lst == p) { break; }                    // このとき、lstが左右の子でないならば、ヒープ条件に違反はない
            std::swap(a[lst], a[p]); p = lst;           // lstが左右の子ならば、a[lst]とa[p]を交換し、pを適切な子に更新し、再びループする(木を下る)
        }
    };
    dif_t hpsz = n;  // for文の各繰り返しが開始される時点では、各節点i+1, i+2,...,n-1はあるヒープの根である(ループ不変式)
    for (dif_t i = n / 2; i >= 0; --i) {  // i < 0で手続きは終了. iを1減らすことで次の繰り返しに対するループ不変式が再び成立する
        heapify(a, i, hpsz, cmp);         // 節点iをヒープの根に修正する. またこの呼び出しは節点i+1,i+2,...,n-1がヒープの根であるという性質を保存する
    }  // 手続きはi < 0で終了する.ループ不変式から、各節点0,1,..,n-1はヒープの根である.特に節点0はヒープの根である
    
    for (dif_t i = n - 1; i > 0; --i) {   // ループの最初でA[n-1..i]は常にソートされていることに注意
        std::swap(a[0], a[i]);            // 最大(最小)要素を正しい最終位置に置く
        hpsz = i;                         // ヒープサイズを1つ減らしてヒープから節点iを削除する
        heapify(a, 0, hpsz, cmp);         // ヒープ条件に違反した可能性があるのでヒープ条件を回復する
    }

    // 以下、一応残しておく
    
    // using dif_t = typename std::iterator_traits<T>::difference_type;
    
    // // zero-based arrayにおけるheap-treeの親子関係(最適化がかかれば、インライン化されるはず)
    // auto parent = [ ](std::size_t i) { return (i - 1) >> 1; };
    // auto left   = [ ](std::size_t i) { return (i << 1) + 1; };
    // auto right  = [ ](std::size_t i) { return (i + 1) << 1; };
    
    // for (dif_t i = 1;  i < n; ++i) {                    // ヒープの構築を行う
    //     dif_t c = i, p = parent(c);                     // 挿入対象となる節点cと その親pを取得する
    //     while (c > 0 && cmp(a[p], a[c])) {              // ヒープ木を根に向かって辿ってゆく
    //         std::swap(a[p], a[c]);                      // ヒープ条件に違反しているのでa[c]とa[p]を交換し
    //         c = p, p = parent(p);                       // cをp, pをp.pに更新して、再びループする(木を上る)
    //     }
    // }

    // dif_t c;  // 再構築対象となるヒープの根pの子cを宣言する
    // while ((c = (p<<1)+1) < hpsz) {  // 節点cを左の子とし、ヒープサイズ以上になるまでヒープ木を根に向かって辿ってゆく
    //     if (c+1 < hpsz && cmp(a[c], a[c+1])) { ++c; }  // p右の子c+1が存在する場合、左右の子(c, c+1)を比較する
    //     if (!cmp(a[p], a[c])) { break; }  // このとき、pの子のうち大きい方がpよりも小さいならば、ヒープ条件を満たしているのでループを抜ける 
    //     std::swap(a[p], a[c]); p = c;     // pの方が小さいならば、pとpの適切な子を交換し、pを更新する
    // }
}


/**
 * @brief  ヒープソートアルゴリズムの本体呼び出し
 * @note   最悪実行時間はΘ(lgn)
 * @tparam Iterator イテレータ
 * @tparam Compare  比較用関数オブジェクト
 * @param  Iterator a0  先頭を指すイテレータ
 * @param  Iterator aN  末尾の次を指すイテレータ
 * @param  Compare cmp  比較述語
 */
template <class Iterator, class Compare>
void _heapsort(Iterator a0, Iterator aN, Compare cmp)
{
    using dif_t = typename std::iterator_traits<Iterator>::difference_type;
    dif_t n = std::distance(a0, aN);
    _heapsort(a0, n, cmp);
}


/**
 * @brief  ヒープソートアルゴリズムの本体呼び出し
 * @note   最悪実行時間はΘ(lgn)
 * @tparam Iterator イテレータ
 * @param  Iterator a0  先頭を指すイテレータ
 * @param  Iterator aN  末尾の次を指すイテレータ
 */
template <class Iterator>
void _heapsort(Iterator a0, Iterator aN)
{
    using val_t = typename std::iterator_traits<Iterator>::value_type;
    _heapsort(a0, aN, std::less<val_t>());
}


/**
 * @brief  ヒープソートアルゴリズム
 * @note   最悪実行時間はΘ(lgn)
 * @tparam T       配列Aの要素の型
 * @tparam U       typename iterator_traits<T>::difference_type
 * @tparam Compare 比較用関数オブジェクト
 * @param  T         a   配列A
 * @param  U         n   配列Aの要素数
 * @param  Compare   cmp 比較述語
 */
template <class T, class U, class Compare>
void hsort(T a, U n, Compare cmp)
{
    if (n < 2) { return; }  // 要素数が1以下の配列はソートしません.既にソート済みです
    auto heapify = [](T a, U p, U hpsz, Compare cmp) { U c; while ((c = (p<<1)+1) < hpsz) {  // 添字pを根とする部分木がヒープ条件を満たすようにする
            if (c+1 < hpsz && cmp(a[c], a[c+1])) { ++c; } if (!cmp(a[p], a[c])) { break; } std::swap(a[p], a[c]); p = c; } };
    for (U i = n / 2; i >= 0; --i) { heapify(a, i, n, cmp); }                         // ヒープの構築
    for (U i = n - 1; i > 0; --i)  { std::swap(a[0], a[i]); heapify(a, 0, i, cmp); }  // ソート

    // auto heapify = [](T a, dif_t p, dif_t hpsz, Compare cmp) {  // 添字pを根とする部分木がヒープ条件を満たすようにするためのlambda expression
    //     dif_t c; while ((c = (p<<1)+1) < hpsz) {           // pの左の子lがヒープサイズより小さいか確認(ほぼ完全2分木なので左の子がいないならループ終了)
    //         if (c+1 < hpsz && cmp(a[c], a[c+1])) { ++c; }  // pの右の子rが存在する場合、lと比較して大きい方をcとする(右の子は左の子がいるときのみ存在しうる)
    //         if (!cmp(a[p], a[c])) { break; }               // 節点pとcの値を比較しpのほうが大きいならば、ヒープ条件を満たすためループ終了
    //         std::swap(a[p], a[c]); p = c;                  // cのほうが大きならば、節点pとcを交換し、pをcに更新する(ヒープ木を下る)
    //     } };
}


/**
 * @brief  ヒープソートアルゴリズムの本体呼び出し
 * @note   最悪実行時間はΘ(lgn)
 * @tparam Iterator イテレータ
 * @tparam Compare  比較用関数オブジェクト
 * @param  Iterator a0  先頭を指すイテレータ
 * @param  Iterator aN  末尾の次を指すイテレータ
 * @param  Compare cmp  比較述語
 */
template <class Iterator, class Compare>
void hsort(Iterator a0, Iterator aN, Compare cmp)
{
    using dif_t = typename std::iterator_traits<Iterator>::difference_type;
    dif_t n = std::distance(a0, aN);
    hsort(a0, n, cmp);
}


/**
 * @brief  ヒープソートアルゴリズムの本体呼び出し
 * @note   最悪実行時間はΘ(lgn)
 * @tparam Iterator イテレータ
 * @param  Iterator a0  先頭を指すイテレータ
 * @param  Iterator aN  末尾の次を指すイテレータ
 */
template <class Iterator>
void hsort(Iterator a0, Iterator aN)
{
    using val_t = typename std::iterator_traits<Iterator>::value_type;
    hsort(a0, aN, std::less<val_t>());
}



#endif  // end of __HEAPSORT_HPP__


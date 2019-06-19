/**
 * @brief ソートのテストプログラム
 * @date  作成日    : 2016/02/10
 * @date  最終更新日 : 2016/02/10
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <iostream>
#include <iterator>
#include <functional>
#include <utility>
#include <algorithm>



//****************************************
// 関数の定義
//****************************************



/**
 * @brief  符号なし整数vの先頭から続くゼロの数を数える
 * @note   IEEE 754形式をサポートしているアーキテクチャにのみ対応
 * @note   エンディアンはリトル、ビッグどちらにも対応
 * @param  std::uint32_t v 符号なし整数v
 * @return vのゼロの数
 */
std::int32_t nlz(std::uint32_t v)
{
    union { std::uint64_t asu64; double asf64;} u;  // 無名共用体を準備
    u.asf64 = (double)v + 0.5;                      // 0は例外表現なので0.5(1.0 * 2^(-1))を加算しておく
    return 1054 - (u.asu64 >> 52);                  // 1054(ゲタ(bias)の数+32-1) - vの指数部を返す
}



/**
 * @brief  ヒープソートアルゴリズム
 * @note   最悪実行時間はΘ(lgn)
 * @note   zero-basedです.わかりやすさを殺さないために最適化はある程度コンパイラに任せています(たとえばhpszなんかは直接値入れればいい)
 * @tparam T       ソート対象の配列要素の型
 * @tparam Compare 比較用関数オブジェクトの型
 * @param  T a              配列A[0..n-1]
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
}


template <class Iterator, class Compare>
static Iterator hoarepart(Iterator p, Iterator r, Compare cmp)
{
    using val_t = typename std::iterator_traits<Iterator>::value_type;
    using dif_t = typename std::iterator_traits<Iterator>::difference_type;
    const Iterator& A = p;
    const dif_t d = xorshift128() % std::distance(p, r);

    const val_t x  = *p;  // ピボットxをA[p]に選ぶ
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


template <class Iterator, class Compare>
static void _introsort(Iterator p, Iterator r, Compare cmp, std::size_t limit)
{
    using dif_t = typename std::iterator_traits<Iterator>::difference_type;
    const dif_t d = std::distance(p, r);
    if (d < 1) {  // 入力配列の要素数が1のとき、
        return;   // 再起は底をつく
    }
    if (limit < 1) {
        _heapsort(p, d+1, cmp);
        return;
    }
    
    // 分割: partitionは(元々、A[r]にあった)ピボット値を構成する2つの分割から分離する
    //      一方、hoare-partitionはピボット値を2つの分割
    //      A[p..j]とA[j+1..r]のどちらかに置く. p <= j < rだから、
    //      この分割は常に自明(すなわち、片方が空になる状況)ではない
    Iterator q = hoarepart(p, r, cmp);

    // 統治: 2つの部分配列A[p..q]とA[q+1..r]をイントロソートを
    // 再帰的に呼び出すことでソートする
    limit = limit-1;
    _introsort(p, q, cmp, limit);
    _introsort(++q, r, cmp, limit);
}


template <class Iterator, class Compare>
void introsort(Iterator a0, Iterator aN, Compare cmp)
{
    using dif_t = typename std::iterator_traits<Iterator>::difference_type;
    const dif_t n = std::distance(a0, aN);
    std::size_t limit = (31 - nlz(n)) << 1;  // 再帰の深さの限界はfloor(lg(A.length)) * 2に設定
    _introsort(a0, --aN, cmp, limit);
}

template <class Iterator>
void introsort(Iterator a0, Iterator aN)
{
    using val_t = typename std::iterator_traits<Iterator>::value_type;
    introsort(a0, aN, std::less<val_t>());
}



int main(void)
{
    using namespace std;
    cin.tie(0);
    ios::sync_with_stdio(0);
    int n;
    cin >> n;
    static int a[1000000];
    for (int i = 0; i < n; i++) cin >> a[i];
    introsort(a, a + n);
    for (int i = 0; i < n; i++) cout<< a[i] << ((i == n - 1) ? '\n' : ' ');

    return 0;
}

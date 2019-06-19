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
#include <vector>
#include <algorithm>



//****************************************
// 関数プロトタイプ
//****************************************

template <class Iterator, class Compare, class T, class U>
static void msort(Iterator aP, Iterator aQ, Compare cmp, T b0, U N);

template <class Iterator, class Compare, class T>
static void merge(Iterator aP, Iterator aQ, Iterator aR, Compare cmp, T b0);



//****************************************
// 関数の定義
//****************************************

/**
 * @brief マージソート
 * @tparam Iterator     イテレータ
 * @tparam Compare      比較用パラメタ
 * @param  Iterator a0  先頭イテレータ
 * @param  Iterator aN  末尾の次を指すイテレータ
 * @param  Compare  cmp 比較述語
 */
template <class Iterator, class Compare>
void msort(Iterator a0, Iterator aN, Compare cmp)
{
    using val_t = typename std::iterator_traits<Iterator>::value_type;
    using dif_t = typename std::iterator_traits<Iterator>::difference_type;

    const dif_t n = std::distance(a0, aN);  // n = aN - a0(a0とaNの距離)
    const dif_t m = n / 2;                  // m = (aN - a0) / 2
    std::vector<val_t> b(m);                // マージ用記憶領域を確保

    msort(a0, aN, cmp, b.begin(), n);       // 本体手続き呼び出し
}


/**
 * @brief マージソート
 * @note   cmpを引数に渡さない場合こちらが呼ばれる
 * @tparam Iterator     イテレータ
 * @param  Iterator a0  先頭イテレータ
 * @param  Iterator aN  末尾の次を指すイテレータ
 */
template <class Iterator>
void msort(Iterator a0, Iterator aN)
{
    using val_t = typename std::iterator_traits<Iterator>::value_type;
    msort(a0, aN, std::less<val_t>());
}


/**
 * @brief マージソート
 * @note  実行時間はΘ(nlgn)
 */
template <class Iterator, class Compare, class T, class U>
static void msort(Iterator aP, Iterator aR, Compare cmp, T b0, U n)
{
    if (n < 2) {  // nが1のとき、再帰は底をつく
        return;
    }

    // ソートすべき長さnの列を2つの長さn/2の部分列に分割する(分割)
    U q = n / 2;
    Iterator aQ = aP; std::advance(aQ, q);  // aQ = aP + q

    // マージソートを用いて2つの部分列を再帰的にソートする(統治)
    msort(aP, aQ, cmp, b0, q);      // a[p..q)のソート
    msort(aQ, aR, cmp, b0, n - q);  // a[q..r)のソート

    // 2つのソートされた部分列をマージしてソートされた解をつくる(結合)
    merge(aP, aQ, aR, cmp, b0);
}

/**
 * @brief 2つの既ソート列のマージ(結合)を行う
 * @note  nを2つの配列の要素数の和とすると、実行時間はΘ(n)
 */
template <class Iterator, class Compare, class T>
static void merge(Iterator aP, Iterator aQ, Iterator aR, Compare cmp, T b0)
{
    T li = b0;                     // L[0..q-p+1)
    T bM = std::copy(aP, aQ, b0);  // A[p..q)を記憶領域Bにコピー

    Iterator rj = aQ;              // R[0..r-q)
    Iterator ak = aP;              // A[p..r)

    // ループ不変式を維持しつつ基本操作を繰り返し実行する
    for (; li != bM && rj != aR; ++ak) {

        // L[i] < R[j]かどうか判定する(ただし昇順の場合で降順の場合は逆)
        if (cmp(*li, *rj)) {  // L[i] < R[j](L[i] > R[j])ならば
            *ak = *li;        // A[k] = L[i]
            ++li;             // i = i + 1
        }
        else {                // L[i] >= R[j](L[i] <= R[j])ならば、
            *ak = *rj;        // A[k] = R[j]
            ++rj;             // j = j + 1
        }
    }

    // 残り部分をコピー
    std::copy(li, bM, ak);
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
    msort(a, a + n);
    for (int i = 0; i < n; i++) cout<< a[i] << ((i == n - 1) ? '\n' : ' ');

    return 0;
}

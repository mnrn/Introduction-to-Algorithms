/**
 * @brief ソートのテストプログラム
 * @date  作成日    : 2016/02/10
 * @date  最終更新日 : 2016/02/10
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <iostream>
#include <utility>



//****************************************
// 関数の定義
//****************************************


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



int main(void)
{
    using namespace std;
    cin.tie(0);
    ios::sync_with_stdio(0);
    int n;
    cin >> n;
    static int a[1000000];
    for (int i = 0; i < n; i++) cin >> a[i];
    hsort(a, a + n);
    for (int i = 0; i < n; i++) cout<< a[i] << ((i == n - 1) ? '\n' : ' ');

    return 0;
}

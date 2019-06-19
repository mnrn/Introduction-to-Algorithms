/**
 * @brief 2分ヒープ
 * @date  作成日     : 2016/02/15
 * @date  最終更新日 : 2016/02/15
 */



//****************************************
// インクルードガード
//****************************************

#ifndef __BINHEAP_HPP__
#define __BINHEAP_HPP__



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <functional>
#include <utility>
#include <algorithm>
#include <vector>
#include <cassert>



//****************************************
// 構造体の定義
//****************************************

/**
 * @brief  2分ヒープ
 * @tparam class T         ヒープ木の要素の型(copyおよびmove可能であることを要求する)
 * @tparam class Compare   比較用関数オブジェクト(デフォルトでmaxヒープを構築する)
 * @tparam class Allocator アロケータ
 */
template <class T,
          class Compare   = std::less<T>,
          class Allocator = std::allocator<T>>
struct binheap {

    std::size_t size;             /**< 配列Aに格納されているヒープの要素数 */
    std::size_t length;           /**< 配列Aが含む要素数 */
    std::vector<T, Allocator> A;  /**< およそ完全2分木とみなせる配列A */
    Compare cmp;                  /**< 比較述語 */

    explicit binheap(std::size_t n) : size(0), length(n), A(n) {}
    template <class Iterator>
    binheap(Iterator a0, std::size_t n) : size(0), length(n), A(n)
    {
        std::copy(a0, a0 + n, A.begin());  // A[0..n-1]にコピー
    }
    template <class Iterator>
    binheap(Iterator a0, Iterator aN) : size(0)
    {
        typedef typename std::iterator_traits<Iterator>::difference_type dif_t;
        dif_t n = std::distance(a0, aN);
        length = static_cast<std::size_t>(n);
        A.resize(length);                  // Aの記憶領域確保
        std::copy(a0, aN, A.begin());      // A[0..n-1]にコピー
    }
    explicit binheap(const std::vector<T>&& v)
        : size(0), length(v.size()), A(std::move(v)) /* A[0..n-1]にムーブ */ {}

    

    /**
     * @brief max(min)ヒープ条件を回復する
     * @note  heapifyはA[i]の値をmax(min)ヒープの中に”滑り落とし”、
     *        添字iを根とする部分木がmax(min)ヒープ条件を満たすようにする
     * @note  実行時間はΟ(lgn)
     * @param std::size_t i ヒープ木の節点i
     */
    void heapify(std::size_t i)
    {
        std::size_t l = left(i);   // 節点iの左の子を取り出す
        std::size_t r = right(i);  // 節点iの右の子を取り出す

        // A[i], A[l], A[r]の中から最大値または最小値の添字を格納する
        std::size_t lhs = i;
        if (l < size && cmp(A[lhs], A[l])) {
            lhs = l;
        }
        if (r < size && cmp(A[lhs], A[r])) {
            lhs = r;
        }
        // lhsが左右の子であるか判定
        if (lhs != i) {  // 左右の子であるならば、
            std::swap(A[i], A[lhs]);  // A[i]とA[lhs]を交換する
            
            // lhsを根とする部分木がmax(min)ヒープ条件に違反する可能性がある
            heapify(lhs);  // そこで、max(min)ヒープ条件を回復するため再帰呼び出しを行う
        }
    }

    /**
     * @brief max(min)ヒープの構築を行う
     * @note  nをA.lengthとする.ボトムアップ的に手続きheapifyを利用することで
     *        配列A[0..n-1]をmax(min)ヒープに変換できる.部分配列A[floor(n/2)..n-1]の
     *        要素は全てこの木の葉であり、それぞれ1要素だけからなるヒープである
     *        手続きbuildは木の残りの節点に進み、それぞれの節点においてheapifyを実行する
     * @note  実行時間はΟ(n)
     */
    void build()
    {
        size = length;
        // for文の各繰り返しが開始される時点では、
        // 各節点i,i+1,..n-1はあるmax(min)ヒープの根である
        for (std::int32_t i = length / 2;
             i >= 0;     // i = -1で手続き終了
             --i) {      // iを1減らすことで次の繰り返しに対するループ不変式が再び成立する
            heapify(i);  // 節点iをmax(min)ヒープの根に修正する
            // また、このheapify呼び出しは節点i,i+1,..,n-1がmax(min)ヒープの根であるという性質を保存する
        }
        // 手続きはi = -1で終了する.ループ不変式から、各節点0,1,..n-1はmax(min)ヒープの根である
        // 特に節点0はmax(min)ヒープの根である
    }

    /**
     * @brief ヒープソート
     * @note  n = A.lengthとする.ヒープソートアルゴリズムは以下のように働く
     *
     *        最初に、buildを用いて入力配列A[0..n-1]上でmax(min)ヒープを構築する
     *        配列の最大(最小)要素は根A[0]に格納されているので、A[n-1]と交換することで
     *        最大(最小)要素を正しい最終位置に置く.次に、A.heap-sizeを1だけ減らして
     *        ヒープから節点n-1を削除する.このとき、根の左右の子はともにmax(min)ヒープを形成するが、
     *        更新された根はmax(min)ヒープ条件に違反する可能性がある.そこで、heapify(0)を
     *        呼び出してmax(min)ヒープ条件を修復すると、A[0..(n-2)]上にmax(min)ヒープが構築される
     *        この操作をサイズn-1のmax(min)ヒープからサイズ2のヒープまで繰り返す
     *
     * @note  最悪実行時間はΩ(nlgn)
     */
    void sort()
    {
        // 最初に、buildを用いて入力配列A[0..n-1]上でmax(min)ヒープを構築する
        build();
        // for文の各繰り返しの直前では、部分配列A[0..i]はA[0..n-1]の小さい(大きい)ほうから
        // i個の要素を含むmax(min)ヒープであり、部分配列A[i+1..n-1]はA[0..n-1]の大きい(小さい)ほうから
        // n - i個の要素をソートされた順序で含む
        for (std::size_t i = length - 1; i > 0; --i) {
            // 最大(最小)要素を正しい最終位置に置く
            std::swap(A[0], A[i]);
            // A.heap-sizeを1だけ減らしてヒープから節点iを削除する
            size = size - 1;
            // 更新された根はmax(min)ヒープ条件に違反する可能性があるので修復する
            heapify(0);
        }
    }

    /**
     * @brief []演算子オーバーオード
     *        配列Aに関する参照を返します
     * @param std::size_t i 配列インデックス
     */
    T& operator[](std::size_t i)
    {
        return A[i];
    }


    //**********************************************************************
    // 優先度付きキューに関する実装
    // 優先度付きキューはキーと呼ぶ値を持つ要素の集合Sを管理するためのデータ構造である
    //**********************************************************************

    /**
     * @brief Sから最大または最小のキーを持つSの要素を削除し、その要素を返す
     * @note  実行時間はΟ(lgn)
     */
    T extract()
    {
        assert(size > 0);    // ヒープアンダーフローチェック
        T root = A[0];       // 根を取り出す
        A[0] = A[size - 1];  // 根を更新
        size = size - 1;     // ヒープサイズの更新
        heapify(0);          // max(min)ヒープの再構築
        return root;         // 最大または最小のキーを持つ要素を返す
    }

    /**
     * @brief 要素iのキーの値を新しいキー値keyに変更する
     * @note  ただしmaxヒープにおいてkey > A[i],
     *        minヒープにおいてkey < A[i]を仮定する
     *        実行時間はΟ(lgn)
     * @param std::size_t i 節点i
     * @param T key         節点iの新しいキー値
     * @param bool check    キーの値をチェックするか？
     */
    void update(std::size_t i, const T& key, bool check = true)
    {   
        // 新しいキーは現在のヒープにふさわしいか判定する
        if (check && cmp(key, A[i])) {
            return;
        }
        A[i] = key;  // キー値A[i]を新しい値に変更

        // 節点から根に至る道を辿る
        // 要素のキー値が親より小さくなる(大きくなる)とmax(min)ヒープ条件が成立し,
        // 手続きを終了する
        while (i > 0 && cmp(A[parent(i)], A[i])) {
            std::swap(A[i], A[parent(i)]);  // 親のキー値との交換
            i = parent(i);                  // 根に至る道を辿る
        }
    }

    /**
     * @brief 集合Sに要素xを挿入する
     * @note  実行時間はΟ(lgn)
     * @param T key 要素x
     */
    void insert(const T& key)
    {
        assert(size < length);         // ヒープオーバーフローチェック
        size = size + 1;               // A.heap-sizeを1つ増やす
        update(size - 1, key, false);  // A[size]のキー値を変更する
    }

    /**
     * @brief ヒープから節点iの要素を削除する
     * @note  実行時間はΟ(lgn)
     * @param std::size_t i 節点i
     */
    void erase(std::size_t i)
    {
        assert(size > 0);    // ヒープアンダーフローチェック
        A[i] = A[size-1];    // A[i]にA[size-1]を代入
        size = size - 1;     // A.heap-sizeを1つ減らす
        heapify(i);          // max(min)ヒープの再構築
    }

    
private:
    /**< @brief 節点の添字iから親を示す添字を計算して返す */
    constexpr std::size_t parent(std::size_t i) const noexcept {
        return ((i - 1) >> 1);
    }
    /**< @brief 節点の添字iから左の子を示す添字を計算して返す*/
    constexpr std::size_t left(std::size_t i) const noexcept {
        return ((i << 1) + 1);
    }
    /**< @brief 節点の添字iから右の子を示す添字を計算して返す*/
    constexpr std::size_t right(std::size_t i) const noexcept {
        return ((i + 1) << 1);
    }
};



#endif  // end of __BINHEAP_HPP__


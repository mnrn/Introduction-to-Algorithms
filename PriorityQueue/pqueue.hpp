/**
 * @brief 優先度付きキュー(priority-queue)の実装
 * @date  作成日     : 2016/02/15
 * @date  最終更新日 : 2016/02/15
 */



//****************************************
// インクルードガード
//****************************************

#ifndef __PQUEUE_HPP__
#define __PQUEUE_HPP__



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <functional>
#include <utility>
#include <algorithm>
#include <vector>
#include "binheap.hpp"



//****************************************
// 構造体の定義
//****************************************

/**
 * @brief  優先度付きキュー
 * @tparam class T         優先度付きキューに格納する要素の型
 * @tparam class Compare   比較用関数オブジェクト(デフォルトでminヒープを構築する)
 * @tparam class Allocator アロケータ
 */
template <class T,
          class Compare   = std::greater<T>,
          class Allocator = std::allocator<T>
          >
struct pqueue {
    binheap<T, Compare, Allocator> H;

    explicit pqueue(std::size_t size) : H(size) { }
    explicit pqueue(const std::vector<T>&& v) : H(std::move(v))
    {
        H.build();  // ヒープを構築
    }

    /**
     * @brief 集合Sに要素xを挿入する
     * @note  実行時間はΟ(lgn)
     * @param T key 要素x
     */
    void insert(const T& key)
    {
        H.insert(key);
    }

    /**
     * @brief 要素iのキーの値を新しいキー値keyに変更する
     * @note  ただしmaxヒープにおいてkey > A[i],
     *        minヒープにおいてkey < A[i]を仮定する
     *        実行時間はΟ(lgn)
     * @param std::size_t i 節点i
     * @param T key         節点iの新しいキー値
     */
    void update(std::size_t i, const T& key)
    {
        H.update(i, key);
    }
    
    /**
     * @brief Sから最大または最小のキーを持つSの要素を削除し、その要素を返す
     * @note  実行時間はΟ(lgn)
     */
    T extract()
    {
        return H.extract();
    }

     /**
     * @brief ヒープから節点iの要素を削除する
     * @note  実行時間はΟ(lgn)
     * @param std::size_t i 節点i
     */
    void erase(std::size_t i)
    {
        H.erase(i);
    }
    
    /**
     * @brief []演算子オーバーオード
     *        配列Aに関する参照を返します
     * @param std::size_t i 配列インデックス
     */
    T& operator[](std::size_t i)
    {
        return H[i];
    }

    /**
     * @brief キューが空かどうかを返す
     */
    bool empty()
    {
        return H.size == 0;
    }

    // template<class... Args>
    // void emplace(Args&&... args)
    // {
    //     assert(H.size < H.length);
    //     H.size = H.size + 1;
    //     H.update(H.size - 1, T(std::forward<Args>(args)...), false);
    // }
};



#endif  // end of __PQUEUE_HPP__

/**
 * @bfief 両端キュー(deque: double-ended-queue)
 * @date  作成日     : 2016/02/03
 * @date  最終更新日 : 2016/02/03
 */


//****************************************
// インクルードガード
//****************************************

#ifndef __DEQUE_HPP__
#define __DEQUE_HPP__



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <cassert>
#include <cstdint>



//****************************************
// 構造体の定義
//****************************************

/**
 * @brief  両端キュー(deque: double-ended-queue)
 * @tparam class   T キューに格納する要素の型
 * @tparam int32_t n 最大n個の要素を格納できます
 * @note   関数名はPerl, Ruby, JavaScriptに倣っています
 */
template <class T, std::int32_t n = 16>
struct deque {
    T Q[n];
    std::size_t  size;
    std::int32_t head;
    std::int32_t tail;

    deque() noexcept : size(0), head(0), tail(0) {}
    
    /**
     * @brief 両端キューが空かどうか返す
     */
    bool empty()
    {
        return size == 0;
    }

    /**
     * @brief 両端キューが満杯かどうか返す
     */
    bool full()
    {
        return size >= n;
    }

    /**
     * @brief 両端キューの末尾に要素xを挿入する
     * @param T x 要素x
     */
    void push(T x)
    {
        assert(!full());                   // オーバーフローチェック
        if (++tail >= n) { tail = 0; }     // 循環処理 
        Q[tail] = x;                       // 挿入
        size++;                            // サイズを1つ大きくする
    }

    /**
     * @brief 両端キューの先頭に要素xを挿入する
     * @param T x 要素x
     */
    void unshift(T x)
    {
        assert(!full());                   // オーバーフローチェック
        Q[head] = x;                       // 挿入
        if (--head < 0) { head = n - 1; }  // 循環処理
        size+;                             // サイズを1つ大きくする
    }

    /**
     * @brief  両端キューの末尾から要素xを削除し、取り出す
     * @return T 要素x
     */
    T pop()
    {
        assert(!empty());                  // アンダーフローチェック
        T x = Q[tail];                     // 削除
        if (--tail < 0) { tail = n - 1; }  // 循環処理
        size--;                            // サイズを1つ小さくする
        return x;                          // 取り出す
    }

    /**
     * @brief  両端キューの先頭から要素xを削除し、取り出す
     * @return T 要素x
     */
    T shift()
    {
        assert(!empty());                  // アンダーフローチェック
        if (++head >= n) { head = 0; }     // 循環処理
        T x = Q[head];                     // 削除
        size--;                            // サイズを1つ小さくする
        return x;                          // 取り出す
    }
};



#endif  // end of __DEQUE_HPP__


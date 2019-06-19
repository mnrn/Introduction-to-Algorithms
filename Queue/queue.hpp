/**
 * @brief キュー
 * @date  作成日     : 2016/01/25
 * @date  最終更新日 : 2016/02/03
 */



//****************************************
// インクルードガード
//****************************************

#ifndef __QUEUE_HPP__
#define __QUEUE_HPP__



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <cassert>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <type_traits>
#include "../Container/container.hpp"



//****************************************
// 構造体の定義
//****************************************

/**
 * @brief キュー
 * @tparam class T         キューの要素の型
 * @tparam class Allocator Tのアロケータ 
 */
template <class T>
struct queue {
    // std::vector<T, Allocator> Q;
    std::int32_t head, tail;
    std::int32_t _max;
    T* Q;
    
    explicit queue(std::int32_t n) 
    : head(0), tail(0), _max(n+1), Q(static_cast<T*>(::operator new(sizeof(T) * (n + 1))))  { };
    ~queue() noexcept
    {
        if (!std::is_trivially_destructible<T>::value) {
            if (head < tail) {
                std::for_each(Q + head, Q + tail, destroy<T>);
            }
            else if (head > tail) {
                std::for_each(Q, Q + tail, destroy<T>);
                std::for_each(Q + head, Q + _max, destroy<T>);
            }
        }
        ::operator delete(Q);
    };

    /**< @brief キューが空かどうか判定 */
    bool empty()
    {
        return head == tail;
    }

    /**< @brief キューが満杯かどうか判定 */
    bool full()
    {
        return ((tail + 1) % _max == head);
    }

    /**< @brief キューに要素xを格納する */
    template<class... Args>
    void enqueue(Args&&... args)
    {
        assert(!full());        // オーバーフローチェック
        construct(Q[tail], T(std::forward<Args>(args)...));
        tail = (tail + 1) % _max;  // 循環処理
    }

    /**< @brief キューから一番上の要素を取り出す */
    T dequeue()
    {
        assert(!empty());       // アンダーフローチェック
        T x = Q[head];
        destroy(Q[head]);
        head = (head + 1) % _max;  // 循環処理
        return x;
    }

    /**< @brief キューの一番上の要素を返す */
    T top()
    {
        return Q[head];
    }
};



#endif  // end of __QUEUE_H__


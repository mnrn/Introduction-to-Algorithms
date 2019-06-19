/**
 * @brief スタック
 * @date  作成日     : 2016/01/25
 * @date  最終更新日 : 2016/02/03
 */



//****************************************
// インクルードガード
//****************************************

#ifndef __STACK_HPP__
#define __STACK_HPP__



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <cassert>
#include <cstddef>
#include <algorithm>
#include <type_traits>
#include "../Container/container.hpp"



//****************************************
// 構造体の定義
//****************************************

/**
 * @brief  スタック
 * @tparam class   T スタックの要素の型
 */
template <class T>
struct stack {
    std::int32_t _top;
    std::int32_t _max;
    T* S;

    stack(std::size_t size)
    : _top(-1),  _max(size),
      S(static_cast<T*>(::operator new(sizeof(T) * size))) { } 
    ~stack() noexcept
    {
        if (!std::is_trivially_destructible<T>::value) {
            std::for_each(S, S+_top+1, destroy<T>);
        }
        ::operator delete(S);
    }

    /**< @brief スタックが空かどうかを返す */
    bool empty()
    {
        return _top == -1;
    }

    /**< @brief スタックが満杯かどうか返す */
    bool full()
    {
        return _top >= (_max - 1);
    }

    /**< @brief スタックにxをプッシュする  */
    template<class... Args>
    void push(Args&&... args)
    {
        assert(!full());   // オーバーフローチェック
        _top++;
        construct(S[_top], T(std::forward<Args>(args)...));
    }

    /**< @brief スタックから一番上の要素をポップして返す */
    T pop()
    {
        assert(!empty());  // アンダーフローチェック
        T x = S[_top];
        _top--;
        destroy(S[_top+1]);
        return x;
    }

    /**< @brief スタックから一番上の要素を返す */
    T top()
    {
        assert(!empty());
        return S[_top];
    }

    /**< @brief スタックのサイズを返す */
    std::size_t size()
    {
        return _top + 1;
    }
};



#endif  // end of __STACK_HPP__


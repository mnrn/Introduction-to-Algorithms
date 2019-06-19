/**
 * @brief  コンテナ用テンプレート置き場
 * @date   2016/02/23
 */



//****************************************
// インクルードガード
//****************************************

#ifndef __CONTAINER_HPP__
#define __CONTAINER_HPP__



//****************************************
// 名前空間の定義
//****************************************

//#define CONTAINER_BEGIN namespace container {
//#define CONTAINER_END   }

//CONTAINER_BEGIN



//****************************************
// 関数の定義
//****************************************

/**
 * @brief placement newを使用した生成用ヘルパ関数
 */
template <class T, class... Args>
void construct(T& t, Args&&... args)
{
    new(&t)T(std::forward<Args>(args)...);
}

/**
 * @brief 明示的にデストラクタを呼び出す破棄用ヘルパ関数
 */
template <class T>
void destroy(const T& t) noexcept
{
    t.~T();
}



//CONTAINER_END

#endif  // end of __CONTAINER_HPP__

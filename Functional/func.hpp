/**
 * @brief 関数型っぽいかんじにする
 * @date  2016/02/12
 */



//********************************************************************************
// インクルードガード
//********************************************************************************

#ifndef __FUNC_HPP__
#define __FUNC_HPP__



//********************************************************************************
// 必要なヘッダファイルのインクルード
//********************************************************************************

#include <functional>



//********************************************************************************
// 関数の定義
//********************************************************************************

/**
 * @brief  関数の部分適応を行う
 * @note   部分適応された関数は残りの変数を引数として受け取る関数である
 * @param  F f 部分適応対象の関数
 * @param  T a 関数fに適応させる値(第一引数)
 * @return 部分適応後の関数 
 */
template <class F, class T>
auto partial(F f, T a)
{
    return [f, a](auto ...args) { return f(a, args...); };
}


/**
 * @brief 関数のカリー化を行う
 * @note  カリー化関数は、複数の引数を取る代わりに、常にちょうど1つの引数を取る関数である
 * @note  カリー化関数が呼び出されると、その次の引数を受け取る関数を返し、それを繰り返す
 * @param F f カリー化対象の関数
 * @return カリー化された関数
 */
template <class F>
auto curry(F f)
{
    return [f](auto a) { return partial(f, a); };
}


/**
 * @brief  カリー化による再帰関数の実装
 *         循環なく関数の意味を定義できる
 * @note   C++においてはラムダ式の再帰実装などに使用できる
 * @note   背景には不動点コンビネータの概念が存在する
 * @param  F f 再帰関数化対象の関数
 * @return 再帰関数となった関数
 */
template <class F>
auto recursive(F f)
{
    return curry(f)(f);
}



#endif  // end of __FUNC_HPP__

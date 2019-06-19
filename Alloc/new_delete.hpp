/**
 * @brief new/deleteに関する物置
 * @date  作成日     : 2016/02/06
 * @date  最終更新日 : 2016/02/06
 */



//****************************************
// インクルードガード
//****************************************

#ifndef __NEWDELETE_HPP__
#define __NEWDELETE_HPP__



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <new>



//****************************************
// クラスの定義
//****************************************

/**
 * @brief effective c++ 52項にあるクラスのメモ
 *
 * @note  noexcept指定がなされているnewを使用しても、メモリ確保時に呼ばれる「コンストラクタ」が例外を呼び出さないとは限らない
 *        コンストラクタが例外を呼び出したとき、C++はランタイムでdeleteを呼ぶがnewに対応するdeleteが存在しない場合、
 *        C++はdeleteを呼び出さない.従って、newに対応するdeleteを定義する必要がある
 * 
 * @note  C++11ではメモリ解放関数とデストラクタはすべてdefaultで、ユーザー定義もコンパイラが生成したものも含め、
 *        暗黙にnoexceptとなる.したがって、それらの関数についてnoexceptと宣言しなくても良い
 */
class StandardNewDeleteForms {
public:
    
    // ordinary new/delete
    static void* operator new(std::size_t size) throw(std::bad_alloc)
    {
        return ::operator new(size); 
    }
    static void operator delete(void* pMem) noexcept
    {
        ::operator delete(pMem);
    }

    // placement new/delete
    static void* operator new(std::size_t size, void* ptr) noexcept
    {
        return ::operator new(size, ptr);
    }
    static void operator delete(void* pMem, void* ptr) noexcept
    {
        ::operator delete(pMem, ptr);
    }

    // nothrow new/delete
    static void* operator new(std::size_t size, const std::nothrow_t& nt) noexcept
    {
        return ::operator new(size, nt);
    }
    static void operator delete(void* pMem, const std::nothrow_t& nt) noexcept
    {
        ::operator delete(pMem);
    }
    
};






#endif  // end of __NEWDELETE_HPP__


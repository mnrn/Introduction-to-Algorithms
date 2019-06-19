/**
 * @brief 自作アロケータインターフェース用ヘッダ
 * @note  今のところほとんどデフォルトアロケータ
 * @date  作成日     : 2016/01/28
 * @date  最終更新日 : 2016/01/28
 */



//****************************************
// インクルードガード
//****************************************

#ifndef __ALLOC_IF_HPP__
#define __ALLOC_IF_HPP__



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <new>
#include <limits>



//****************************************
// 構造体の定義
//****************************************

template <class T>
struct AllocInterface {
    using value_type      = T;
    // using pointer         = T*;
    // using const_pointer   = const T*;
    // using reference       = T&;
    // using const_reference = const T&;
    // using size_type       = std::size_t;
    // using difference_type = std::ptrdiff_t;


    /**
     * @brief 記憶領域確保
     * @note  n個のT型のオブジェクトを確保できるに足るだけの記憶領域を返す
     */
    T* allocate(std::size_t n)
    {
        return (T*)( ::operator new(n * sizeof(T))); 
    }

    /**
     * @brief 記憶領域解放
     * @note  pが指すn個の記憶領域を解放する
     */
    void deallocate(T* p, std::size_t n)
    {
        static_cast<void>(n);
        ::operator delete((void*)p);
    }

    /**
     * @brief 確保済みの記憶領域を初期化する
     * @note  第1引数で指定した位置にある1個のオブジェクトを
     *        第2引数の値で初期化する
     */
    void construct(T* p, const T& value)
    {
        new ((void*)p) T(value);
    }

    /**
     * @brief オブジェクトのアドレスを返す
     */
    T* address(T& value) const
    {
        return &value;
    }
    const T* address(const T& value) const
    {
        return &value;
    }

    /**
     * @brief 初期化済みの記憶領域を削除する
     */
    void destroy(T* p)
    {
        p->~T();
    }

    /**
     * @brief 割り当てることのできる最大のサイズを返す
     */
    std::size_t max_size() const noexcept
    {
        return std::numeric_limits<std::size_t>::max() / sizeof(T);
    }

    /**< コンストラクタ */
    AllocInterface() noexcept {}
    
    /**< コピーコンストラクタ */
    AllocInterface(const MyAllocator&) noexcept {}

    /**< ムーブコンストラクタ */
    AllocInterface(const MyAllocator&&) noexcept {}

    /**< 別の要素型のアロケータを受け取るコンストラクタ */
    template <class U>
    AllocInterface(const MyAllocator<U>&) noexcept {}

    /**< デストラクタ */
    ~AllocInterface() noexcept {}

    /**< アロケータをU型にバインドする */
    template <class U>
    struct rebind {
        using other = AllocInterface<U>;
    };
};


/**
 * @brief ==演算子オーバーロード
 * @note テンプレート引数の違う他の型とも比較できなければならない
 *       a == bにおいて、互いのallocateで確保した記憶領域が、
 *       deallocateで解放できる場合はtrueを、そうでない場合はfalseを返す
 */
template <class T, class U>
bool operator==(const AllocInterface<T>&, const AllocInterface<U>&)
{
    return true;
}
/** !=演算子オーバーロード
 * @note ==演算子の否定を返す
 */
template <class T, class U>
bool operator!=(const AllocInterface<T>&, const AllocInterface<U>&)
{
    return false;
}



#endif  // end of __ALLOCATOR_HPP__


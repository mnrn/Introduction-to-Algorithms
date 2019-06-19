/**
 * @brief AVL木
 * @note  AVL木(AVL tree)は高さ平衡(height balanced)2分探索木である.すなわち、各節点xの左右の部分木の高さは高々1しか違わない
 *        AVL木を実現するために各節点に特別な属性を管理する. x.hはxの高さである
 *        他の任意の2分探索木Tと同様、T.rootはその根を指すものと仮定する
 *
 * @note  Fhをh番目のフィボナッチ数とするとき、高さhのAVL木には少なくともFh個の節点があることから
 *        n個のAVL木の高さはΟ(lgn)であることがわかる
 *
 * @note  今回は属性pを省略してコードの簡略化を図ることにする
 *
 * @date  作成日     : 2016/02/07
 * @date  最終更新日 : 2016/02/07
 */



//****************************************
// インクルードガード
//****************************************

#ifndef __AVLTREE_HPP__
#define __AVLTREE_HPP__



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <utility>
#include <algorithm>



//****************************************
// 構造体の定義
//****************************************

/**
 * @brief  AVL木
 * @tparam Key     キーの型
 * @tparam T       付属データの型
 * @tparam Compare キーを引数にとる関数
 */
template <class Key, class T, class Compare = std::less<Key>>
struct avltree {

    using height_t  = std::int32_t;
    using pair_t    = std::pair<const Key&, T>;

    struct node {
        node*left;       /**< 左の子    */
        node*right;      /**< 右の子    */
        height_t h;      /**< 高さ      */
        union {
            const Key key;              /**< キー    */
            const char d[sizeof(Key)];  /**< ダミー  */
        };
        T value;         /**< 付属データ */
        node*next;       /**< 一方向未使用リストL */

        constexpr node() noexcept
            : left(nullptr), right(nullptr), h(1), d("\0"), next(nullptr) { }
        constexpr explicit node(const Key& k) noexcept
            : left(nullptr), right(nullptr), h(1), key(k), next(nullptr) { }
        constexpr node(const Key& k, const T& v) noexcept
            : left(nullptr), right(nullptr), h(1), key(k), value(v), next(nullptr) { }
    };

    node*root;         /**< AVL木の根 */
    Compare cmp;       /**< 比較述語  */

    std::size_t size;  /**< AVL木の節点数           */
    node*pool;         /**< AVL木の節点用メモリプール */
    node*free;         /**< 空き節点へのポインタ    */
    

    avltree() = delete;
    explicit avltree(std::size_t n) : root(nullptr), size(n), pool(nullptr), free(nullptr)
    {
        pool = allocnodes(n);                     // 節点n個分の記憶領域確保
        for (std::size_t i = 0; i < size; i++) {  // 初期化
            pool[i].next = (i + 1 == size) ? nullptr : &pool[i + 1];
        }
        free = pool;  // 空き節点へのポインタを保持しておく
    }
    ~avltree()
    {
        freenodes(pool);  // 確保した記憶領域の解放
    }
    

    /**
     * @brief  中間順木巡回を行う
     * @note   n個の節点を持つ2分探索木の巡回はΘ(n)かかる
     * @tparam class F const Key&を引数に取る関数の型
     * @param F fn     const Key&を引数に取る関数
     */
    template <class F>
    void inorder(F fn)
    {
        inorder(root, fn);
    }

    /**
     * @brief AVL木Tにキーkの挿入を行う
     * @note  実行時間はΟ(lgn)
     * @param const Key& k キーk
     * @param const T& v   付属データv
     */
    void insert(const Key& k, const T& v)
    {
        root = insert(root, k, v);
    }
    
    /**
     * @brief AVL木Tからキーkを持つ節点の削除を行う
     * @note  実行時間はΟ(lgn)
     * @param const Key& k キーk
     */
    void erase(const Key& k)
    {
        root = erase(root, k);
    }

    /**
     * @brief  AVL木からキーkに対応する付属データを返す
     * @note   実行時間はΟ(lgn)
     * @param  const Key& k キーk
     * @return キーkに対応する付属データ
     */
    T* find(const Key& k)
    {
        return find(root, k);
    }

private:
    /**
     * @brief  中間順木巡回を行う
     * @note   n個の節点を持つ2分探索木の巡回はΘ(n)かかる
     * @tparam class F const Key&を引数に取る関数の型
     * @param F fn     const Key&を引数に取る関数
     * @param node*x   巡回を行う部分木の根
     */
    template <class F>
    void inorder(node*x, F fn)
    {
        if (x == nullptr) { return; }                             // xがNILを指すとき、再帰は底をつく
        inorder(x->left, fn); fn(x->key); inorder(x->right, fn);  // xの左右の子を根とする部分木に対して中間順木巡回を行う 
    }

    /**
     * @brief  AVL木からキーkに対応する付属データを返す
     * @note   実行時間はΟ(lgn)
     * @param  const Key& k キーk
     * @return キーkに対応する付属データへのポインタ
     */
    T* find(node* x, const Key& k)
    {
        while (x != nullptr && neq(x->key, k)) {
            if (cmp(x->key, k)) {
                x = x->left;
            }
            else {
                x = x->right;
            }
        }
        return x->v;
    }
    
    /**
     * @brief 節点xを根とする部分木にキーkの挿入を行う 
     * @param node*x       節点x
     * @param const Key& k キーk
     * @param const T& v   付属データv
     */
    node* insert(node*x, const Key& k, const T& v)
    {
        if (x == nullptr) { return allocnode(k, v); }  // xがNILを指すとき、再帰は底をつく
        cmp(k, x->key) ? x->left = insert(x->left, k, v) : x->right = insert(x->right, k, v);  // xの適切な子に再帰し、
        return balance(x);                             // xを根とする部分木を高さ平衡にする
    }

    /**
     * @brief 節点xを根とする部分木からキーkを削除する
     * @param node*x       節点x
     * @param const Key& k キーk
     */
    node* erase(node*x, const Key& k)
    {
        if (x == nullptr)   { return nullptr; }                                    // キーkはAVL木Tに存在しなかった
        if (cmp(k, x->key)) { x->left  = erase(x->left, k);  return balance(x); }  // xの適切な子に再帰し、
        if (cmp(x->key, k)) { x->right = erase(x->right, k); return balance(x); }  // xを根とする部分木を高さ平衡にする
        node*y = x->left, *z = x->right;  // x.key == kのとき、yをxの左の子、zをxの右の子とし、
        freenode(x);                      // xを解放する
        if (z == nullptr) { return y; }   // zがNILを指しているならば、新たな部分木の根としてyを返す
        node*w   = leftmost(z);           // zがNILを指していないならば、wをzを根とする部分木の中で最も左の子とする     
        w->right = _erase(z);             // zに対して再帰する.そして、wの右の子に新たな部分木を受け取る
        w->left  = y;                     // yをwの左の子にする
        return balance(w);                // wを根とする部分木を高さ平衡にして戻る
    }
    
    /**
     * @brief  節点xの左右の部分木はそれぞれ高さ平衡しており、その左右の子の高さの差は2以下であるとする
     *         xを入力とし、xを根とする部分木を高さ平衡になるように変換する
     * @note   回転は高々2回であるため実行時間はΟ(1)
     * @param  node*x 節点x
     * @return 高さ平衡な部分木の根 
     */
    node* balance(node*x)
    {
        x->h = reheight(x);          // xの高さを更新する
        if (bfactor(x) > 1)  {       // 左に2つ分偏っている場合、left-left caseおよびleft-right caseが考えられる
            if (bfactor(x->left) < 0) { x->left = leftrot(x->left); }      // left-right caseならば、左回転を行うことで、left-left caseに帰着させる
            return rightrot(x);      // 右回転を行うことでleft-left caseを解消し、高さ平衡を満たす部分木の根を返す
        }
        if (bfactor(x) < -1) {       // 右に2つ分偏っている場合、right-right caseおよびright-left caseが考えられる
            if (bfactor(x->right) > 0) { x->right = rightrot(x->right); }  // right-left caseならば、右回転を行うことで、right-right caseに帰着させる
            return leftrot(x);       // 左回転を行うことでright-right caseを解消し、高さ平衡を満たす部分木の根を返す
        }
        return x;                    // 高さ平衡の場合、xを返す      
    }
    /**
     * @brief  節点xからy(xの右の子)へのリンクを"ピボット"とする左回転で回転の結果、
     *         yが部分木の新しい根、xがyの左の子、yの左の子がxの右の子になる
     * @note   x.right != NILを仮定している.実行時間はΟ(1)
     * @param  node*x 節点x
     * @return 部分木の新しい根
     */
    node* leftrot(node*x)
    {
        node*y   = x->right;     // yをxの右の子とする
        x->right = y->left;      // yの左部分木をxの右部分木にする
        y->left  = x;            // xをyの左の子にする
        x->h     = reheight(x);  // xの高さを更新する
        y->h     = reheight(y);  // yの高さを更新する
        return y;                // 部分木の新しい根yを返す
    }
    /**
     * @brief  節点xからy(xの左の子)へのリンクを"ピボット"とする右回転で回転の結果、
     *         yが部分木の新しい根、xがyの右の子、yの右の子がxの左の子になる
     * @note   x.left != NILを仮定している.実行時間はΟ(1)
     * @param  node*x 節点x
     * @return 部分木の新しい根 
     */
    node* rightrot(node*x)
    {
        node*y   = x->left;      // yをxの左の子とする
        x->left  = y->right;     // yの右部分木をxの左部分木にする
        y->right = x;            // xをyの右の子にする
        x->h     = reheight(x);  // xの高さを更新する
        y->h     = reheight(y);  // yの高さを更新する
        return y;                // 部分木の新しい根yを返す
    }
    /**
     * @brief 節点xを根とする部分木の中から最も左にある子を取得する
     * @param node*x 節点x
     */
    node* leftmost(node*x)
    {
        return x->left ? leftmost(x->left) : x;
    }
    /**
     * @brief 節点xを根とする部分木の中から最も右にある子を取得する
     * @param node*x 節点x
     */
    node* rightmost(node*x)
    {
        return x->right ? rightmost(x->right) : x;
    }
    /**
     * @brief xを根とする部分木の最も左の子を削除する補助関数
     * @param node*x 節点x
     */
    node* _erase(node*x)
    {
        if (x->left == nullptr) { return x->right; }
        x->left = _erase(x->left);
        return balance(x);
    }

private:
    /**< @brief 節点xの高さを取得する  */
    constexpr height_t height(node*x) noexcept
    {
        return x ? x->h : 0;
    }
    /**< @brief 節点xの更新される高さを返す */
    constexpr height_t reheight(node*x) noexcept
    {
        return std::max(height(x->left), height(x->right)) + 1;
    }
    /**< @brief 節点xの左右の子の高さの差(x.left - x.right)を返す */
    constexpr height_t bfactor(node*x) noexcept
    {
        return height(x->left) - height(x->right);
    }
    
private:
    /**< @brief 節点xの記憶領域の確保を行う */
    node* allocnode(const Key& k, const T& v)
    {
        node*x = free;
        free = x->next;
        x = new(x) node(k, v);
        return x;
    }
    /**< @brief 節点xの記憶領域の解放を行う */
    void freenode(node*x)
    {
        // 本来ならば、明示的にデストラクタを呼び出す必要がある
        x->next = free;
        free = x;
    }
    /**< @brief 節点n個分の記憶領域を確保する */
    node* allocnodes(std::size_t n)
    {
        return static_cast<node*>(::operator new(sizeof(node) * n));
    }
    /**< @brief 節点n個分の記憶領域を解放する */
    void freenodes(node*x)
    {
        ::operator delete(x); x = nullptr;
    }
private:
    /**< ＠brief キーlとキーrの非同値判定を行う */
    bool neq(const Key& l, const Key& r)
    {
        return (cmp(l, r) || cmp(r, l));
    }
};



#endif  // end of __AVLTREE_HPP__


/**
 * @brief 2分探索木
 * @note  2分探索木は名前からわかるように2分木の中に組織される
 *        このような木は各節点がオブジェクトである.ポインタに基づくデータ構造によって表現できる
 *        各節点はkeyと付属データ以外に、左の子、右の子、親を指す3つの属性left, right, pを持つ
 *        子や親が欠けているときに対応する属性の値はNILである.親属性がNILである節点は根だけである
 *
 *        キーは2分探索木の中に以下の2分探索条件(binary-search-tree property)を満足するように格納する
 *
 *        xを2分探索木の節点とする.yがxの左部分器の節点ならばy.key <= x.key. 右部分木の節点ならばy.key >= x.keyを満たす
 *
 * @note  n個の節点を持つ完全2分探索木上での最悪実行時間はΘ(lgn)であるが、
 *        枝分かれのない道グラフ上では最悪実行時間がΘ(n)になる
 * @date  作成日     : 2016/01/28
 * @date  最終更新日 : 2016/01/29
 */



//****************************************
// インクルードガード
//****************************************

#ifndef __BSTREE_HPP__
#define __BSTREE_HPP__



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <functional>
#include <utility>



//****************************************
// 構造体の定義
//****************************************

/**
 * @brief 2分探索木
 * 
 * @tparam class Key     キーの型
 * @tparam class T       付属データの型
 * @tparam class Compare 比較用関数オブジェクト(デフォルトでキーの昇順)
 */
template <class Key, class T, class Compare = std::less<Key>>
struct BSTree {
    
    // using pair_t = std::pair<const Key, T>;

    /**< @brief 2分探索木節点構造体 */
    struct Node {
        Node*left;    /**< 左の子     */
        Node*right;   /**< 右の子     */
        Node*parent;  /**< 親         */

        Key key;      /**< キー      */
        T value;      /**< 付属データ */

        Node() noexcept : left(nullptr), right(nullptr), parent(nullptr) { }
    };

    Node*root;     /**< 2分探索木の根 */
    Compare cmp;   /**< 比較器       */

    BSTree() noexcept : root(nullptr) { } 


    /**
     * @brief z.key = v, z.left = NIL, z.right = NILを満たす節点zを入力とし、
     *        zが2分探索木Tの正しい位置に挿入されるようにTとzのいくつかの属性を変更する
     * @note  hを木の高さとするとき、実行時間はΟ(h)
     * @param Node*z 木Tに挿入する節点z
     */
    void insert(Node*z)
    {
        Node*y = nullptr;  // yはxの親を指すトレーラポインタ(trailing pointer)
        Node*x = root;     // xは木の根から開始する

        // 入力アイテムzと置き換えるnullptr(NIL)を探してある単純道に沿って木を下る
        while (x != nullptr) {           // xがnullptr(NIL)になるまで、
            y = x;                       // ポインタxとyは
            if (cmp(z->key, x->key)) {   // z.keyとx.keyの比較結果に従って、
                x = x->left;             // 左の子の方向
            }
            else {                       // あるいは
                x = x->right;            // 右の子の方向に向かって下方に移動する
            }
        }
        // zに置き換えるnullptr(NIL)を発見したときには、探索は変更が必要な節点を
        // 1ステップだけ超えて進んでしまっているのでトレーラポインタが必要になる
        z->parent = y; 
        if (y == nullptr) {              // yがnullptr(NIL)を指す場合、
            root = z;                    // 木Tは空であった
        }
        else if (cmp(z->key, y->key)) {  // z.key < y.keyの場合、
            y->left = z;                 // zをyの左の子にする
        }
        else {                           // z.key > y.keyの場合、
            y->right = z;                // zをyの右の子にする
        }
    }

    /**
     * @brief 節点zを2分探索木Tから削除する
     * @note  hを木の高さとするとき、実行時間はΟ(h)
     * @param Node*z 節点z
     */
    void erase(Node*z)
    {
        if (z->left == nullptr) {         // zが左の子を持たない場合、
            transplant(z, z->right);      // zを右の子rと置き換える.rはnullptr(NIL)かもしれない
        }
        else if (z->right == nullptr) {   // zが左の子lを持つが、右の子を持たない場合、
            transplant(z, z->left);       // zを左の子lと置き換える
        }
        else {                            // zが左右の子l,rをともに持つ場合、
            Node*y = leftmost(z->right);  // zの次節点yを発見する(yは左の子を持たないことに注意)
            
            if (y->parent != z) {         // zの次節点y(!=r)がrを根とする部分木の中にある場合、
                transplant(y, y->right);  // yが置かれていた(yの親の子)の場所にyの右の子を置き、
                y->right = z->right;      // yの右の子がzの右の子rを指すようにする
                y->right->parent = y;     // yをrの親にする
            } 
            transplant(z, y);             // zが置かれていた(zの親の子)の場所にyを置き、
            y->left = z->left;            // yの左の子がzの左の子lを指すように設定する
            y->left->parent = y;          // yをlの親にする
        }
    }
    

    /**
     * @brief  中間順木巡回を行う
     * @note   n個の節点を持つ2分探索木の巡回はΘ(n)時間かかる
     * @tparam class F Node*を引数にとる関数オブジェクトの型
     * @param  F fn    Node*を引数にとる関数オブジェクト
     * @param  Node*x  巡回を行う部分木の根
     */
    template <class F>
    void inorder(Node*x, F fn)
    {
        // xがnullptr(NIL)を指すとき、再帰は底をつく(葉に達していた)
        if (x == nullptr) {
            return;
        }
        inorder(x->left, fn);
        fn(x);
        inorder(x->right, fn);
    }

    /**
     * @brief  2分探索木からある与えられたキーを持つ節点を探索する
     * @note   hを木の高さとするとき、実行時間はΟ(h)
     * @param  Key  k キー
     * @param  Node*x 探索を行う部分木の根
     * @return キーkを持つ節点が存在すれば、その節点を指すポインタを返し、
     *         存在しなければ、nullptr(NIL)を返す
     */
    Node*find(Key k, Node*x)
    {
        while (x != nullptr && k != x->key) {
            if (cmp(k, x->key)) {  // kがx.keyより小さければ、
                x =  x->left;      // 左部分木に対し、探索続行
            }
            else {                 // kがx.keyより大きければ、
                x = x->right;      // 右部分木に対し、探索続行
            }
        }
        return x;
    }
    
    //************************************************************
    // 以下の手続きにおける引数xはnullptr(NIL)ではないと仮定する
    //************************************************************
    
    /**
     * @brief 節点xを根とする部分木の中で最小のキーを持つ節点を指すポインタを返す
     * @note  実行時間はΟ(h)
     */
    Node*leftmost(Node*x)
    {
        // 節点xに左部分木がなければ、xの右部分木に出現する任意のキーはx.key以上だから、
        // xを根とする部分木に出現する最小のキーはx.keyである
        // 節点xが左部分木をもつなら、右部分木に出現するどのキーもx.key以上で、
        // 左部分木に出現するどのキーもx.key以下だから、
        // xを根とする部分木に出現する最小のキーはx.leftを根とする部分木の中に存在する
        while (x->left != nullptr) {
            x = x->left;
        }
        return x;
    }

    /**
     * @brief 節点xを根とする部分木の中で最大のキーを持つ節点を指すポインタを返す
     * @note  実行時間はΟ(h)
     */
    Node*rightmost(Node*x)
    {
        // 節点xに右部分木がなければ、xの左部分木二出現する任意のキーはx.key以下だから、
        // xを根とする部分木に出現する最大のキーはx.keyである
        // 節点xが右部分木をもつなら、左部分木に出現するどのキーもx.key以下で、
        // 右部分木に出現するどのキーもx.key以上だから、
        // xを根とする部分木に出現する最大のキーはx.rightを根とする部分木の中に存在する
        while (x->right != nullptr) {
            x = x->right;
        }
        return x;
    }

    /**
     * @brief 2分探索木の節点xに次節点が存在するときには次節点を返し、
     *        xのキーが木の中で最大ならば、nullptr(NIL)を返す
     * @note  実行時間はΟ(h)
     */
    Node*succ(Node*x)
    {
        if (x->right != nullptr) {      // 節点xが右部分木を持つ場合は、
            return leftmost(x->right);  // xの次節点は右部分木の最左節点である
        }
        // (xが次節点yを持ち)節点xに右部分木がない場合は、
        // yは、xの祖先で、その左の子もxの祖先であるものの中で最もxに近い節点である
        Node*y = x->parent;
        // xから開始し、ある節点の左の子に出会うまで木を根に向かって遡る
        while (y != nullptr && x == y->right) {
            x = y;
            y = y->parent;
        }
        return y;
    }

    /**
     * @brief 2分探索木の節点xに先行節点が存在するときには先行節点を返し、
     *        xのキーが木の中で最小ならば、nullptr(NIL)を返す
     * @note  実行時間はΟ(h)
     */
    Node*pred(Node*x)
    {
        if (x->left != nullptr) {       // 節点xが左部分木を持つ場合は、
            return rightmost(x->left);  // xの先行節点は左部分木の最右節点である
        }
        // (xが先行節点を持ち)節点xに左部分木がない場合は、
        // yは、xの祖先で、その右の子もxの祖先であるものの中で最もxに近い節点である
        Node*y = x->parent;
        // xから開始し、ある節点の右の子に出会うまで木を根に向かって遡る
        while (y != nullptr && x == y->left) {
            x = y;
            y = y->parent;
        }
        return y;
    }

private: 
    /**
     * @brief 節点uを根とする部分木を節点vを根とする部分木を置き換える
     *        uの親がvの親になり、uの親がvを適切な子として持つことになる
     * @note  v.leftとv.rightを更新しないことに注意せよ
     *        これらの更新はtransplantを呼び出す側の責任である
     * @param Node*u 節点u
     * @param Node*v 節点v
     */
    void transplant(Node*u, Node*v)
    {
        if (u->parent == nullptr) {       // uの親がnullptr(NIL)の場合、
            root = v;                     // vは2分探索木Tの根である
        }
        else if (u == u->parent->left) {  // uが左の子の場合、
            u->parent->left = v;          // vがuの親の左の子になる
        }
        else {                            // uが右の子の場合、
            u->parent->right = v;         // vがuの親の右の子になる
        }
        
        if (v != nullptr) {               // vがnullptr(NIL)でない場合、
            v->parent = u->parent;        // uの親をuの親に更新する
        }
    }
};



#endif  // end of __BSTREE_HPP__


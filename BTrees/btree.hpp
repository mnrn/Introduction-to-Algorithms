/**
 * @bfief B木
 * @note  B木(B-tree)Tは以下の条件を満足する根付き木であり、その根をT.rootで示す
 *
 *        1. 各節点xは以下の属性を持つ
 *            a  節点xに現在格納されているキー数x.n
 *            b  格納されているx.n個のキー自身x.key0, x.key1, ..., x.keyx.(n-1)
 *               キーは昇順に格納されており、x.key0<=x.key1<=...<=x.keyx.(n-1)を満たす
 *            c  xが葉であればTRUE, 内部節点であればFALSEを取るブール変数x.leaf
 *
 *        2. 各内部節点xはその子を指すx.n+1個のポインタx.c0, x.c1,...x.cx.nも持つ
 *           葉は子を持たないのでそのci属性は未定義である
 *
 *        3. x.n個のキーx.keyiは各部分木に格納されているキーの存在範囲を分離する.すなわち、
 *           x.ciを根とする部分木に格納されている任意のキーをkiとすると
 *             k0 <= x.key0 <= k1 <= x.key1 <= ... <= x.key(x.(n-1)) <= kx.n
 *           である
 *
 *        4. すべての葉は同じ深さを持ち、その深さは木の高さhである
 *
 *        5. 1つの節点が格納できるキー数に上限と下限が存在する.これらの限界はB木の最小次数(minimum degree)と呼ぶ
 *           ある固定された整数 t >= 2 を用いて表現する
 *             a  根を除くすべての節点は少なくともt-1個のキーを持つ.したがって、根を除くすべての
 *                内部節点は少なくともt個の子を持つ.木が空でなければ根は少なくとも1個のキーを持つ
 *             b  どの節点も最大2t-1個のキーを持つことができる.したがって、内部節点は最大2t個の子を持つことができる
 *                ちょうど2t-1個のキーを持つ内部節点は飽和している(full)という
 *
 * @date  作成日     : 2016/02/04
 * @date  最終更新日 : 2016/02/05
 */



//****************************************
// インクルードガード
//****************************************

#ifndef __BTREE_HPP__
#define __BTREE_HPP__



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <functional>
#include <utility>
#include <iostream>



//****************************************
// 構造体の定義
//****************************************

template <
    class Key,
    class T,
    class Compare = std::less<Key>
>
struct BTree {
    using pair_t  = std::pair<Key, T>;
    using int_t   = std::int32_t;
    
    struct Node {
        int_t n;      /**< 現在格納されているキー数                             */
        pair_t*key;   /**< 格納されているn個のキー自身(ただし、2t-1個だけ確保する)  */
        Node**c;      /**< 子を指すn+1個のポインタ(ただし、2t個だけ確保する)       */
        bool leaf;    /**< 葉であればTRUE, 内部節点であればFALSEを取るブール変数   */
    };
    
    
    Node*root;        /**< B木Tの根     */
    const int_t t;    /**< B木の最小次数 */
    Compare _cmp;     /**< 比較述語     */

    
    explicit BTree(std::int32_t t) : t(t) { create(); }
    

    /**
     * @brief  ある部分木の根xを指すポインタと探索対象であるキーkを入力として取る
     *         kがB木の中にあれば、y.keyi = kである節点yと添字iの順序対(y, i)を返す
     *         kがB木の中になければ値NILを返す
     *
     * @param  Node*x ある部分木の根x
     * @param  const Key& k 探索対象のキーk
     * @return y.keyi = kである節点yと添字iの順序対(y, i)
     *         存在しなければ順序対(nullptr, 0)を返す
     */
    std::pair<Node*, int_t> find(Node*x, const Key& k)
    {
        // 線形探索を用いて、k<=x.key(k>=x.key)を満足する最小のiを見つけるか、
        // (条件を満たすiが存在しないときには)iをx.nに設定する
        int_t i = 0;
        while (i < x->n && cmp(x->key[i], k)) {
            i = i + 1;
        }
        // キーkを発見できたかどうか調べ、
        if (i < x->n && eq(x->key[i], k)) {    // kが発見できたならば、
            return std::make_pair(x, i);       // (x, i)を返す
        }
        // 発見できなかったときは、
        else if (x->leaf) {                     // xが葉なので
            return std::make_pair(nullptr, 0);  // 探索を不成功として終了するか、
        }
        else {
            return find(x->c[i], k);            // これを根とする部分木に再帰して探索を続行する
        }
    }

    /**
     * @brief B木の根から下る1つの道を辿って高さhのB-木Tにあるキーkを挿入する
     * @note  再帰が決して飽和節点に下りてゆかないことを保証するために、手続きinsertはsplit_childを用いる
     * @param const pair_t& k キーk
     */
    void insert(const pair_t k)
    {
        Node*r = root;
        if (r->n == 2 * t - 1) {  // 根rが飽和節点の場合、
            // 根を分割し、(2つの子を持つ)新しい節点sを根とする.根の分割がB木の高さを上げる唯一の手段である
            Node*s  = allocnode();
            root    = s;
            s->leaf = false;
            s->n    = 0;
            s->c[0] = r;
            split_child(s, 0);    // 必要に応じてsplit_childを呼び出し、手続きが再帰する節点が未飽和であることを保証する

        // insertは未飽和な根を持つ根付き木にキーkを挿入する手続きinsert_nonfullを呼び出して、終了する
            insert_nonfull(s, k);
        }
        else {
            insert_nonfull(r, k);
        }
    }

    /**
     * @brief  キーkをxを根とする部分木から削除する
     *
     * @note   eraseが節点x上に再帰するときには、xのキー数が常に最小次数t以上となるように手続きは構成されている
     *         B木の(根以外の)各節点のキー数はt-1以上でなければならないが、この条件はそれよりも1だけ大きいキー数を要求している
     *         したがって、再帰が(この条件を満足しない)子に降りていくときには、事前にその子にキーを移し、この条件を満足するようにする必要がある
     *         この強化された条件のおかげで、(のちに説明する唯一の例外を覗いて)1つの道に沿って"後戻り"することなく木を下ることでキーを削除できる
     *
     * @note   以下に示すB木からの削除手続きの仕様を解釈する際には、つぎの事実を理解しておかなければならない
     *
     *         根xがキーを持たない内部節点になるような事態が生じたときには(この事態は以下の場合2cと場合3bで起こる可能性がある)、
     *         xを削除し、xの唯一の子x.c0を木の新しい根とする.その結果、木の高さが1減少し、(木が空でない限り)
     *         木の根が少なくとも1つのキーを持つという性質が保存される
     *
     * @detail      場合1. キーkが節点xに存在し、xが葉ならば、kをxから削除する
     *
     *
     *              場合2. キーkが節点xに存在し、xが内部節点ならば、以下の処理を行う
     *
     *                a  xにおけるkの直前の子yが少なくともt個のキーを持つならば、yを根とする部分木からkの直前のキーk'を見つける
     *                   再帰的にk'を削除し、xにおいてkをk'に置き換える(k'の発見とその削除は木を下る1つの道に沿って実行できる)
     *
     *                b  対称的に、xにおけるkの直後の子zが少なくともt個のキーを持つならば、zを根とする部分木からkの直後のキーk'を見つける
     *                   再帰的にk'を削除し、xにおいてkをk'に置き換える(k'の発見とその削除は木を下る1つの道に沿って実行できる)
     *
     *                c  それ以外の場合、すなわち、yとzがともにt-1個しかキーを持たない場合には、kとzの全てのキーをyとマージする
     *                   その結果、xはkとzへのポインタをともに失い、yは2t-1個のキーを持つ.zを解放し、再帰的にkをyから削除する
     *
     *
     *              場合3. キーkが内部節点xに存在しないとき、kが木の中に存在するならば必ずkを含む部分木の根x.ciを決定する
     *                     x.ciがt-1個のキーしか持たないならば、再帰する節点が少なくともt個のキーを持つことを保証するために、
     *                     ステップ3aまたは3bを必要に応じて実行する.最後にxを適切な子の上に再帰する
     *
     *                a  x.ciはt-1個のキーしか持たないが、少なくともt個のキーを待つ隣り合う(右あるいは左の)兄弟が存在するならば、
     *                   xのキーをx.ciに移し、この兄弟のキーをxに移し、適切な子ポインタをこの兄弟からx.ciに移すことで、x.ciのキー数を増やす
     *
     *                b  x.ciも左右の隣り合う兄弟もすべてt-1個のキーしか持たないならば、x.ciをどちらかの兄弟とマージする
     *                   この操作の中で、xのキーがマージされた新しい節点に移り、その中央キーになる
     *
     *
     * @note   B木ではほとんどのキーは葉に格納されているので、実際上、ほとんどの削除は葉からだと予想される
     *         このとき、erase手続きは木を後戻りせず、1つの道に沿って動く.しかし、内部節点からキーを削除しようとするときには、
     *         手続きは木を下る道を形成するが、このキーをその直前あるいは直後のキーと置き換えるためにこの節点に戻る可能性がある(場合2aと2b)
     *
     * @param Node*         x 部分木の根x
     * @param const pair_t& k キーk
     */
    void erase(Node*x, const pair_t& k)
    {
        // 線形探索を用いて、k<=x.key(k>=x.key)を満足する最小のiを見つけるか、
        // (条件を満たすiが存在しないときには)iをx.nに設定する
        int_t i = 0;
        while (i < x->n && cmp(x->key[i], k)) {
            i = i + 1;
        }
        if (i < x->n && eq(x->key[i], k)) {  // キーkが節点xに存在し、
            if (x->leaf) {                      // xが葉の場合、
                erase_leaf(x, i);               // 場合1に対する操作を行う
            }
            else {                              // xが内部節点の場合、
                erase_nonleaf(x, i);            // 場合2に対する操作を行う
            }
        }
        else {              // キーkが節点xに存在せず、
            if (x->leaf) {  // xが葉の場合、
                return;     // キーkはB-木Tに存在しない
            }
            // xが内部節点の場合、
       
            bool end = i == x->n;       // iがx.nに設定されているかを保持しておく
            
            if (x->c[i]->n == t - 1) {  // x.ciがt-1個のキーしか持たないならば、
                fill_child(x, i);       // 場合3に対する操作を行う
            }
            // 最後にxを適切な子の上に再帰する
            (end && i > x->n) ? erase(x->c[i - 1], k) : erase(x->c[i], k);
        }
    }

private:

    /**
     * @brief   「未飽和な」内部節点xとx.ciがxの「飽和した」子である添字iを入力とする
     *          手続きはこの子を2つに分割し、xが新たに1個の子を獲得するようにxを変形する
     *
     * @note    飽和した根を分割するには、現在の根を新しくした空の根の子にし、split_childを使う
     *          根の分割の結果、木の高さは1だけ増える.分割が木を成長させる唯一の操作である
     *
     * @detail  xは分割が実行されている節点(分割される節点の親), yはxのi番目の子である
     *          節点yはもともと2t個の子(2t-1個のキー)を持っていたが、操作の結果、t個(t-1個のキー)に減少する
     *          節点zはyの大きいほうからt-1個のキーを受け継ぎ、xの子達の中でyのつぎに位置するxの新しい子になる
     *          yの中央のキーはxのキーになり、yとzを分離する
     *
     * @param   Node* x 「未飽和な」内部節点x
     * @param   int_t i  xの「飽和した」子である添字
     */
    void split_child(Node*x, int_t i)
    {
        Node*z  = allocnode();
        Node*y  = x->c[i];
        z->leaf = y->leaf;
        z->n    = t - 1;
               
        // yの大きいほうからt-1このキーと、
        int_t j = 0;
        for (j = 0; j < t - 1; j++) {
            z->key[j] = y->key[j+t];
        }
        // それらに対応するt個の子を与える
        if (y->leaf == false) {
            for (j = 0; j < t; j++) {
                z->c[j] = y->c[j+t];
            }
        }
        y->n = t - 1;   // yのキー数を修正する
        
        // zをxの子としてxに挿入し、
        for (j = x->n; j >= i + 1; j--) {
            x->c[j+1] = x->c[j];
        }
        x->c[i+1] = z;
        // yとzを分離するためにyからその中央キーをxに移し、xのキー数を修正する
        for (j = x->n - 1; j >= i; j--) {
            x->key[j+1] = x->key[j];
        }
        x->key[i] = y->key[t-1];
        x->n = x->n + 1;
    }

    /**
     * @brief 節点xを根とする部分木にキーkを挿入する
     * @note  手続きが呼び出された時点ではxは未飽和であると仮定する
     *        insert操作とinsert_nonfullの再帰的操作からこの仮定は成立する
     *
     * @param Node*         x 節点x
     * @param const pair_t& k キーk
     */
    void insert_nonfull(Node*x, const pair_t& k)
    {
        int_t i = x->n - 1;
        if (x->leaf) {
            // xにkを挿入することでxが葉の場合を解決する
            while (i >= 0 && cmp(k, x->key[i])) {
                x->key[i+1] = x->key[i];
                i = i - 1;
            }
            x->key[i+1] = k;
            x->n = x->n + 1;
        }
        else {
            // xが葉でなければ、 内部節点xを根とする部分木の適切な葉にkを挿入する必要がある
            // 再帰が降りてゆくxの子を決定する
            while (i >= 0 && cmp(k, x->key[i])) {
                i = i - 1;
            }
            i = i + 1;
            // 再帰が降りてゆく節点が飽和節点か否かを調べ、
            if (x->c[i]->n == 2 * t - 1) {  // 飽和節点ならば、
                split_child(x, i);          // split_childを呼び出してこの節点を2つの未飽和節点に分割し、
                // これらの内のどちらに再帰するのが正しいのか決定する
                if (cmp(x->key[i], k)) { i = i + 1; }
            }
            insert_nonfull(x->c[i], k);  // kを適切な部分木に挿入するために再帰する
        }
    }

private:
    /**
     * @brief 空のB木を生成する
     */
    void create()
    {
        Node*x  = allocnode();
        x->leaf = true;
        x->n    = 0;
        root    = x;
    }

private:  
    decltype(auto) cmp(const pair_t& l, const pair_t& r)
    {
        return _cmp(l.first, r.first);
    }
    decltype(auto) cmp(const pair_t& p, const Key& k)
    {
        return _cmp(p.first, k);
    }
    decltype(auto) cmp(const Key& k, const pair_t& p)
    {
        return _cmp(k, p.first);
    }

    bool eq(const pair_t& l, const pair_t& r)
    {
        return !(cmp(l, r) || cmp(r, l));
    }
    bool eq(const pair_t& p, const Key& k)
    {
        return !(cmp(p, k) || cmp(k, p));
    }
    bool eq(const Key& k, const pair_t& p)
    {
        return eq(p, k);
    }
    
private:
    Node* allocnode()
    {
        Node*x  = new Node();
        x->leaf = false;
        x->n    = 0;
        x->key  = new pair_t[2 * t - 1];
        x->c    = new Node*[2 * t];
        return x;
    }
    
    
    void freenode(Node*x)
    {
        delete[] x->key;
        delete[] x->c;
        delete x;
    }

public:
    /**< @brief B木の最も左に存在するキーを取り出す */
    pair_t leftmost(Node*x)
    {
        if (x->leaf) {                     // xが葉である場合、
            return x->key[0];              // xの一番最初のキーを返す
        }
        else {                             // xが内部節点である場合、
            return leftmost(x->c[0]);      // xの最も左の子に対して再びleftmostを呼ぶ
        }
    }
    /**< @brief B木の最も右に存在するキーを取り出す */
    pair_t rightmost(Node*x)
    {
        if (x->leaf) {                     // xが葉である場合、
            return x->key[x->n - 1];       // xの一番最後のキーを返す
        }
        else {                             // xが内部節点である場合、
            return rightmost(x->c[x->n]);  // xの最も右の子に対して再びrightmostを呼ぶ
        }
    }
    
private:
    /**
     * @brief 手続きeraseの場合1に対する操作を行う
     * @note  場合1. キーkが節点xに存在し、xが葉ならば、kをxから削除する
     * @param Node* x 節点x
     * @param int_t i x.keyi(キーk)の添字i
     */
    void erase_leaf(Node*x, int_t i)
    {
        // 場合1. キーkが節点xに存在し、xが葉ならば、kをxから削除する
        erase_key(x, i);
    }

    /**
     * @brief 手続きeraseの場合2に対する操作を行う
     * @note  場合2. キーkが節点xに存在し、xが内部節点ならば、以下の処理を行う
     *
     *          a  xにおけるkの直前の子yが少なくともt個のキーを持つならば、yを根とする部分木からkの直前のキーk'を見つける
     *             再帰的にk'を削除し、xにおいてkをk'に置き換える(k'の発見とその削除は木を下る1つの道に沿って実行できる)
     *
     *          b  対称的に、xにおけるkの直後の子zが少なくともt個のキーを持つならば、zを根とする部分木からkの直後のキーk'を見つける
     *             再帰的にk'を削除し、xにおいてkをk'に置き換える(k'の発見とその削除は木を下る1つの道に沿って実行できる)
     *
     *          c  それ以外の場合、すなわち、yとzがともにt-1個しかキーを持たない場合には、kとzの全てのキーをyとマージする
     *             その結果、xはkとzへのポインタをともに失い、yは2t-1個のキーを持つ.zを解放し、再帰的にkをyから削除する
     *
     * @param Node* x 内部節点x
     * @param int_t i x.keyi(キーk)の添字i
     */
    void erase_nonleaf(Node*x, int_t i)
    {
        Node*y = x->c[i];
        Node*z = x->c[i+1];

        // 場合2a  xにおけるkの直前の子yが少なくともt個のキーを持つならば、yを根とする部分木からkの直前のキーk'を見つける
        //         再帰的にk'を削除し、xにおいてkをk'に置き換える(k'の発見とその削除は木を下る1つの道に沿って実行できる)
        if (y->n >= t) {
           
            pair_t _k = pred(x, i);  // yを根とする部分木からkの直前のキーk'を見つける
            erase(y, _k);            // 再帰的にk'を削除し、
            x->key[i] = _k;          // xにおいてkをk'に置き換える
            
        }
        // 場合2b  対称的に、xにおけるkの直後の子zが少なくともt個のキーを持つならば、zを根とする部分木からkの直後のキーk'を見つける
        //         再帰的にk'を削除し、xにおいてkをk'に置き換える(k'の発見とその削除は木を下る1つの道に沿って実行できる)
        else if (z->n >= t) {
            pair_t _k = succ(x, i);  // zを根とする部分木からkの直後のキーk'を見つける
            erase(z, _k);            // 再帰的にk'を削除し
            x->key[i] = _k;          // xにおいてkをk'に置き換える
        }
        // 場合2c  それ以外の場合、すなわち、yとzがともにt-1個しかキーを持たない場合には、kとzの全てのキーをyとマージする
        //         その結果、xはkとzへのポインタをともに失い、yは2t-1個のキーを持つ.zを解放し、再帰的にkをyから削除する
        else {
            pair_t k = x->key[i];
            merge(x, i);             // kとzの全てのキーをyとマージする
            erase(y, k);             // 再帰的にkをyから削除する
        }
    }

    /**
     * @brief  節点xにおけるキーkの直前の子yとその兄弟であるkの直後の子zに対して、kとzのすべてのキーをyとマージする
     *         その結果、xはkとzへのポインタをともに失い、yは2t-1個のキーを持つ.最後にzが解放される
     * @param  Node* x キーkを持つ節点x 
     * @param  int_t i x.keyi(キーk)の添字
     */
    void merge(Node*x, int_t i)
    {
        Node*y = x->c[i];    // xにおけるkの直前の子y
        Node*z = x->c[i+1];  // yの兄弟であり、xにおけるkに直後の子z

        // キーkをyのt番目のキーとして挿入する(zero-basedなら添字はt-1)
        y->key[t-1] = x->key[i];        // kをyとマージする
        for (int_t j = 0; j < z->n; j++) {
            y->key[j + t] = z->key[j];  // zのすべてのキーをyとマージする
        }

        if (y->leaf == false) {         // yが葉ではない場合、(このとき、yの兄弟zも葉ではない)
            for (int_t j = 0; j <= z->n; j++) {
                y->c[j + t] = z->c[j];  // zのすべての子をyとマージする
            }
        }
        // xはkとzへのポインタを失い、
        erase_key(x, i);
        erase_child(x, i+1);
        
        y->n = y->n + z->n + 1;  // yは2t-1個のキーを持つ
        freenode(z);             // zを解放
    }

public:
    /**
     * @brief  xにおけるk(x.keyi)の直前の子yを根とする部分木から直前のキーk'を見つける
     * @param  Node* x キーkを持つ節点x
     * @param  int_t i x.keyi(キーk)の添字i
     * @return 直前のキーk'
     */
    pair_t pred(Node*x, int_t i)
    {
        Node*y = x->c[i];     //  xにおけるkの直前の子yを取得する
        return rightmost(y);  //  yを根とする部分木から直前のキーk'を見つける
    }

    /**
     * @brief  xにおけるk(x.keyi)の直後の子zを根とする部分木から直前のキーk'を見つける
     * @param  Node* x キーkを持つ節点x 
     * @param  int_t i x.keyi(キーk)の添字
     * @return 直後のキーk'
     */
    pair_t succ(Node*x, int_t i)
    {
        Node*z = x->c[i+1];   // xにおけるkの直後の子zを取得する
        return leftmost(z);   // zを根とする部分木から直前のキーk'を見つける
    }

private:
    /**
     * @brief 節点xからk.keyiを削除する
     * @note  左詰めです
     * @param Node* x 節点x
     * @param int_t i x.keyの添字
     */
    void erase_key(Node*x, int_t i)
    {
        for (int_t j = i; j < x->n - 1; j++) {
            x->key[j] = x->key[j + 1];
        }
        x->n = x->n - 1;
    }
    /**
     * @brief 節点xからx.ciを削除する
     * @note  手続きerase_childが呼ばれる直前に手続きerase_keyが呼ばれていることを仮定します
     * @note  左詰めです
     * @param Node* x 節点x
     * @param int_t i x.cの添字
     */
    void erase_child(Node*x, int_t i)
    {
        for (int_t j = i; j <= x->n; j++) {
            x->c[j] = x->c[j + 1];
        }
    }


private:
    /**
     * @brief 手続きeraseの場合3に対する操作を行う
     * @note  場合3. キーkが内部節点xに存在しないとき、kが木の中に存在するならば必ずkを含む部分木の根x.ciを決定する
     *               x.ciがt-1個のキーしか持たないならば、再帰する節点が少なくともt個のキーを持つことを保証するために、
     *              ステップ3aまたは3bを必要に応じて実行する.最後にxを適切な子の上に再帰する
     *
     *          a  x.ciはt-1個のキーしか持たないが、少なくともt個のキーを待つ隣り合う(右あるいは左の)兄弟が存在するならば、
     *             xのキーをx.ciに移し、この兄弟のキーをxに移し、適切な子ポインタをこの兄弟からx.ciに移すことで、x.ciのキー数を増やす
     *
     *          b  x.ciも左右の隣り合う兄弟もすべてt-1個のキーしか持たないならば、x.ciをどちらかの兄弟とマージする
     *             この操作の中で、xのキーがマージされた新しい節点に移り、その中央キーになる
     *
     * @param  Node* x 節点x 
     * @param  int_t i 添字
     */
    void fill_child(Node*x, int_t i)
    {
        if (i > 0 && x->c[i - 1]->n >= t) {          // x.iの左の兄弟が少なくともt個のキーを持つ場合は、
            borrow_prev(x, i);                       // 場合3aに対する操作を行う
        }
        else if (i < x->n && x->c[i + 1]->n >= t) {  // x.iの右の兄弟が少なくともt個のキーを持つ場合も、
            borrow_next(x, i);                       // 場合3aに対する操作を行う
        }
        else {                                       // x.ciも左右の隣り合う兄弟もすべてt-1個のキーしか持たない場合、
            //x.ciをどちらかの兄弟とマージする.この操作の中で、xのキーがマージされた新しい節点に移り、その中央キーになる
            i == x->n ? merge(x, i - 1) : merge(x, i);  
        }
    }

    /**
     * @brief 手続きeraseの場合3aに対する操作を行う
     * @note  場合3a  x.ciはt-1個のキーしか持たないが、少なくともt個のキーを待つ隣り合う(右あるいは左の)兄弟が存在するならば、
     *               xのキーをx.ciに移し、この兄弟のキーをxに移し、適切な子ポインタをこの兄弟からx.ciに移すことで、x.ciのキー数を増やす
     * @note Node* x 節点x
     * @note int_t i 添字
     */
    void borrow_prev(Node*x, int_t i)
    {
        Node*c = x->c[i];                  // xのi+1番目の子child(zero-based)
        Node*s = x->c[i-1];                // cの兄弟sibling
        
        unshift_key(c, x->key[i - 1]);     // xのキーをcのキーリストの先頭に移す
        if (c->leaf == false) {            // このとき、cが内部節点ならば、
            unshift_child(c, s->c[s->n]);  // cの子リストの先頭にsの子リストの末尾の子を挿入する
        }
        x->key[i-1] = s->key[s->n - 1];    // cの兄弟sの末尾のキーをxに移す
        s->n = s->n - 1;
    }

    /**
     * @brief 手続きeraseの場合3aに対する操作を行う
     * @note  場合3a  x.ciはt-1個のキーしか持たないが、少なくともt個のキーを待つ隣り合う(右あるいは左の)兄弟が存在するならば、
     *               xのキーをx.ciに移し、この兄弟のキーをxに移し、適切な子ポインタをこの兄弟からx.ciに移すことで、x.ciのキー数を増やす
     * @note Node* x 節点x
     * @note int_t i 添字
     */
    void borrow_next(Node*x, int_t i)
    {
        Node*c = x->c[i];                  // xのi+1番目の子child(zero-based)
        Node*s = x->c[i+1];                // cの兄弟sibling

        push_key(c, x->key[i]);            // xのキーをcのキーリストの末尾に移す
        if (c->leaf == false) {            // このとき、cが内部節点ならば、
            push_child(c, s->c[0]);        // cの子リストの末尾にsの子リストの先頭の子を挿入する...(1)
        }
        x->key[i] = s->key[0];             // cの兄弟sの先頭のキーをxに移す
        shift_key(s);                      // sの先頭のキーがxに移ったので左詰めを行う
        if (s->leaf == false) {            // sが内部節点ならば、(1)のとき、
            shift_child(s);                // sの子リストの先頭がcの子リストに移ったので左詰めを行う
        }
    }

private:
    /**
     * @brief 節点xからキーリストの先頭を削除して取り出す
     */
    void shift_key(Node*x)
    {
        for (int_t i = 0; i < x->n - 1; i++) {
            x->key[i] = x->key[i + 1];
        }
        x->n = x->n - 1;
    }
    /**
     * @brief 節点xから子リストの先頭を削除して取り出す
     * @note  手続きshift_childは呼び出される直前に手続きshift_keyが呼ばれていると仮定する
     */
   void shift_child(Node*x)
    {
        for (int_t i = 0; i < x->n - 1; i++) {
            x->c[i] = x->c[i + 1];
        }
    }
    /**
     * @brief 節点xのキーリストの先頭にキーを挿入する
     */
    void unshift_key(Node*x, const pair_t& k)
    {
        for (int_t i = x->n; i > 0; i--) {
            x->key[i] = x->key[i - 1];
        }
        x->key[0] = k;
        x->n = x->n + 1;
    }
    /**
     * @brief 節点uに子リストの先頭に節点vを挿入する
     * @note  手続きunshift_childが呼び出される直前には手続きunshift_keyが呼ばれていると仮定する
     */
    void unshift_child(Node*u, Node*v)
    {
        for (int_t i = u->n; i > 0; i--) {
            u->c[i] = u->c[i - 1];
        }
        u->c[0] = v;
    }
    /**
     * @brief 節点xのキーリストの末尾にキーを挿入する
     */
    void push_key(Node*x, const pair_t& k)
    {
        x->key[x->n] = k;
        x->n = x->n + 1;
    }
    /**
     * @brief 節点uの子リストの末尾に節点vを挿入する
     * @note  手続きpush_childが呼び出される直前には手続きpush_keyが呼ばれていると仮定する
     */
    void push_child(Node*u, Node*v)
    {
         u->c[u->n] = v;
    }

public:
    void dbg_traverse(Node*x)
    {
        int_t i = 0;
        for(; i < x->n; i++) {
            if (x->leaf == false) { dbg_traverse(x->c[i]); }
            std::cout << " " << x->key[i].first;
        }
        if (x->leaf == false) { dbg_traverse(x->c[i]); }
    }

    void insert(const Key& k, T x)
    {
        insert(std::make_pair(k, x));
    }
    void erase(const Key& k)
    {
        auto p = find(root, k);
        erase(root, p.first->key[p.second]);
    }

private:
};



#endif  // end of __BTREE_HPP__


/**
 * @brief 赤黒木(2色木)
 * @note  次の5つの2色条件(red-black properties)をすべて満たすような
 *        2分探索木を2色木(red-black tree)と呼ぶ
 * 
 *        1. 各節点は赤または黒のどちらかである.
 *        2. 根は黒である.
 *        3. すべての葉(NIL)は黒である.
 *        4. ある節点が赤ならばその子は黒である.
 *        5. 各節点について、その節点とその子孫の任意の葉を結ぶ単純道は同数の黒節点を含む.
 *
 * @note  n個の内部節点を持つ2色木の高さは高々2lg(n+1)である.        
 * @date  作成日     : 2016/01/29
 * @date  最終更新日 : 2016/02/06
 */



//****************************************
// インクルードガード
//****************************************

#ifndef __REDBLACKTREE_HPP__
#define __REDBLACKTREE_HPP__



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <functional>
#include <cstdint>
#include <utility>
#include <iostream>



//****************************************
// 構造体の定義
//****************************************

/**
 * @brief 赤黒木
 *
 * @tparam class Key     キーの型
 * @tparam class T       付属データの型
 * @tparam class Compare 比較述語(デフォルトでキーの昇順)
 */
template <class Key, class T, class Compare = std::less<Key>>
struct redblacktree {

    using pair_t = std::pair<const Key, T>;
    
    /**< @brief 色列挙型構造体(scoped enum) */
    enum struct color : std::int32_t {
        red,    /**< 赤 */
        black,  /**< 黒 */
    };

    /**< @brief 赤黒木節点構造体 */
    struct node {
        node*left;          /**< 左の子 */
        node*right;         /**< 右の子 */
        node*p;             /**< 親     */

        color color;        /**< 節点の色   */
        union {
            const Key key;  /**< キー       */
            char d[sizeof(Key)];
        };
        T value;            /**< 付属データ */

        constexpr node() noexcept : left(nullptr), right(nullptr), p(nullptr) { }
        //constexpr explicit Node(const Key& k) : left(nullptr), right(nullptr), p(nullptr), key(k) { }
        node(const Key& k, const T& v) : left(nullptr), right(nullptr), p(nullptr), key(k), value(v) { }
        //constexpr explicit Node(Color color) noexcept : left(nullptr), right(nullptr), p(nullptr), color(color) { }
    };

    node*nil;     /**< NILを表現する番兵(sentinel) */
    node*root;    /**< 赤黒木の根                  */
    Compare cmp;  /**< 比較述語                    */
    node _nil;    /**< sentinel本体               */

    constexpr redblacktree() noexcept { _nil.color = color::black;  root = nil = &_nil; }


    
    /**
     * @brief キーkとその付属データvを持つ節点zが2色木Tに挿入される
     * @note  実行時間はΟ(lgn)
     * @param const Key& k キーk
     * @param const T& v   付属データv
     */
    void insert(const Key& k, const T& v)
    {
        node*z = allocnode(k, v);
        insert(z);
    }

    /**
     * @brief キーkを持つ節点zを2色木Tから削除する
     * @note  実行時間はΟ(lgn)
     * @param const Key& k キーk
     */
    void erase(const Key& k)
    {
        node*z = find(k, root);
        erase(z);
        freenode(z);
    }

    /**
     * @brief  中間順木巡回を行う
     * @note   n個の節点を持つ2分探索木の巡回はΘ(n)時間かかる
     * @tparam class F const Key&を引数にとる関数の型
     * @param  F fn    const Key&を引数にとる関数
     */
    template <class F>
    void inorder(F fn)
    {
        inorder(root, fn);
    }
    

private:
    /**
     * @brief 節点zが2色木Tに挿入される
     * @note  2色木Tを通常の2分探索木と見なして節点zを木Tに挿入し、zを赤に彩色する
     *        次に、2色条件を保存するために補助手続きinsert_fixupを呼び出して
     *        節点の再彩色と回転を行う
     * @note  zのkeyはすでに格納されていると仮定する
     * @note  実行時間はΟ(lgn)
     * @param node*z 節点z
     */
    void insert(node*z)
    {
        node*y = nil;   // yはxの親を指すトレーラポインタ(trailing pointer)
        node*x = root;  // xは木の根から開始する

        // 入力アイテムzと置き換えるT.nilを探してある単純道に沿って木を下る
        while (x != nil) {               // xがT.nilになるまで、
            y = x;                       // ポインタxとyは
            if (cmp(z->key, x->key)) {   // z.keyとx.keyの比較結果に従って、
                x = x->left;             // 左の子の方向
            }
            else {                       // あるいは
                x = x->right;            // 右の子の方向に向かって下方に移動する
            }
        }
        // zに置き換えるT.nilを発見したときには、探索は変更が必要な節点を
        // 1ステップだけ超えて進んでしまっているのでトレーラポインタが必要になる
        z->p = y;
        if (y == nil) {                  // yがT.nilを指す場合、
            root = z;                    // 2色木Tは空であった
        }
        else if (cmp(z->key, y->key)) {  // z.key < y.keyの場合、
            y->left = z;                 // zをyの左の子にする
        }
        else {                           // z.key > y.keyの場合、
            y->right = z;                // zをyの右の子にする
        }
        z->left  = nil;                  // z.leftにT.nilを代入
        z->right = nil;                  // z.rightにT.nilを代入
        z->color = color::red;           // zを赤に彩色
        insert_fixup(z);                 // 2色条件を回復する
    }

    /**
     * @brief 節点zを2色木Tから削除する 
     * @note  実行時間はΟ(lgn)
     * @param node*z 節点z
     */
    void erase(node*z)
    {
        node*y = z;    // 節点yは木からの削除あるいは木の中の移動が想定される節点である
        color y_original_color = y->color;  // 節点yを再彩色する可能性があるので、yの色を記憶しておく
        node*x = nil;  // 節点yが元々置かれていた場所に移動する節点x
        
        if (z->left == nil) {             // zが左の子を持たない場合、
            x = z->right;                 // xがzの右の子rを指すようにする
            transplant(z, z->right);      // zを右の子rと置き換える.rはT.nilかもしれない
        }
        else if (z->right == nil) {       // zが左の子lを持つが、右の子を持たない場合、
            x = z->left;                  // xがzの左の子lを指すようにする
            transplant(z, z->left);       // zを左の子lと置き換える
        }
        else {                            // zが左右の子l,rをともに持つ場合、
            y = leftmost(z->right);       // zの次節点を指すようにyを設定する(yは左の子を持たないことに注意)
            y_original_color = y->color;  // yの色を記憶する
            x = y->right;                 // xがzの次節点yの右の子を指すようにする
            
            if (y->p == z) {              // zの次節点yがzの右の子rである場合、
                // 節点yは木を登ってzの場所を占めることになるので、x.pをyに設定すると、
                x->p = y;                 // x=T.nilであってもx.pはyの親が元々置かれていた場所を指す
            }
            else {                        // zの次節点y(!=r)がrを根とする部分木の中にある場合、
                transplant(y, y->right);  // yが置かれていた(yの親の子)の場所にyの右の子を置き、
                y->right = z->right;      // yの右の子がzの右の子rを指すようにする
                y->right->p = y;          // yをrの親にする
            }
            transplant(z, y);             // zが置かれていた(zの親の子)の場所にyを置き、
            y->left = z->left;            // yの左の子がzの左の子lを指すように設定する
            y->left->p = y;               // yをlの親にする
            y->color = z->color;          // yはzの色を継承する
        }
        if (y_original_color == color::black) {  // 節点yが黒ならば、
            // これらの操作が1つあるいは複数の2色条件に対する違反を導く可能性がある
            erase_fixup(x);               // そこで、2色条件を回復する
        }
        // yが赤ならば、以下の3つの理由からyの削除あるいは移動は2色条件を保存する
        // 1. 木の黒高さは変化しない
        // 2. 2つの赤節点が連続することはない.yがzの色を継承した上でzの場所に移されるので、
        //    yの木の新しい場所で2つの赤節点が連続することはない.加えて、yがzの右の子でなかったならば、
        //    yの元の右の子xが木の中でyと置き換わる.yが赤ならば、xは黒であり、
        //    yをxで置き換えても2つの赤節点が連続することはない
        // 3. yが赤ならば、yが根であるはずはなく、根は依然として黒である.
    }

private:
    /**
     * @brief 節点xからy(xの右の子)へのリンクを"ピボット"とする左回転で、回転の結果、
     *        yが部分木の新しい根、xがyの左の子、yの左の子がxの右の子になる
     * @note  x.right != T.nilであり、根の親がT.nilであることを仮定する
     *        実行時間はΟ(1)で、回転はポインタだけを変更し、他の属性を変更しない
     * @param node*x 節点x
     */
    void leftrot(node*x)
    {
        node*y = x->right;           // yをxの右の子とする
        x->right = y->left;          // yの左部分木をxの右部分木にする
        
        if (y->left != nil) {        // yの左の子がT.nilでないならば、
            y->left->p = x;          // yの左の子の親をxにする 
        }
        y->p = x->p;                 // yの親をxの親にする

        if (x->p == nil) {           // xが根であった場合、
            root = y;                // yが木Tの新しい根となる
        }
        else if (x == x->p->left) {  // xがxの親の左の子であった場合、
            x->p->left = y;          // xの親の左の子をyにする
        }
        else {                       // xがxの親の右の子であった場合、
            x->p->right = y;         // xの親の右の子をyにする
        }
        y->left = x;                 // xをyの左の子にする
        x->p = y;                    // xの親をyにする　
    }

    /**
     * @brief 節点xからy(xの左の子)へのリンクを"ピボット"とする右回転で、回転の結果、
     *        yが部分木の新しい根、xがyの右の子、yの右の子がxの左の子になる
     * @note  x.left != T.nilであり、根の親がT.nilであることを仮定する
     *        実行時間はΟ(1)で、回転はポインタだけを変更し、他の属性を変更しない
     * @param node*x 節点x
     */
    void rightrot(node*x)
    {
        node*y = x->left;            // yをxの左の子とする
        x->left = y->right;          // yの右部分木をxの左部分木にする

        if (y->right != nil) {       // yの右の子がT.nilでないならば、
            y->right->p = x;         // yの右の子の親をxにする
        }
        y->p = x->p;                 // yの親をxの親にする

        if (x->p == nil) {           // xが根であった場合、
            root = y;                // yが木Tの新しい根となる
        }
        else if (x == x->p->left) {  // xがxの親の左の子であった場合、
            x->p->left = y;          // xの親の左の子をyにする
        }
        else {                       // xがxの親の右の子であった場合、
            x->p->right = y;         // xの親の右の子をyにする
        }
        y->right = x;                // xをyの右の子にする
        x->p = y;                    // xの親をyにする
    }

    /**
     * @brief 手続きinsertの補助関数であり、2色条件を回復する
     *
     * @note  insert_fixupが呼び出されたとき、2色条件の中で成立しない可能性がある性質は、
     *        根は黒であることを要請する性質2と、赤節点の子は黒であることを要請する性質4である
     *        これらはzが赤に彩色されたことによる
     *        zが根ならば性質2が成立せず、zの親が赤ならば性質4が成立しない
     *
     * @note  性質1は明らかに成立し、新しく挿入された左右の子はともに番兵nil[T]だから、
     *        性質3も成立する.zの挿入によって(黒の)番兵が赤節点zに置き換わったが、zは番兵を
     *        左右の子として持つから、与えられた節点から葉のすべての単純道は同数の黒節点を持つという
     *        性質5もまた成立する
     * @note  実行時間はΟ(lgn)
     * @param node*z 節点z
     */
    void insert_fixup(node*z)
    {
        // while文は、ループの各繰り返しの直前で、以下に示す3つの部分命題から構成される不変式を維持する
        // a. 節点zは赤である
        // b. z.pが根ならば、z.pは黒である
        // c. 木が2色条件に違反しているならば、性質2か性質4の(両方ではなく)どちらか一方の性質に違反している
        //    性質2に違反しているならば、それはzが赤の根だからである
        //    性質4に違反しているならば、それはzとz.pがともに赤だからである
        while (z->p->color == color::red) {
            // 部分命題(b)から、ループを繰り返すならば、z.pは赤であり、z.pは根ではなくz.p.pが存在する
            if (z->p == z->p->p->left) {                    // zの親z.pがzの祖父z.p.pの左の子である場合、
                node*y = z->p->p->right;                    // yをzの叔父z.p.p.rightを指すように定め、
                if (y->color == color::red) {               // その色を判断する.yが赤ならば、場合1を実行する
                    // 場合1: zの叔父yは赤である(z.pとyがともに赤の場合)
                    // z.p.pが黒であるから、(∵ zとz.p以外の場所では性質4が成立する)
                    // z.pとyの両方を黒に再彩色でき、zとz.pがともに赤である違反を解消できる
                    z->p->color    = color::black;
                    y->color       = color::black;
                    z->p->p->color = color::red;            // z.p.pを赤に再彩色すると性質5が成立する
                    z = z->p->p;                            // z.p.pをzとする.ポインタzは木を2レベル登る
                }
                else {                                      // yが黒ならば、場合2と場合3に制御を移す
                    // 場合2や場合3では節点zは根ではないから性質2に対する違反は存在しない
                    if (z == z->p->right) {
                        // 場合2: zの叔父yは黒であり、zは右の子である
                        // 場合2は場合3にすぐに合流するので、これらは互いに排他的ではない
                        // 直ちに左回転を行って、場合3に帰着する(zはその親の左の子となる)
                        z = z->p;                           // zを1レベル上げ、
                        leftrot(z);                         // zを1レベル下げている
                    }
                    // 場合3: zの叔父yは黒であり、zは左の子である
                    // いくつかの節点を再彩色し、右回転を実行するが、これらの操作は性質5を保存する
                    // 連続する2個の赤節点が存在しないので性質4が成立し、while文が停止する
                    z->p->color = color::black;             // z.pを黒に再彩色するのでwhile文の本体が再び繰り返されることはない
                    z->p->p->color = color::red;            // 赤に再彩色される唯一の節点は
                    rightrot(z->p->p);                      // 回転によって黒節点の子になるから性質2に対する違反を発生しない
                }
            }
            else {                                          // zの親z.pが祖父z.p.pの右の子である場合、
                node*y = z->p->p->left;                     // yをzの叔父z.p.p.leftを指すように定め、
                if (y->color == color::red) {               // その色を判断する.yが赤ならば、場合4を実行する
                    // 場合4: zの叔父yは赤である(z.pとyがともに赤の場合)
                    // z.p.pが黒であるから、(∵ zとz.p以外の場所では性質4が成立する)
                    // z.pとyの両方を黒に再彩色でき、zとz.pがともに赤である違反を解消できる
                    z->p->color = color::black;
                    y->color         = color::black;
                    z->p->p->color = color::red;            // z.p.pを赤に再彩色すると性質5が成立する
                    z = z->p->p;                            // z.p.pをzとする.ポインタzは木を2レベル登る
                }
                else {                                      // yが黒ならば、場合5と場合6に制御を移す
                    // 場合5や場合6では節点zは根ではないから性質2に対する違反は存在しない
                    if (z == z->p->left) {
                        // 場合5: zの叔父yは黒であり、zは左の子である
                        // 場合5は場合6にすぐに合流するので、これらは互いに排他的ではない
                        // 直ちに右回転を行って、場合6に帰着する(zはその親の右の子となる)
                        z = z->p;                           // zを1レベル上げ、
                        rightrot(z);                        // zを1レベル下げている
                    }
                    // 場合6: zの叔父yは黒であり、zは右の子である
                    // いくつかの節点を再彩色し、左回転を実行するが、これらの操作は性質5を保存する
                    // 連続する2個の赤節点が存在しないので性質4が成立し、while文が停止する
                    z->p->color = color::black;             // z.pを黒に再彩色するのでwhile文の本体が再び繰り返されることはない
                    z->p->p->color = color::red;            // 赤に再彩色される唯一の節点は
                    leftrot(z->p->p);                       // 回転によって黒節点の子になるから性質2に対する違反を発生しない
                }
            }
        }
        // ループが停止するのはz.pが黒のときである(zが根ならば、z.pは番兵T.nilだから黒である)
        // したがって、ループ停止時に性質4に対する違反はない.ループ不変式から、
        // 成立しない可能性があるのは性質2だけである.
        root->color = color::black;  // 性質2を回復することですべての2色条件を満たす
    }

    /**
     * @brief 節点uを根とする部分木を節点vを根とする部分木を置き換える
     *        uの親がvの親になり、uの親がvを適切な子として持つことになる
     * @note  v.leftとv.rightを更新しないことに注意せよ
     *        これらの更新はtransplantを呼び出す側の責任である
     * @param node*u 節点u
     * @param node*v 節点v
     */
    void transplant(node*u, node*v)
    {
        if (u->p == nil) {                // uの親がT.nilの場合、
            root = v;                     // vは2色木Tの根である
        }
        else if (u == u->p->left) {       // uが左の子の場合、
            u->p->left = v;               // vがuの親の左の子になる
        }
        else {                            // uが右の子の場合、
            u->p->right = v;              // vがuの親の右の子になる
        }
        // vが番兵を指しているときでもv.pへの代入は可能である
        v->p = u->p;                      // vの親をuの親に(無条件で)更新する
    }

    /**
     * @brief 手続きeraseの補助関数であり、2色条件を回復する
     * 
     * @note  節点yが黒ならば、3つの問題が発生する可能性がある
     *        第1に、元々はyが根であってyの赤の子が新しい根になったとき、性質2に違反する
     *        第2に、xとx.pが共に赤のとき、性質4に違反する
     *        第3に、木の中をyが移動すると、元々yが含んでいた単純道の黒節点が1減少する可能性がある
     *        したがって、yの任意の祖先が性質5に違反する
     *
     * @note  性質5に対する違反を、元々yが置かれていた場所を占めている節点xが"特(別な)黒"を持っていると考えることで解消できる
     *        すなわち、xを含む単純道の黒節点を数えるときには1を足すと決めるならば、この解釈の下で性質5は成立する
     *        黒節点yを削除あるいは移動するときには、その黒を節点xの上に"プッシュ"する.この解釈の問題点は、
     *        節点xが黒でも赤でもなくなって性質1に違反することである.そこで、その代わりに節点xは「黒黒」か「赤黒」のどちらかであり、
     *        xを含む単純道上の黒節点数を数えるときにはそれぞれ2あるいは1だけ寄与すると考えることにする
     *        ただし、xのcolor属性は依然としてRED(xが赤黒のとき)か、BLACK(xが黒黒のとき)のどちらかである
     *        言い換えると、節点の特黒をxが指す節点だけに反映させ、color属性には反映させない
     *
     * @param node*x 節点x
     */
    void erase_fixup(node*x)
    {
        // while文の目的は、以下の3つの条件のいづれかを成立するまで木の中の特黒を持ち上げることである
        // 1. xが赤黒節点を指す.この場合には手続きの最後でxを(普通の)黒に彩色する
        // 2. xが根を指す.この場合には特黒を単に"取り除く"
        // 3. 適切な回転と再彩色を行ってループを停止する
        while (x != root && x->color == color::black) {
            // このwhile文中では、xは常に根ではない黒黒節点を指している
            if (x == x->p->left) {             // xがその親x.pの左の子である場合、
                node*w = x->p->right;          // xの兄弟を指すポインタwを管理する
                // 節点xは黒黒だから、wがT.nilならば、x.pから(黒である)葉wまでの単純道上の節点数は
                // x.pからxまでの単純道上の黒節点よりも小さくなってしまう.したがってwはT.nilでなはい
                if (w->color == color::red) {
                    // 場合1: xの兄弟wが赤の場合
                    // このとき、wの子は黒だから、wとx.pの色を交換し、x.p上で左回転を行っても2色条件に対する違反を生まない
                    w->color = color::black;
                    x->p->color = color::red;
                    leftrot(x->p);
                    w = x->p->right;           // xの新しい兄弟は回転前はwの子だったから黒であり、場合1が場合2、3または4に変換された
                }
                // 場合2、3、4は節点wが黒のときに起こる.これらの場合はwの子の色によって区別する
                if (   w->left->color  == color::black
                    && w->right->color == color::black   ) {
                    // 場合2: xの兄弟wもwの両方の子もすべて黒の場合
                    // xとwの両方から黒を1つ取り除き、xを黒、wを赤にする
                    // xとwから黒を1つ取り除く代わりに、元々赤か黒だったx.pに特黒を付加する
                    w->color = color::red;
                    x = x->p;                  // x.pを新たな節点xとしてループを繰り返す
                    // 場合1を経由して場合2が実行された場合、元々x.pは赤だったから新しい節点xは赤黒である
                    // したがって、新しい節点xのcolor属性の値cはREDだから、while文は繰り返し判定を行って停止する
                }
                else {
                    if(w->right->color == color::black)  {
                        // 場合3: xの兄弟wは黒、wの左の子は赤、wの右の子は黒の場合
                        // このとき、wとその左の子w.leftの色を交換し、w上で右回転を行っても2色条件に対する違反を発生しない
                        w->left->color = color::black;
                        w->color       = color::red;
                        rightrot(w);
                        w = x->p->right;       // xの新しい兄弟wは黒、その右の子が赤だから、場合3が場合4に変換された
                    }
                    // 場合4: xの兄弟wが黒でwの右の子が赤の場合
                    // 数回の再彩色とx.p上の左回転を行うことで、2色条件に対する違反を生み出すことなく、
                    // xの特黒を取り除いて(普通の)黒にできる
                    w->color         = x->p->color;
                    x->p->color      = color::black;
                    w->right->color  = color::black;
                    leftrot(x->p);
                    x = root;                  // xを根に設定すると、while文は繰り返し判定を行って停止する
                }
            }
            else {                             // xがその親x.pの右の子である場合、
                node*w = x->p->left;           // xの兄弟を指すポインタwを管理する
                // 節点xは黒黒だから、wがT.nilならば、x.pから(黒である)葉wまでの単純道上の節点数は
                // x.pからxまでの単純道上の黒節点よりも小さくなってしまう.したがってwはT.nilでなはい
                if (w->color == color::red) {
                    // 場合5: xの兄弟wが赤の場合
                    // このとき、wの子は黒だからwとx.pの色を交換し、x.p上で右回転を行っても2色条件に対する違反を生まない
                    w->color = color::black;
                    x->p->color = color::red;
                    rightrot(x->p);
                    w = x->p->left;            // xの新しい兄弟は回転前はwの子だったから黒であり、場合5が場合6、7または8に変換された
                }
                // 場合6、7、8は節点wが黒のときに起こる.これらの場合はwの子の色によって区別する
                if (   w->left->color  == color::black
                    && w->right->color == color::black   ) {
                    // 場合6: xの兄弟wもwの両方の子もすべて黒の場合
                    // xとwの両方から黒を1つ取り除き、xを黒、wを赤にする
                    // xとwから黒を1つ取り除く代わりに、元々赤か黒だったx.pに特黒を付加する
                    w->color = color::red;
                    x = x->p;                  // x.pを新たな節点xとしてループを繰り返す
                    // 場合5を経由して場合6が実行された場合、元々x.pは赤だったから新しい節点xは赤黒である
                    // したがって、新しい節点xのcolor属性の値cはREDだから、while文は繰り返し判定を行い停止する
                }
                else {
                    if (w->left->color == color::black) {
                        // 場合7: xの兄弟wは黒、wの右の子は赤、wの左の子は黒の場合
                        // このとき、wとその右の子w.rigthの色を交換し、w上で左回転を行っても2色条件に対する違反を発生しない
                        w->right->color = color::black;
                        w->color        = color::red;
                        leftrot(w);
                        w = x->p->left;        // xの新しい兄弟wは黒、その左の子が赤だから、場合7が場合8に変換された 
                    }
                    // 場合8: xの兄弟wが黒でwの左の子が赤の場合
                    // 数回の再々色とx.p上の右回転を行うことで、2色条件に対する違反を生み出すことなく
                    // xの特黒を取り除いて(普通の)黒にできる
                    w->color = x->p->color;
                    x->p->color = color::black;
                    w->left->color   = color::black;
                    rightrot(x->p);
                    x = root;                  // xを根に設定すると、while文は繰り返し判定を行って停止する
                }
            }
        }
        x->color = color::black;  // 節点xを黒に再彩色する
    }


private:
    /**
     * @brief  中間順木巡回を行う
     * @note   n個の節点を持つ2分探索木の巡回はΘ(n)時間かかる
     * @tparam class F Node*を引数にとる関数オブジェクトの型
     * @param  F fn    Node*を引数にとる関数オブジェクト
     * @param  node*x  巡回を行う部分木の根
     */
    template <class F>
    void inorder(node*x, F fn)
    {
        // xがT.nilを指すとき、再帰は底をつく(葉に達していた)
        if (x == nil) {
			return;
        }
        inorder(x->left, fn);
        fn(x->key);
        inorder(x->right, fn);
    }

    /**
     * @brief  2色木Tからある与えられたキーを持つ節点を探索する
     * @note   実行時間はΟ(lgn)
     * @param  Key  k キー
     * @param  node*x 探索を行う部分木の根
     * @return キーkを持つ節点が存在すれば、その節点を指すポインタを返し、
     *         存在しなければ、T.nilを返す
     */
    node*find(Key k, node*x)
    {
        while (x != nil && neq(x->key, k)) {
            if (cmp(k, x->key)) {  // kがx.keyより小さければ、
                x =  x->left;      // 左部分木に対し、探索続行
            }
            else {                 // kがx.keyより大きければ、
                x = x->right;      // 右部分木に対し、探索続行
            }
        }
        return x;
    }

    /**
     * @brief 節点xを根とする部分木の中で最小のキーを持つ節点を指すポインタを返す
     * @note  実行時間はΟ(lgn)
     */
    node*leftmost(node*x)
    {
        // 節点xに左部分木がなければ、xの右部分木に出現する任意のキーはx.key以上だから、
        // xを根とする部分木に出現する最小のキーはx.keyである
        // 節点xが左部分木をもつなら、右部分木に出現するどのキーもx.key以上で、
        // 左部分木に出現するどのキーもx.key以下だから、
        // xを根とする部分木に出現する最小のキーはx.leftを根とする部分木の中に存在する
        while (x->left != nil) {
            x = x->left;
        }
        return x;
    }

    /**
     * @brief 節点xを根とする部分木の中で最大のキーを持つ節点を指すポインタを返す
     * @note  実行時間はΟ(lgn)
     */
    node*rightmost(node*x)
    {
        // 節点xに右部分木がなければ、xの左部分木二出現する任意のキーはx.key以下だから、
        // xを根とする部分木に出現する最大のキーはx.keyである
        // 節点xが右部分木をもつなら、左部分木に出現するどのキーもx.key以下で、
        // 右部分木に出現するどのキーもx.key以上だから、
        // xを根とする部分木に出現する最大のキーはx.rightを根とする部分木の中に存在する
        while (x->right != nil) {
            x = x->right;
        }
        return x;
    }

    /**
     * @brief 2分探索木の節点xに次節点が存在するときには次節点を返し、
     *        xのキーが木の中で最大ならば、T.nilを返す
     * @note  実行時間はΟ(lgn)
     */
    node*succ(node*x)
    {
        if (x->right != nil) {          // 節点xが右部分木を持つ場合は、
            return leftmost(x->right);  // xの次節点は右部分木の最左節点である
        }
        // (xが次節点yを持ち)節点xに右部分木がない場合は、
        // yは、xの祖先で、その左の子もxの祖先であるものの中で最もxに近い節点である
        node*y = x->p;
        // xから開始し、ある節点の左の子に出会うまで木を根に向かって遡る
        while (y != nil && x == y->right) {
            x = y;
            y = y->p;
        }
        return y;
    }

    /**
     * @brief 2分探索木の節点xに先行節点が存在するときには先行節点を返し、
     *        xのキーが木の中で最小ならば、T.nilを返す
     * @note  実行時間はΟ(lgn)
     */
    node*pred(node*x)
    {
        if (x->left != nil) {           // 節点xが左部分木を持つ場合は、
            return rightmost(x->left);  // xの先行節点は左部分木の最右節点である
        }
        // (xが先行節点を持ち)節点xに左部分木がない場合は、
        // yは、xの祖先で、その右の子もxの祖先であるものの中で最もxに近い節点である
        node*y = x->p;
        // xから開始し、ある節点の右の子に出会うまで木を根に向かって遡る
        while (y != nil && x == y->left) {
            x = y;
            y = y->p;
        }
        return y;
    }

private:
    /**< @brief キー同士の非同値判定を行う */
    bool neq(const Key& l, const Key& r)
    {
        return cmp(l, r) || cmp(r, l);
    }
    /**< @brief 節点xの記憶領域を確保する */
    node* allocnode(const Key& k, const T& v)
    {
        return new node(k, v);
    }
    /**< @brief 節点xの記憶領域を解放する */
    void freenode(node*x)
    {
        delete x; x = nullptr;
    }
    /**< @brief 与えられた値を4の倍数に切りあげた数を返す */
    static constexpr std::size_t align4(std::size_t x) noexcept
    {
        return x + (-x & 3);
    }

};



#endif  // end of __REDBLACKTREE_HPP__


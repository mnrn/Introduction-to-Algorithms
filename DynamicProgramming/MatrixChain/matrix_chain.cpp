/**
 * @brief 動的計画法の例として連鎖行列席を扱う
 *
 * @note  n個の行列の列として<A1, A2, ..., An>が与えられたとき、積
 *          A1A2・・・An  ...(*1)
 *        を計算することを考える. 行列の積を取る順序の曖昧さが連鎖行列に括弧付けを施すことで解消されているならば、
 *        2つの行列の積を求める標準的なアルゴリズムをサブルーチンとして用いて式(*1)を計算できる. 行列の積は結合的だから、
 *        どのように括弧付けをしても答えは同じである. ある行列の積が完全に括弧付けされている(fully parenthesize)のはそれが
 *        単一の行列であるか、あるいは2つの完全に括弧付けされた行列の積が括弧で囲まれているときである
 *        たとえば、連鎖行列<A1, A2, A3, A4>に対して、積A1A2A3A4を完全に括弧付けする5通りの方法
 *          (A1(A2(A3A4)))
 *          (A1((A2A3)A4))
 *          ((A1A2)(A3A4))
 *          ((A1(A2A3))A4)
 *          (((A1A2)A3)A4)
 *        がある. 連鎖行列に対する括弧付けの仕方が積の計算コストに劇的な影響を与えることがある
 *        まず、2つの行列の積を計算コストを考えてみよう. 2つの行列AとBを掛けることができるのは、
 *        それらが両立可能な行列(compatible matrix)の場合、すなわちAの列数がBの行数に等しいときだけである
 *        Aがp x q型の行列でBがq x r型の行列ならば、結果Cはp x r型の行列である
 *        Cの計算時間を支配するのはスカラー乗算の回数pqrである. 以下では、実行時間をスカラー乗算回数で評価する
 *        
 *        括弧付けの仕方によって連鎖行列の計算コストが変化することを示すために、サイズが3の連鎖行列<A1, A2, A3>を考える
 *        行列の次元をそれぞれ10x100, 100x5, 5x50とする. ((A1A2)A3)と括弧付けすると、10x5型行列である積A1A2を計算するのに
 *        10*100*5 = 5000回のスカラー乗算が必要であり、この行列をA3と掛けるのに10*5*50=2500回のスカラー乗算が必要だから、
 *        全部で7500回のスカラー乗算が必要である. 一方、(A1(A2A3))と括弧付けると、100x50型の行列である積A2A3を計算するのに
 *        100*5*50 = 25000回のスカラー乗算が必要であり、A1をこの行列に掛けるのに10*100*50=50000回のスカラー乗算が必要だから、
 *        全部で75000回のスカラー乗算が必要である. したがって、最初の括弧付けで計算するほうが10倍速い
 *        
 *
 * @date  2016/03/16
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <iostream>
#include <vector>



//****************************************
// 型シノニム
//****************************************

using elem_t   = std::int32_t;
using vec_t    = std::vector<elem_t>;
using mat_t    = std::vector<vec_t>;

using order_t  = std::int32_t;
using orders_t = std::vector<order_t>;
using ordmat_t = std::vector<orders_t>;



//****************************************
// 関数の宣言
//****************************************

order_t lookup_chain(ordmat_t& m, const orders_t& p, order_t i, order_t j);



//****************************************
// 関数の定義
//****************************************

/**
 * @brief  最適括弧付けを行う
 *
 * @note   m[i, j]を再帰的に定義する. i = jならば問題は自明である. この連鎖は1つの行列Ai..i = Aiから構成されているから、
 *         積の計算にスカラー乗算は必要ない. したがって、すべてのi = 1, 2, ..., nに対してm[i, i] = 0である
 *         つぎに、i < jのときにm[i, j]を計算するために、第1段階で観察した部分構造最適性を用いる
 *         最適な括弧付けはAiA(i+1)...AjをAkとA(k+1)..jの間で分割すると仮定しよう. ただし、i <= k < jである
 *         m[i, j]は部分積Ai..kとA(k+1)..jを計算するための最小コストにこれら2つの行列を掛け合わせるためのコストを加えたものに等しい
 *         各行列Aiはp(i-1) x pi型だから、行列積Ai..kA(k+1)..jはp(i-1)pkpj回のスカラー乗算で計算できる. したがって、
 *           m[i, j] = m[i, k] + m[k+1, j] + p(i-1)pkpj
 *         が成立する
 *         この再帰方程式では未知の値kを既知であると仮定している. しかし、kの取りうる値はj - i通り、すなわちk = i,i+1,...,j-1だけであり、
 *         最適括弧付けはこれらのkの値のうちの1つを取るはずだから、これらをすべて調べて最良のものを求めればよい. したがって、
 *         積AiA(i+1)...Ajの計算に最小限必要なコストの再帰的定義は
 *           m[i, j] = { 0                                               when i = j
 *                       min[i<=k<j] { m[i, k] + m[k+1, j] + p(i-1)pkpj  when i < j }
 *         となる
 *         値m[i, j]は部分問題の最適解のコストであるが、最適解を構成するにはこれらの値だけでは不十分である
 *         最適解の構成を容易にするために、積AiA(i+1)...Ajの最適な括弧付けにおける分割地点kの値をs[i, j]として定義する
 *         すなわち、s[i, j]はm[i, j] = m[i, k] + m[k+1, j] + p(i-1)pkpjを達成するkの値である
 */
std::pair<ordmat_t, ordmat_t> matrix_chain_order(const orders_t& p)
{
    order_t n = p.size() - 1;  // n = p.length - 1

    ordmat_t m(n+1, orders_t(n+1)), s(n+1, orders_t(n+1));  // m[1..n, 1..n]とs[1..n-1, 2..n]を新しい表とする

    for (order_t i = 1; i <= n; i++) {  // i = 1, 2, ..., nに対して
        m[i][i] = 0;                    // m[i, i] = 0を実行する
    }

    // for文の最初の繰り返しで、漸化式(*2)を用いて、長さl = 2の連鎖に対する最小コストm[i, i+1]をi = 1, 2, ...,n-1に対して計算する
    // このループの2回目の繰り返しでは、長さl = 3の連鎖に対する最小コストm[i, i+2]をi = 1, 2, ..., n-2に対して計算し、以下同様に繰り返しが進む
    // どの段階でも、最も内側のfor文で実行される計算コストm[i][j]は表の計算済みの要素m[i, k]とm[k+1, j]だけに依存する
    
    for (order_t l = 2; l <= n; l++) {  // lは連鎖の長さ

        for (order_t i = 1; i <= n - l + 1; i++) {

            order_t j = i + l - 1;

            m[i][j] = std::numeric_limits<order_t>::max();

            for (order_t k = i; k < j; k++) {

                order_t q = m[i][k] + m[k + 1][j] + p[i - 1] * p[k] * p[j];

                if (q < m[i][j]) {
                    m[i][j] = q;
                    s[i][j] = k;
                }
            }
        }
    }
    return std::make_pair(m, s);
}


void print_optimal_parens(const ordmat_t& s, order_t i, order_t j)
{
    if (i == j) {
        std::cout << "A" << i;
    }
    else {
        std::cout << "(";
        print_optimal_parens(s, i, s[i][j]);
        print_optimal_parens(s, s[i][j] + 1, j);
        std::cout << ")";
    }
}


/**
 * @brief 連鎖行列積Ai..j = AiA(i+1)...Ajを計算するために必要な最小スカラー乗算回数m[i, j]を決定する
 *        (効率の良くない)再帰手続き
 */
order_t recursive_matrix_chain(const orders_t& p, order_t i, order_t j)
{
    if (i == j) {
        return 0;
    }

    order_t n = p.size() - 1;  // n = p.length - 1

    ordmat_t m(n+1, orders_t(n+1));

    m[i][j] = std::numeric_limits<order_t>::max();

    for (order_t k = i; k < j; k++) {
        order_t q = recursive_matrix_chain(p, i, k)
            + recursive_matrix_chain(p, k + 1, j)
            + p[i-1] * p[k] * p[j];

        if (q < m[i][j]) {
            m[i][j] = q;
        }
    }

    return m[i][j];
}


/**
 * @brief  手続きRECURSIVE-MATRIX-CHAINの履歴管理版
 *
 * @note   履歴管理を用いる再帰アルゴリズムは各部分問題の解を表のある要素に保存する
 *         初期状態では表の各要素にはまだ値が代入されていないことを示す特別な初期値が格納されている
 *         再帰アルゴリズムを実行中にある部分問題が初めて現れると、その解を計算し、表の対応する要素に保存する
 *         この部分問題が再び出現したときには、単に表を検索してその値を返す
 */
order_t memoized_matrix_chain(const orders_t& p)
{
    order_t n = p.size() - 1;

    ordmat_t m(n+1, orders_t(n+1));  // m[1..n, 1..n]を新しい表とする

    for (order_t i = 1; i <= n; i++) {
        for (order_t j = i; j <= n; j++) {

            // 表の各要素の初期値は無限大であり、その要素がまだ計算されていないことを示す
            m[i][j] = std::numeric_limits<order_t>::max();
        }
    }

    return lookup_chain(m, p, 1, n);
}


/**
 * @brief  手続きMEMOIZED-MATRIX-CHAINの補助手続き
 */
order_t lookup_chain(ordmat_t& m, const orders_t& p, order_t i, order_t j)
{
    if (m[i][j] < std::numeric_limits<order_t>::max()) {  // m[i][j] < ∞ならば
        return m[i][j];                                   // 計算済みのコストm[i, j]を返す
    }

    // m[i, j] = ∞ならば、そのコストをRECURSIVE-MATRIX-CHAINと同じ方法で計算し、m[i, j]に保存した上で返す
    if (i == j) {
        m[i][j] = 0;
    }
    else {
        for (order_t k = i; k < j; k++) {
            order_t q = lookup_chain(m, p, i, k)
                + lookup_chain(m, p, k+1, j)
                + p[i-1] * p[k] * p[j];

            if (q < m[i][j]) {
                m[i][j] = q;
            }
        }
    }
    return m[i][j];
}



int main()
{
    orders_t p = { 30, 35, 15, 5, 10, 20, 25, };

    const order_t n = 6;

    ordmat_t m(n+1, orders_t(n+1)), s(n+1, orders_t(n+1));

    std::tie(m, s) = matrix_chain_order(p);

    std::cout << m[1][n] << std::endl;

    print_optimal_parens(s, 1, n);

    std::cout << std::endl;

    std::cout << recursive_matrix_chain(p, 1, n) << std::endl;

    std::cout << memoized_matrix_chain(p) << std::endl;
    
    return 0;
}

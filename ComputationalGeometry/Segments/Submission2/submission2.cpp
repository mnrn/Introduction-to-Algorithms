/**
 * @brief 線分を扱います
 * @date  2016/03/20
 */


//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <vector>
#include <algorithm>
#include <cmath>

#include <iostream>
#include <set>



//****************************************
// オブジェクト形式マクロの定義
//****************************************

//#define GEOMETRY_BEGIN namespace geom {
//#define GEOMETRY_END }



//****************************************
// 型シノニム
//****************************************

using elem_t     = long double;
using index_t    = std::int32_t;
using indices_t  = std::vector<index_t>;



//****************************************
// 構造体の定義
//****************************************

struct point {
    elem_t x, y;
    point() : x(0), y(0) {}
    point(elem_t x, elem_t y) : x(x), y(y) {}

    point& operator+=(const point& p) { x += p.x; y += p.y; return *this; }
    point& operator-=(const point& p) { x -= p.x; y -= p.y; return *this; }
    point& operator*=(const elem_t d) { x *= d; y *= d; return *this;}

    point operator+(const point& p)  const { return point(*this) += p; }
    point operator-(const point& p)  const { return point(*this) -= p; }
    point operator*(const elem_t& d) const { return point(*this) *= d; }
};


struct segment {
    point ps;  // 線分の始点
    point pd;  // 線分の終点
};



namespace limits {
    constexpr auto pi  = 3.141592653589793238;
    constexpr auto eps = 1e-10;
    constexpr auto inf = 1e12;
}


/**
 * @brief   与えられた3点a, b, cをa -> b -> cと進むとき、
 * @details a -> bで時計方向に折れてb -> c(cw)
 *          a -> bで反時計方向に折れてb -> c(ccw)
 *          a -> bで逆を向いてaを通り越してb -> c (back)
 *          a -> bでそのままb -> c(front)
 *          a -> bで逆を向いてb -> cまたは、b == c(on)
 */
enum struct orient {
    cw    = +1,
    ccw   = -1,
    back  = -2,
    front = +2,
    on    = 0,
};



//****************************************
// 型シノニムその2
//****************************************

using vector_t   = point;
using polygon_t  = std::vector<point>;
using segments_t = std::vector<segment>;



//****************************************
// 関数の定義
//****************************************

/**
 * @brief  2つのベクトルa, bからなる行列A = (a, b)の行列式(determinant)を返します
 * @param  const vector_t& a ベクトルa
 * @param  const vector_t& b ベクトルb
 * @return elem_t det(A)    行列式|(a, b)|
 */
static constexpr elem_t det(const vector_t& a, const vector_t& b)
{
    return a.x * b.y - a.y * b.x;
}


/**
 * @brief  2つのベクトルa, bのクロス積(cross product)a x bを返します
 * @param  const vector_t& a  ベクトルa
 * @param  const vector_t& b  ベクトルb
 * @return elem_t a x b      クロス積a x b
 */
static constexpr elem_t cross(const vector_t& a, const vector_t& b)
{
    return a.x * b.y - a.y * b.x;
    //return det(a, b);
}


/**
 * @brief  2つのベクトルa, bのドット積(dot product)a・bを返します
 * @param  const vector_t& a  ベクトルa
 * @param  const vector_t& b  ベクトルb
 * @return elem_t a・b       ドット積a・b
 */
static constexpr elem_t dot(const vector_t& a, const vector_t& b)
{
    return a.x * b.x + a.y * b.y;
}


/**
 * @brief  ベクトルvのノルム(norm・大きさの2乗)を返します
 * @param  const vector_t& v
 * @return elem_t norm(v);
 */
static constexpr elem_t norm(const vector_t& v)
{
    return v.x * v.x + v.y * v.y;
    // return dot(v, v);
}


/**
 * @brief  ベクトルvの大きさ|v|(原点からの距離)を返します
 * @param  const vector_t& v
 * @return elem_t sqrt(norm(v))
 */
static inline elem_t abs(const vector_t& v)
{
    return std::sqrt(norm(v));
}


/**
 * @brief  点pから線分sに下ろした垂線と線分sの交点を返します
 * @param  const segment& s 線分s
 * @param  const point& p   点p
 * @return 垂線と線分の交点
 */
static inline point proj(const segment& s, const point& p)
{
    vector_t base = s.pd - s.ps;
    vector_t hypo = p - s.ps;
    elem_t r = dot(hypo, base) / norm(base);
    return s.ps + base * r;
}


/**
 * @brief  点pと線対称の位置に存在する点を返します
 * @param  cosnt segment& s 線分s
 * @param  const point&   p 点p
 * @param  pと線対称な点
 */
static inline point reflect(const segment& s, const point p)
{
    return p + (proj(s, p) - p) * 2.0;
}


/**
 * @brief  絶対許容誤差(absolute tolerance)を比較します
 *
 * @note   2つの浮動小数点数値が等しいかどうか比較するためのイプシロン許容誤差の利用は、
 *         イプシロンの値が固定されているので、絶対許容誤差(absolute tolerance)と呼ばれている
 *         絶対許容誤差の欠点は適切なイプシロンの値を見つけるのが困難なことである
 *         イプシロンの値は入力データの値の範囲、および使用している浮動小数点の形式に依存する
 *         浮動小数点数の範囲全体に対するイプシロンの値を1つだけ選ぶことは不可能である
 *         xおよびyの値が非常に小さな(互いに等しくない)値の場合は、その差は常にイプシロンよりも小さくなる可能性があり、
 *         逆に大きな値の場合は、その差はイプシロンよりも常に大きくなるかもしれない. 別の見方として、
 *         判定している数が大きくなればなるほど、絶対値による判定が成立するために必要な桁数はどんどん大きくなっていく
 *
 * @note   固定されているイプシロンよりも数値が十分大きくなったとき、数値が正確に等しくない限り判定は常に失敗する
 *         これは通常、意図したことではない. 絶対許容誤差は数値の桁数の大きさが予めわかっており、
 *         許容誤差の値をそれに応じて設定することができる場合にのみ利用するべきである
 */
static inline bool absolute_tolerance_compare(elem_t x, elem_t y) noexcept
{
    return std::fabs(x - y) <= limits::eps;
}


/**
 * @brief  相対許容誤差(relatice tolerance)を比較します
 *
 * @note   基本的な考え方はある数を別の数によって除算し、その結果がどのくらい1に近づいているかを見るというものである
 *
 *
 * @note   |x| <= |y|を仮定すると、判定は以下のようになる
 *           if (Abs(x/y - 1.0) <= epsilon)...
 *         これは以下のように書き直せる
 *           if (Abs((x - y) / y) <= epsilon)...
 *         コストのかかる除算を避け、ゼロによる除算のエラーから守るために、後者の式の両辺にAbs(y)を乗算して、以下のように単純化する
 *           if (Abs(x - y) <= epsilon * Abs(y))...
 *         仮定|x| <= |y|を取り除くと、式は最終的に以下のようになる
 *           if (Abs(x - y) <= epsilon * Max(Abs(x), Abs(y)))...  // 相対許容誤差の比較
 *
 *
 * @note   比較において相対的な判定は「より小さいか等しい」であり、「より小さい」ではないことは重要である
 *         もしそうでなければ、両方の数が正確にゼロだった場合、判定は失敗する。相対的な判定も問題がないわけではない
 *         判定の式はAbs(x)およびAbs(y)が1よりも大きいときには、望み通りの働きをするが、それらの数値が1よりも小さいときは、
 *         イプシロンはより小さくないと効力がなくなってしまい、それらの数値が小さくなるほど式を成立させるのに必要な桁数はより多く必要になる         
 */
static inline bool relative_tolerance_compare(elem_t x, elem_t y) noexcept
{
    return std::fabs(x - y) <= limits::eps * std::max(std::fabs(x), std::fabs(y));
}


/**
 * @brief  上記2つの判定を1つに結合させる
 * @note   数値の絶対値が1よりも大きい場合には、相対的な判定を用い、1よりも小さい場合には、絶対的な判定を用いる
 * @attention  この式はMax()が機械語による命令によって利用できない場合には高価な計算になる可能性がある
 */
static inline bool combined_tolerance_compare(elem_t x, elem_t y) noexcept
{
    return std::fabs(x - y) <= limits::eps * std::max({ std::fabs(x), std::fabs(y), static_cast<elem_t>(1.0) });
}


/**
 * @brief COMBINED-TOLERANCE-COMPAREより少ない労力で行える近似的な判定
 */
static inline bool approximate_combined_tolerance_compare(elem_t x, elem_t y) noexcept
{
    return std::fabs(x - y) <= limits::eps * (std::fabs(x) + std::fabs(y) + 1.0);
}


/**
 * @brief 手続きAPPROXIMATE-COMBINED-TOLERANCE-COMPAREの短い名前
 */
static inline bool eq(elem_t x, elem_t y)
{
    return approximate_combined_tolerance_compare(x, y);
}


/**
 * @brinf 2点(p1, p2)のp0に関する偏角(polar angle)から、
 *        p0から見た2つのベクトルp0p1↑, p0p2↑の方向を返す
 */
static inline orient orientation(point p0, point p1, point p2)
{
    p1 -= p0; p2 -= p0;

    if (cross(p1, p2) > limits::eps)  { return orient::cw;    }  // クロス積(p1-p0)x(p2-p0)が正の場合、cw  ...(*1)
    if (cross(p1, p2) < -limits::eps) { return orient::ccw;   }  // クロス積(p1-p0)x(p2-p0)が負の場合、ccw ...(*2)


    // (*1), (*2)に当てはまらないとき、p2は直線p0p1|上(線分p0p1↑上とは限らない)に存在する

    if (dot(p1, p2) < -limits::eps)   { return orient::back;  }  // ドット積(p1-p0)・(p2-p0)が負の場合、p2->p0->p1(back) ...(*3)

    // (*3)に当てはまらないとき、p2はp0->p1->p2またはp0->p2->p1の位置に存在する

    if (eq(norm(p1), norm(p2)))       { return orient::front; }  // p0p2↑の大きさがp0p1↑の大きさより大きい場合、p0->p1->p2(front) ...(*4)

    // (*4)に当てはまらないとき、p0->p2->p1(on)
    return orient::on;
}


/**
 * @brief 2点(p1, p2)を原点p0に関する偏角に従って、比較を行う
 */
static inline int polar_angle_cmp(const point& p1, const point& p2)
{
    const point p0(0, 0);
    switch(orientation(p0, p1, p2)) {
      case orient::cw    : return 1;
      case orient::ccw   : return -1;
      case orient::back  : return 1;
      case orient::front : return -1;
      case orient::on    : return 1;
    }
    return 0;
}



//****************************************
// 構造体の定義
//****************************************

enum struct event_type {
    // ソートを行ったとき、leftが先に来るようにする
    left   = 0,
    right  = 1,
};


struct event_point {
    point p;        /**< 座標 */
    index_t seg;      /**< 線分のインデックス */
    event_type e;     /**< イベント点の種類 */

    event_point() = default;
    event_point(const point& p, index_t seg, event_type type)
        : p(p), seg(seg), e(type) { }

    bool operator < (const event_point& ep) const {
        // 端点を(x, e, y)上の辞書式順序で比較する
        
        // p1.x == p2.xのとき...(*1)、
        if (eq(p.x, ep.p.x)) {
            
            // (*1)かつ、p1.e == p2.eのとき、
            if (e == ep.e) {
                // p1.y < p2.yを返す
                return p.y < ep.p.y;
            }
            // (*1)かつ、p1.e != p2.eのとき、
            else {  // p1.e < p2.eを返す
                return static_cast<int>(e) < static_cast<int>(ep.e);
            }
        }
        // p1.x != p2.xのとき、
        else {  // p1.x < p2.x返す
            return p.x < ep.p.x;
        }
    }
};



//****************************************
// 型シノニム
//****************************************

using event_points = std::vector<event_point>;



//****************************************
// 関数の定義
//****************************************

/**
 * @brief  3点(pi, pj, pk)を引数に取り、クロス積(pk - pi) x (pj - pi)を返す
 * @note   direction > epsのとき、cw(clockwise)...ただし、定義によってはccw
 *         direction < -epsのとき、ccw(counterclockwise)...ただし、定義によってはcw
 *         それ以外のとき、0であり、境界条件が発生する. このとき、ベクトルは同一直線上(colinear)にあり、
 *         それらの方向は同じか互いに逆である     
 */
elem_t direction(const point& pi, const point& pj, const point& pk)
{
    return cross(pk - pi, pj - pi);
}


/**
 * @brief  pkがpipj|の端点の間にあるか否かを判定する
 *
 * @note   この手続きは、pkが線分pipj|と同一直線上にあると仮定する
 */
bool on_segment(const point& pi, const point& pj, const point& pk)
{
    elem_t xi = pi.x, xj = pj.x, xk = pk.x;
    elem_t yi = pi.y, yj = pj.y, yk = pk.y;
    
    return std::min(xi, xj) <= xk && xk <= std::max(xi, xj)
        && std::min(yi, yj) <= yk && yk <= std::max(yi, yj);
}


/**
 * @brief  2本の線分の交差判定
 *
 * @note   2本の線分の交差性を判定するために、各線分が他方を含む直線を跨ぐか否か調べる
 *         線分p1p2|がある直線を跨ぐ(straddle)のは、点p1がこの直線の一方の側にあり、
 *         点p2が他方の側にあるときである. 境界となるのは、p1かp2が直線上にある場合である
 *         2本の線分が交差するための必要十分条件は次の条件の一方(あるいは両方)が成り立つときである
 *
 *           1. どちらの線分も他方を含む直線を跨ぐ
 *           2. 一方の線分の端点が線分上にある(この条件は境界上にある場合から発生する)
 *
 * @note   このアイデアを次の手続きで実現する. SEGMENT-INTERSECTは、線分p1p2|と線分p3p4|が交差するときに
 *         TRUEを返し、そうでないときはFALSEを返す. この手続きは、サブルーチンDIRECTIONを呼び出して
 *         クロス積法を用いて相対的な方向を求め、ON-SEGMENTを呼び出して、線分を含む直線上にあることが分かっている点が
 *         この線分上にあるかどうかを判定する
 */
bool segment_intersect(const point& p1, const point& p2,
                       const point& p3, const point& p4)
{
    elem_t d1 = direction(p3, p4, p1);
    elem_t d2 = direction(p3, p4, p2);
    elem_t d3 = direction(p1, p2, p3);
    elem_t d4 = direction(p1, p2, p4);

    // 線分p1p2↑と線分p3p4↑が互いに他方の直線を跨ぐ場合
    if (   ((d1 > limits::eps && d2 < -limits::eps) || (d1 < -limits::eps && d2 > limits::eps))
        && ((d3 > limits::eps && d4 < -limits::eps) || (d3 < -limits::eps && d4 > limits::eps)))  {
        // p1p2|がp3p4|を含む直線を跨ぐから、クロス積(p1-p3)x(p2-p1)と(p4-p2)x(p2-p3)の符号は異なる
        // p3p4|がp1p2|を含む直線を跨ぐから、クロス積(p3-p1)x(p2-p1)と(p4-p1)x(p2-p1)の符号は異なる
        return true;
    }

    // そうではないとき、これらの線分が互いに他方を跨ぐことはないが、端点が他方の線分上にある余地は残る
    // どの相対的な方向も0でなければこの可能性は消える

    // ある相対的方向dkが0のときには、pkは他方の線分と同一直線上にある
    // pkがこの線分上にあるための必要十分条件は、これがこの線分の端点の間にあることである

    // ON-SEGMENT呼び出しにおいて、この線分は、第一引数を端点とする線分と異なる方の線分である

    else if (eq(d1, 0) && on_segment(p3, p4, p1)) {
        return true;
    }

    else if (eq(d2, 0) && on_segment(p3, p4, p2)) {
        return true;
    }

    else if (eq(d3, 0) && on_segment(p1, p2, p3)) {
        return true;
    }

    else if (eq(d4, 0) && on_segment(p1, p2, p4)) {
        return true;
    }

    else {
        return false;  // 0判定はすべて失敗し、FALSEを返す
    }
}


/**
 * @brief  線分交差検出を行う
 * 
 * @note   n本の線分の集合をSとし、Sの中に交差する線分対が1つでもあれば
 *         ブール値TRUEを返し、そうでなければFALSEを返す
 */
int any_segments_intersect(segments_t& S)
{
    // struct cmp { bool operator () (const segment& s1, const segment& s2) {
    //     return (s1.ps.x - s2.ps.x) * (s2.pd.y - s2.ps.y)
    //         <  (s1.ps.y - s2.ps.y) * (s2.pd.x - s2.ps.x);
    // } };
    
    std::set<index_t> T;  // 全擬順序Tを空に初期化する
    
    int n = S.size();
    event_points Q;      // イベントキューQを空に初期化する
    
    for (index_t i = 0; i < n; i++) {
        // 端点ps, pdが左下を基準に並ぶように調整する
        if (eq(S[i].ps.y, S[i].pd.y)) {
            if (S[i].ps.x> S[i].pd.x)   { std::swap(S[i].ps, S[i].pd); }
        }
        else if (S[i].ps.y > S[i].pd.y) { std::swap(S[i].ps, S[i].pd); }

        Q.emplace_back(S[i].ps, i, event_type::left);
        Q.emplace_back(S[i].pd, i, event_type::right);
    }

    // Sの線分の端点を左から右にソートする
    std::sort(Q.begin(), Q.end());

    int count = 0;
    for (index_t i = 0; i < 2 * n; i++) {   // イベント点の個数は2nだからループの繰り返しは2n

        if (Q[i].e == event_type::left)  {  // pが線分sの左端点

            for (index_t s : T) {
                if (segment_intersect(S[Q[i].seg].ps, S[Q[i].seg].pd,
                                      S[s].ps, S[s].pd)) {
                    count = count + 1;
                }
            }         
            T.insert(Q[i].seg);
        }
        if (Q[i].e == event_type::right) {  // pが線分sの右端点
            T.erase(Q[i].seg);
        }
    }

    return count;
}



//****************************************
// エントリポイント
//****************************************

int main()
{
    using namespace std;

    int s;
    cin >> s;

    segments_t S(s);
    for (int i = 0; i < s; i++) {
        int x1, y1, x2, y2;
        cin >> x1 >> y1 >> x2 >> y2;
        S[i].ps = point(x1, y1);
        S[i].pd = point(x2, y2);
    }
    cout << any_segments_intersect(S) << endl;

    
    return 0;
}

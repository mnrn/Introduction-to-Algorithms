/**
 * @brief 線分を扱います
 * @date  2016/03/20
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include "segments.hpp"



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
 * @note   n本の線分の集合をSとし、Sの中で交差している線分の数を返す
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
            if (S[i].ps.x > S[i].pd.x)  { std::swap(S[i].ps, S[i].pd); }
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




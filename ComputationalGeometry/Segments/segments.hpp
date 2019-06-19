/**
 * @brief 線分を扱います
 * @date  2016/03/18 ~ 2016/03/20
 */



//****************************************
// インクルードガード
//****************************************

#ifndef __SEGMENTS_HPP__
#define __SEGMENTS_HPP__



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include "../Geometry/geometry.hpp"



//****************************************
// 関数の宣言
//****************************************

/**
 * @brief  3点(pi, pj, pk)を引数に取り、クロス積(pk - pi) x (pj - pi)を返す
 * @note   direction > epsのとき、cw(clockwise)...ただし、定義によってはccw
 *         direction < -epsのとき、ccw(counterclockwise)...ただし、定義によってはcw
 *         それ以外のとき、0であり、境界条件が発生する. このとき、ベクトルは同一直線上(colinear)にあり、
 *         それらの方向は同じか互いに逆である     
 */
elem_t direction(const point& pi, const point& pj, const point& pk);


/**
 * @brief  pkがpipj|の端点の間にあるか否かを判定する
 *
 * @note   この手続きは、pkが線分pipj|と同一直線上にあると仮定する
 */
bool on_segment(const point& pi, const point& pj, const point& pk);



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
                       const point& p3, const point& p4);


/**
 * @brief  線分交差検出を行う
 * 
 * @note   n本の線分の集合をSとし、Sの中で交差している線分の数を返す
 */
int any_segments_intersect(segments_t& S);



#endif  // end of __SEGMENTS_INTERSECT_HPP__


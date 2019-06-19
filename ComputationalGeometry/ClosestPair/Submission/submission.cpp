/**
 * @brief  最近点対の発見について扱います
 * @note   関連URL: http://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=0585
 * @date   2016/03/22
 */


//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <iomanip>



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



//****************************************
// 構造体の定義
//****************************************

struct cmp_x { bool operator()(const point& pi, const point& pj) { return pi.x < pj.x; } };
struct cmp_y { bool operator()(const point& pi, const point& pj) { return pi.y < pj.y; } };



//****************************************
// 関数の定義
//****************************************

elem_t closest_pair(point*P, index_t n)
{
    if (n <= 1) {            // |P| <= 1か否かを判定する
        return limits::inf;  // |P| <= 1ならば、再帰の基底
    }

    // |P| > 1ならば、再帰呼び出しによって、以下のように分割統治法を実行する

    // 分割: 以下の条件を満たすように、点集合Pを2つの集合PLとPRに分割する垂直線lを求める
    //      |PL|=ceil(|P|/2), |PR|=floor(|P|/2)であり、PLの点はすべてlの上または左に、
    //      PRの点はすべてlの上または右にある
    index_t m = n / 2;
    elem_t lx = P[m].x;  // NOTE: このとき、PRおよびPLはxを昇順に含む(presortingによって保証済み)

    // 統治: PをPLとPRに分割した後、1回はPLにおける最近点対を求めるために、
    //      もう1回はPRにおける最近点対を求めるために、2回の再帰呼び出しを行う
    //      PLとPRに対して求めた最近点対間距離をδLとδRとし、δ = min(δL, δR)と置く
    elem_t delta = std::min(closest_pair(P, m),
                            closest_pair(P + m, n - m));

    // 結合: 最近点対は、上の再帰呼び出しの一方で見つかった距離がδの点対か、1点がPLに属し、
    //      もう1点がPRに属する点対のどちらかである. アルゴリズムは、一方がPLに属し、
    //      他方がPRに属する点対で、距離がδ未満のものが存在するか否かを判定する
    //      距離がδ未満の点対を構成する2つの点は、それぞれ、直線lからδ単位距離以内に存在する
    //      したがって、このような2点は直線lを中心とする、幅2δの垂直帯領域に存在する
    //      このような点対を、存在するならば、以下の要領で発見する
    //
    //      1. 配列Y(P)から、2δ幅の垂直帯領域を除く領域に属するすべての点を除去してできる、
    //         配列Y'を生成する. 配列Y'はYと同様、y座標に関してソートされている
    //
    //      2. 配列Y'の各点pについて、pからδ単位未満の距離にあるY'の点を発見しようと努める
    //         このとき、Y'においてpに続く5個の点だけに候補対象を限定できる
    //         pからこれら5個の点までの距離を計算し、Y'のすべての点対について発見できた
    //         最近点対の距離δ'を記憶する
    //
    //      3. δ' < δならば、この垂直帯領域は再帰呼び出しが発見したものより近い点対を確かに含む
    //         そこで、この点対と距離δ'を返す. そうでなければ、再帰呼び出しが発見した最近点対と距離δを返す


    // NOTE: 配列Yと配列Yのプレソーティングを避けるため、ソート済み配列PLとPRをマージしてソート済み配列Yを構成する(Pの再構成)
    std::inplace_merge(P, P + m, P + n, cmp_y());

    polygon_t Y_;  // 直線lから距離がδ未満の点を入れていく
    elem_t delta_ = limits::inf;
    for (index_t i = 0; i < n; i++) {

        // 直線lよりδ以上離れている(幅2δの垂直帯領域に存在しない)点は考慮から外す
        if (std::fabs(P[i].x - lx) >= delta) { continue; }

        index_t limit = Y_.size();  // NOTE: Y'の要素数は高々5個である
        for (index_t j = 0; j < limit; j++) {

            // Y'に入っている点を、末尾からy座標の差がδ以上になるまで確認する
            point p = Y_[limit - j - 1];
            if ((P[i].y - p.y) >= delta) { break; }

            delta_ = std::min(delta_, norm(P[i] - p));
        }
        Y_.push_back(P[i]);
    }

    return std::min(delta, delta_);
}


elem_t closest_pair(polygon_t& P)
{
    std::sort(P.begin(), P.end(), cmp_x());   // xに関して、あらかじめソートしておく(presorting)
    return closest_pair(P.data(), P.size());  // 最初の再帰呼び出し
}



//****************************************
// エントリポイント
//****************************************

int main()
{
    using namespace std;

    int n;
    cin >> n;
    polygon_t P(n);
    for (int i = 0; i < n; i++) {
        int x, y;
        cin >> x >> y;
        P[i].x = x, P[i].y = y;
    }

    cout << closest_pair(P) << endl;

    return 0;
}

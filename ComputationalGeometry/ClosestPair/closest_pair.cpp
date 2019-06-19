/**
 * @brief  最近点対の発見について扱います
 * @date   2016/03/22
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include "closest_pair.hpp"
#include <iostream>
#include <iomanip>



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

            delta_ = std::min(delta_, abs(P[i] - p));
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



int main()
{
    using namespace std;

    int n;
    cin >> n;
    polygon_t P(n);
    for (int i = 0; i < n; i++) {
        elem_t x, y;
        cin >> x >> y;
        P[i].x = x, P[i].y = y;
    }

    cout << closest_pair(P) << endl;

    return 0;
}

/**
 * @brief 凸包の構成について扱います
 *
 * @note  点集合Qの各点がその境界上か内部にある最小の凸多角形PをQの凸包(convex hull)と言い、CH(Q)で表す
 *        集合Qの点はすべて異なり、同一直線上にはない3点を少なくとも含むと仮定する
 *        直観的には、Qの各点を板から突き出した釘であると考えることができる. これらすべての釘を取り囲む、強い輪ゴムが作る形状が凸包である
 *
 * @date  2016/03/21
 */


//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include "convex_hull.hpp"
#include <iostream>



//****************************************
// 関数の定義
//****************************************

/**
 * @brief 2点(p1, p2)をp0に関する偏角に従って、反時計回りの比較を行う
 */
static bool polar_angle_cmp(const point& p0, const point& p1, const point& p2)
{
    switch(cw(p0, p1, p2)) {
      case orientation::cw    : return true;
      case orientation::ccw   : return false;
      case orientation::back  : return false;
      case orientation::front : return true;
      case orientation::on    : return false;
    }
    return false;
}


/**
 * @brief  Grahamのスキャン(Graham's scan)では、候補点をスタックSで管理して、凸包問題を解く
 *
 * @note   入力集合Qに属するすべての点を一旦スタックにプッシュし、CH(Q)の頂点ではない点は最終的にスタックからポップする
 *         アルゴリズムが終了した時点で、スタックSはCH(Q)の頂点だけを、境界上で反時計回りに出現する順序で含んでいる
 *
 * @note   GRAHAM-SCANの実行時間はΟ(nlgn)である. ただし n = |Q|である
 *
 * @param  polygon_t&        Q  |Q| >= 3を満たす点集合Q
 * @return std::stack<point> S  底から先頭に向けて、CH(Q)の頂点だけを反時計回りで含む
 */
std::stack<point> graham_scan(polygon_t& Q)
{
    index_t n = Q.size();
    
    elem_t  min_y = Q[0].y;
    index_t min_i = 0;
    for (index_t i = 1; i < n; i++) {
        if (  (Q[i].y < min_y)                                // Qが含むy座標最小の点をp0とする
           || (eq(Q[i].y, min_y) &&(Q[i].x < Q[min_i].x))) {  // タイは最左のものをp0とすることで解消する
            min_i = i;
            min_y = Q[i].y;
        }
    }

    // Qの残りの点をp0の周りで反時計回りの偏角順にソートしたものを<p1,p2,..,pm>とする
    // 複数の点が同じ偏角を持つとき、p0から近いものから昇順としている
    std::swap(Q[0], Q[min_i]);
    auto cmp = [&Q](const point& pi, const point& pj) -> bool { return polar_angle_cmp(Q[0], pi, pj); };
    std::sort(Q.begin() + 1, Q.end(), cmp);  // STLの手続きsortはΟ(nlgn)で走ることが保障されている


    // 複数の点が同じ偏角を持つとき、p0から最も遠いもの以外を除去する(実行時間はΟ(n))
    auto vp = [&Q](index_t i) -> bool { orientation ori = cw(Q[0], Q[i], Q[i + 1]);
                                        return (ori == orientation::back || ori == orientation::front || ori == orientation::on); };
    index_t m = 1;
    for (index_t i = 1; i < n; i++) {
        while (i < n - 1 && vp(i)) { i++; }  // 同じ偏角を持たない点を探す
        Q[m] = Q[i];
        m = m + 1;
    }
    m = m - 1;
    
    std::stack<point> S;  // Sを空のスタックとする
    if (m < 2) { return S; }
    S.push(Q[0]);
    S.push(Q[1]);
    S.push(Q[2]);
    auto next_to_top = [](std::stack<point>& S) -> point {
        point pj = S.top(); S.pop(); point pk = S.top(); S.push(pj); return pk; };

    
    // ループ不変式 : for文の各繰り返しが開始される時点で、スタックSが格納する要素を底から先頭に向けて並べると、
    //              正確にCH(Q(i-1))の頂点だけが反時計回りの順に並ぶ

    
    // for文を最初に実行するとき、スタックSはQ2 = Q(i-1)の頂点を含み、この3点からなる集合は、それ自身の凸包である
    // さらに、この3点は、底から先頭に向けて反時計回りで現れる. したがって、この不変式は最初に実行するとき成立する
    for (index_t i = 3; i <= m; i++) {
        // for文の繰返しに入るとき、スタックの先頭は点p(i-1)である. そして、これは前回の繰返しが終わった後で
        // (あるいは、i = 3のときは、最初の繰返しの前に)スタックにプッシュされた
        
        while (cw(S.top(), next_to_top(S), Q[i]) != orientation::ccw) {
            S.pop();
        }
        // while文の実行を終了した後、piをプッシュする直前において、スタックの先頭に格納されている点をpj,
        // pjの下(すなわち、先頭から2つ目)の点をpkとする. pjがSの先頭にあり、piをまだプッシュしていない時点では、
        // スタックSが格納する点列はforループのj番目の繰返し直後と一致する. したがって、ループ不変式から、
        // Sが格納する要素を底から先頭に向けて並べると、正確にCH(Qj)の頂点だけが反時計回りにの順に並ぶ

        // 引き続き、piをプッシュする直前の時点に焦点を合わせる. p0の回りのpiの偏角はpjの偏角より大きく、
        // ∠pkpjpiは左折れである(そうでなければ、pjは既にポップされている). Sは正確にCH(Qj)の頂点を含むから、
        // piをプッシュすると、Sは正確にCH(Qj ∪ {pi})の頂点を含み、これらは依然としてそこから先頭に向けて
        // 反時計回りに並んでいる

        // ここでCH(Qj ∪ {pi}) = CH(Qi)を示す. for文のi番目の繰返しの間にスタックSからポップした任意の点をpt,
        // ptをポップしたときに、Sにおいてptのすぐ下に置かれていた点をprとする(prはpjかもしれない)
        // ∠prptpiは左折れではなく、p0の回りで、ptの偏角はprの偏角より大きい. ptは3点p0, pr, piによって作られる
        // 三角形の内部か辺上にある(が三角形の頂点ではない). ptはQiの(pi以外の)3点が作る三角形の内部にあるから、
        // 明らかに、CH(Qi)の頂点ではありえない. ptがCH(Qi)の頂点ではないから
        //   CH(Qi - {pt}) = CH(Qi)  ...(*1)
        // が成立する. for文のi回目の繰返しの間にポップした点の集合をPiとする. 式(*1)はPiのすべての点について適用できるから、
        // この式を繰返し適用すると、CH(Qi - Pi) = CH(Qi)である. しかし、Qi - Pi = Qj ∪ {pi}だから、
        // CH(Qj ∪ {pi}) = CH(Qi - Pi) = CH(Qi)である

        // piをプッシュした後では、スタックSが正確にCH(Qi)の頂点を含み、これらは底から先頭に向けて反時計回りに
        // 並んでいることを示した. iを1増やすと、次の繰返しに対してループ不変式が成立する
        S.push(Q[i]);
    }
    // ループが終了するとき、i = m + 1であって、ループ不変式から、スタックSは正確にCH(Qi)、すなわち、CH(Q)の頂点を含み、
    // これらは底から先頭に向けて反時計回りに並んでいる
    return S;
}


int main()
{
    using namespace std;
    int n;
    cin >> n;

    polygon_t poly(n);
    for (int i = 0; i < n; i++) {
        int x, y;
        cin >> x >> y;
        poly[i] = point(x, y);
    }

    stack<point> S = graham_scan(poly);

    cout << S.size() << endl;
    while (!S.empty()) {
        point p = S.top();
        cout << p.x << " " << p.y  << endl;
        S.pop();
    }
    
    return 0;
}


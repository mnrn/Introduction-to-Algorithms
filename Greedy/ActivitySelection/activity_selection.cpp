/**
 * @brief  貪欲法による活動選択問題の解答例を与えます
 *
 * @note   共有資源の排他的利用を要求して競合する複数の活動の中から両立可能なものを選択し、選択された活動数を最大化する問題を最初に検討する
 *         会議室のように同時には1つの活動しか利用できない資源の使用許可を申請するn個の活動(activity)の集合S = {a1,a2,...,an}を考える
 *         各活動iには開始時刻(start time)siと終了時刻(finish time)fiがある. ここで、0<=si,fi<∞であり、活動iの活動時間は半区間[si, fi)である
 *         区間[si, fi)と[sj, fj)が重ならないとき、活動iとjは両立可能(compatible)である. すなわち、si >= fjまたはsj >= fiならばaiとajは
 *         両立可能な活動から構成される集合で、サイズが最大のものを求めることが目的である. これらの活動はすでに終了時刻に関して昇順でソートされていること、すなわち
 *           f1 <= f2 <= f3 <= ... <= f(n-1) <= fn
 *         を仮定する
 *          
 * @date   2016/03/24
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <iostream>
#include <vector>
#include <algorithm>



//****************************************
// 型シノニム
//****************************************

using acttime_t  = std::int32_t;
using acttimes_t = std::vector<acttime_t>;
using index_t    = std::int32_t;



//****************************************
// 構造体の定義
//****************************************

struct activity {
    acttime_t s;  /**< 活動の開始時刻sk */
    acttime_t f;  /**< 活動の終了時刻fk */
    index_t k;    /**< 活動の添字  */

    activity(acttime_t s, acttime_t f, index_t k) : s(s), f(f), k(k) { }
};
using actvec_t = std::vector<activity>;



//****************************************
// 関数の定義
//****************************************

/**
 * @brief  配列sとfに格納されている各活動の開始時刻と終了時刻、解くべき部分問題Skを定義するための添字k, 
 *         そして元の問題のサイズnを引数として取り、Skに属する互いに両立可能な活動から構成されるサイズが最大の集合を返す
 *
 * @note   入力であるn個の活動は、終了時刻の昇順でソート済みであると仮定する. この仮定が成立しない場合には、
 *         Ο(nlgn)時間をかけて活動を事前にソートすれば良い(タイがあれば任意の順序で並べることにする)
 *
 * @note   部分問題S0が活動の全体集合Sになるように、f0 = 0であるダミーの活動a0を最初に加えておく
 *         初期呼び出しRECURSIVE-ACTIVITY-SELECTOR(s, f, 0, n)は、与えられた最初の問題全体を解く
 *
 * @note   活動が終了時刻に関してソート済みであると仮定すると、手続きRECURSIVE-ACTIVITY-SELECTOR(s,f,0,n)の
 *         実行時間はΘ(n)である. なぜなら、すべての再帰呼び出しの全体を通して、各活動はちょうど一度だけwhile文の
 *         検査を受ける(活動aiはk < iが成立する最後の呼び出しで検査される)からである
 */
actvec_t recursive_activity_selector(const acttimes_t& s, const acttimes_t& f, index_t k, index_t n)
{
    index_t m = k + 1;
    while (m <= n && s[m] < f[k]) {  // Skの中から終了時刻最小の活動を発見する
        m = m + 1;
    }

    if (m <= n) {
        activity am{s[m], f[m], m};
        actvec_t A{ am, };
        actvec_t A_ = recursive_activity_selector(s, f, m, n);
        A.insert(A.end(), A_.begin(), A_.end());  // 集合和演算を行う ({am} ∪ RECURSIVE-ACTIVITY-SELECTOR(s, f, m, n))
        return A;
    }
    else {           // akと両立可能な活動を発見できなかった場合は、
        return { };  // Sk = φ(空集合)であり、φを返す
    }
}


/**
 * @brief  手続きGREEDY-ACTIVITY-SELECTORは手続きRECURSIVE-ACTIVITY-SELECTORの繰り返し版である
 *
 * @note   これまでと同様、入力となる活動はその終了時刻に関して昇順でソート済みであると仮定する
 *         手続きは選択した活動を集合Aに集める. そして、終了した時、これが戻り値となる
 *
 * @note   変数kは再帰版における活動akに対応しており、Aに最後に突きつけられた活動を指す
 *         終了時刻の昇順で活動を検討するので、fkは常にAに属する活動が持つ最後の終了時刻である. したがって
 *           fk = max{fi : ai ∈ A }
 *         である
 *
 * @note   呼び出しGREEDY-ACTIVITY-SELECTOR(s,f)の戻り値Aは呼び出しRECURSIVE-ACTIVITY-SELECTOR(s,f,0,n)の
 *         戻り値である集合に等しい
 *
 * @note   入力時に活動がその終了時刻の昇順でソートされていると仮定すると、再帰版と同様、
 *         GREEDY-ACTIVITY-SELECTORはn個の活動からなる集合をΘ(n)時間でスケジュールする
 */
actvec_t greedy_activity_selector(const acttimes_t& s, const acttimes_t& f)
{
    index_t   n = s.size();       // n = s.length
    
    activity  a1{s[1], f[1], 1};  // 活動a1を選択し、
    actvec_t  A = { a1, };        // この活動だけを含むようにAを初期化し、
    index_t   k = 1;              // kをこの活動を指すように初期化する 

    // Skの中から終了時刻最小の活動を発見する
    for (index_t m = 2; m <= n; m++) {
        // 活動amを順番に調べ、
        if (s[m] >= f[k]) {                 // amがすでに選択したすべての活動と両立可能ならば、
            A.emplace_back(s[m], f[m], m);  // amをAに付け加え、
            k = m;                          // kをmに更新する
        }
    }
    return A;
}



//****************************************
// エントリポイント
//****************************************

int main()
{
    using namespace std;

    acttime_t inf = std::numeric_limits<acttime_t>::max();
    acttimes_t s = { inf, 1, 3, 0, 5, 3, 5, 6,  8,  8,  2,  12, };
    acttimes_t f = { 0,   4, 5, 6, 7, 9, 9, 10, 11, 12, 14, 16, };
    

    cout << "再帰型貪欲アルゴリズム" << endl;
    actvec_t A = recursive_activity_selector(s, f, 0, s.size());
    for (auto& a : A) {
        cout << a.k << endl;
    }
    cout << endl;

    
    cout << "繰り返し型貪欲アルゴリズム" << endl;
    A = greedy_activity_selector(s, f);
    for (auto& a : A) {
        cout << a.k << endl;
    }
    
    
    return 0;
}


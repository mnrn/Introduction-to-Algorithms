/**
 * @brief 素集合森のテストプログラム
 * @date  作成日     : 2016/02/16
 * @date  最終更新日 : 2016/02/16
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include "disjointset.hpp"
#include <iostream>



//****************************************
// 関数の定義
//****************************************

int main(void)
{
    using namespace std;
    int n, a, b, q, t;

    cin >> n >> q;
    disjointset ds(n);
    for (int i = 0; i < n; i++) {
        ds.makeset(i);
    }

    for (int i = 0; i < q; i++) {
        cin >> t >> a >> b;
        if (t == 0) {
            ds.merge(a, b);
        }
        else if (t == 1){
            cout << (ds.findset(a) == ds.findset(b)) << endl;
        }
    }

    return 0;
}


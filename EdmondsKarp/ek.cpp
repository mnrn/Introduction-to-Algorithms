/**
 * @brief  最大フロー問題におけるEdmonds-Karpアルゴリズムを扱う
 * @date   2016/03/03
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include "edmondskarp.hpp"
#include <iostream>



//****************************************
// 関数の定義
//****************************************

int main(void)
{
    using namespace std;
    int V, E;

    cin >> V >> E;
    
    graph_t G(V);
    for (int i = 0; i < E; i++) {
        int a, b, c;
        cin >> a >> b >> c;
        G[a].emplace_back(a, b, c);
    }
    edmondskarp ek(G);

    // ek.debug();
    auto max_flow = ek.execute(0,V-1);

    cout << max_flow << endl;

    return 0;
}


/**
 * @brief  最大フロー問題におけるFord-Fulkerson法を扱う
 * @date   作成日     : 2016/02/24
 * @date   最終更新日 : 2016/02/24
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include "fordfulkerson.hpp"
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

    fordfulkerson ff(G);
    
    auto max_flow = ff.execute(0,V-1);

    cout << max_flow << endl;

    return 0;
}


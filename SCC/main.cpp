/**
 * @brief 強連結成分(分解)アルゴリズムのテストを行う
 * @date  作成日     : 2016/02/14
 * @date  最終更新日  : 2016/02/14
 */


//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <iostream>
#include "scc.hpp"



//****************************************
// 関数の定義
//****************************************



int main(void)
{
    const index_t vs = 8;
    graph_t G;
    G.resize(vs);
    int adjs[vs][2] = {
        { 1, }, { 2, }, { 0, }, { 4, 6, },
        { 3, 7, }, { 6, }, { 5, 7 }, { 7, },
    };
    int adji[vs] = {1, 1, 1, 2, 2, 1, 2, 1, };

    for (int j = 0; j < vs; j++) {
        G[j].resize(adji[j]);
        std::cout << j << " : ";
        for (int i = 0; i < adji[j] ; i++) {
            G[j][i].src = j;
            G[j][i].dst = adjs[j][i];
            std::cout << G[j][i].dst << " ";
        }
        std::cout << std::endl;
    }
    
    std::cout << std::endl;

    auto _scc = scc(G);
    for (auto c : _scc) {
        std::cout << c << " ";
    }
    std::cout << std::endl;
}




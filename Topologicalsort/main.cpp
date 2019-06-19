/**
 * @brief トポロジカルソートのテスト
 * @date  作成日     : 2016/02/14
 * @date  最終更新日 : 2016/02/14
 */



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <iostream>
#include "tsort.hpp"



//****************************************
// 関数の実装
//****************************************



int main(void)
{
    const int vs = 9;
    const char* name[vs] = {
        "パンツ", "ズボン", "ベルト", "くつ", "ソックス",
         "時計", "シャツ", "ネクタイ", "上着",
    };

    graph_t G;
    G.resize(vs);
    int adjs[vs][2] = {
        { 1, 3, }, { 2, }, { 8, }, { graph::nil, }, { 3, },
        { graph::nil, }, { 2, 7, }, { 8, }, { graph::nil, },
    };
    int adji[vs] = {2, 1, 1, 0, 1, 0, 2, 1, 0, };

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
    


    array_t lst = tsort(G);
    
    for (int i = 0; i < vs; i++) {
        std::cout << name[lst[i]] << " ";
    }
    std::cout << std::endl;
}

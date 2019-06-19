/**
 * @brief  文字列照合における物置
 * @date   2016/03/31
 */



//****************************************
// インクルードガード
//****************************************

#ifndef __STRING_MATCHING_HPP__
#define __STRING_MATCHING_HPP__



//****************************************
// 必要なヘッダファイルのインクルード
//****************************************

#include <cstdint>
#include <cstring>
#include <vector>



//****************************************
// 型シノニム
//****************************************

using string_t  = const char*;
using strlen_t  = std::int32_t;

using radix_t   = std::int32_t;
using prime_t   = std::int32_t;

using state_t   = std::int32_t;
using states_t  = std::vector<state_t>;
using chridx_t  = std::int32_t;
using chrset_t  = std::vector<chridx_t>;
using delta_t   = std::vector<chrset_t>;



#endif  // end of __STRING_MATCHING_HPP__


#include "BitwiseOperations.hpp"

std::array<uint64_t, 64> bitmask_table;

void Init() {
  for (int i = 0; i < 64; ++i)
    bitmask_table[i] = GetBitMaskImpl(i);
}

int CountBits(uint64_t data) {
  data = (data & UINT64_C(0x5555555555555555))
      + ((data & UINT64_C(0xAAAAAAAAAAAAAAAA)) >> 1);
  data = (data & UINT64_C(0x3333333333333333))
      + ((data & UINT64_C(0xCCCCCCCCCCCCCCCC)) >> 2);
  data = (data & UINT64_C(0x0F0F0F0F0F0F0F0F))
      + ((data & UINT64_C(0xF0F0F0F0F0F0F0F0)) >> 4);
  data += data >> 8;
  data += data >> 16;
  data += data >> 32;
  return data & 0xFF;
}

uint8_t Column2Uint8(uint64_t data) {
  data = (data & UINT64_C(0x0001000100010001)) |
        ((data & UINT64_C(0x0100010001000100)) >> 7);
  data = (data & UINT64_C(0x0000000300000003)) |
        ((data & UINT64_C(0x0003000000030000)) >> 14);
  data = (data & UINT64_C(0x000000000000000F)) |
        ((data & UINT64_C(0x0000000F00000000)) >> 28);
  return static_cast<uint8_t>(data);
}

uint8_t CrossWhite2Uint8(uint64_t data) {
  data = (data & UINT64_C(0x0040001000040001)) |
        ((data & UINT64_C(0x8000200008000200)) >> 8);
  data = (data & UINT64_C(0x0000003000000003)) |
        ((data & UINT64_C(0x00C00000000C0000)) >> 16);
  data = (data & UINT64_C(0x000000000000000F)) |
        ((data & UINT64_C(0x000000F000000000)) >> 32);
  return static_cast<uint8_t>(data);
}

uint8_t CrossBlack2Uint8(uint64_t data) {
  data = ((data & UINT64_C(0x0002000800200080)) >> 1) |
         ((data & UINT64_C(0x0100040010004000)) >> 7);
  data = ((data & UINT64_C(0x0000000C000000C0)) >> 2) |
         ((data & UINT64_C(0x0003000000300000)) >> 14);
  data = ((data & UINT64_C(0x00000000000000F0)) >> 4) |
         ((data & UINT64_C(0x0000000F00000000)) >> 28);
  return static_cast<uint8_t>(data);
}


uint64_t Uint82Column(const uint8_t data) {
  uint64_t result = static_cast<uint64_t>(data);
  result = (result & UINT64_C(0x000000000000000F)) |
          ((result & UINT64_C(0x00000000000000F0)) << 28);
  result = (result & UINT64_C(0x0000000300000003)) |
          ((result & UINT64_C(0x0000000C0000000C)) << 14);
  result = (result & UINT64_C(0x0001000100010001)) |
          ((result & UINT64_C(0x0002000200020002)) << 7);
  return result;
}

uint64_t Uint82CrossWhite(const uint8_t data) {
  uint64_t result = static_cast<uint64_t>(data);
  result = (result & UINT64_C(0x000000000000000F)) |
          ((result & UINT64_C(0x00000000000000F0)) << 32);
  result = (result & UINT64_C(0x0000003000000003)) |
          ((result & UINT64_C(0x000000C00000000C)) << 16);
  result = (result & UINT64_C(0x0040001000040001)) |
          ((result & UINT64_C(0x0080002000080002)) << 8);
  return result;
}

uint64_t Uint82CrossBlack(const uint8_t data) {
  uint64_t result = static_cast<uint64_t>(data);
  result = ((result & UINT64_C(0x000000000000000F)) << 4) |
           ((result & UINT64_C(0x00000000000000F0)) << 28);
  result = ((result & UINT64_C(0x0000000300000030)) << 2) |
           ((result & UINT64_C(0x0000000C000000C0)) << 14);
  result = ((result & UINT64_C(0x0001000400100040)) << 1) |
           ((result & UINT64_C(0x0002000800200080)) << 7);
  return result;
}

uint8_t MaskShiftRow8(const uint64_t data, const int index) {
  return static_cast<uint8_t>(MaskShiftRow64(data, index));
}

uint64_t MaskShiftRow64(const uint64_t data, const int index) {
  return (data >> (index * 8)) & 0xFF;
}

// 周囲8方位のビットだけ1になっているビット列を返す
// 01001001
// 00101010
// 00011100
// 11111111
// 00011100
// 00101010
// 01001001
// 10001000
uint64_t GetBitMaskImpl(const Position position) {
  return bitmask_table[position];
}

uint64_t GetBitMaskImpl(const Position position) {
  std::pair<int, int> xy_pair = Pos2XY(position);
  uint64_t result = 0;
  const int shift_row = xy_pair.second * 8;
  result |= UINT64_C(0x00000000000000FF) << shift_row;
  result |= UINT64_C(0x0101010101010101) << xy_pair.first;
  const int cross_white_shift = (xy_pair.first - xy_pair.second) * 8;
  result |= (cross_white_shift > 0) ?
      UINT64_C(0x8040201008040201) >> cross_white_shift :
      UINT64_C(0x8040201008040201) << -cross_white_shift;
  const int cross_black_shift = ((7-xy_pair.first) - xy_pair.second) * 8;
  result |= (cross_black_shift > 0) ?
      UINT64_C(0x0102040810204080) >> cross_black_shift :
      UINT64_C(0x0102040810204080) << -cross_black_shift;
  return result;
}

uint64_t GetPutBits(const uint8_t row, const uint8_t column,
    const uint8_t cross_white, const uint8_t cross_black,
    const std::pair<int, int> xy_pair) {
  uint64_t result = 0;
  const int shift_row = xy_pair.second * 8;
  result |= static_cast<uint64_t>(row) << shift_row;
  result |= Uint82Column(column) << xy_pair.first;
  const int cross_white_index = xy_pair.first-xy_pair.second;
  if (cross_white_index > 0) {
    result |= Uint82CrossWhite(cross_white) << cross_white_index;
  } else {
    result |= Uint82CrossWhite(cross_white) << (-cross_white_index * 8);
  }
  const int cross_black_index = (7-xy_pair.first)-xy_pair.second;
  if (cross_black_index > 0) {
    result |= Uint82CrossBlack(cross_black) >> cross_black_index;
  } else {
    result |= Uint82CrossBlack(cross_black) << (-cross_black_index * 8);
  }
  return result;
}

// ビット列を転置する
uint64_t TransposeBits(uint64_t data) {
  data = (data & UINT64_C(0xAA55AA55AA55AA55))
      | ((data & UINT64_C(0x5500550055005500)) >> 7)
      | ((data & UINT64_C(0x00AA00AA00AA00AA)) << 7);
  data = (data & UINT64_C(0xCCCC3333CCCC3333))
      | ((data & UINT64_C(0x3333000033330000)) >> 14)
      | ((data & UINT64_C(0x0000CCCC0000CCCC)) << 14);
  data = (data & UINT64_C(0xF0F0F0F00F0F0F0F))
      | ((data & UINT64_C(0x0F0F0F0F00000000)) >> 28)
      | ((data & UINT64_C(0x00000000F0F0F0F0)) << 28);
  return data;
}

// ビット列を左右反転する
uint64_t FlipHorizonalBits(uint64_t data) {
  data = ((data & UINT64_C(0xAAAAAAAAAAAAAAAA)) >> 1)
       | ((data & UINT64_C(0x5555555555555555)) << 1);
  data = ((data & UINT64_C(0xCCCCCCCCCCCCCCCC)) >> 2)
       | ((data & UINT64_C(0x3333333333333333)) << 2);
  data = ((data & UINT64_C(0xF0F0F0F0F0F0F0F0)) >> 4)
       | ((data & UINT64_C(0x0F0F0F0F0F0F0F0F)) << 4);
  return data;
}

// TODO : 分割統治法を使うように変更する
uint64_t DistortBitsWhite1(uint64_t data) {
  return (data & UINT64_C(0x0101010101010101))
      | ((data & UINT64_C(0x0202020202020200)) >> 8)
      | ((data & UINT64_C(0x0404040404040000)) >> 16)
      | ((data & UINT64_C(0x0808080808000000)) >> 24)
      | ((data & UINT64_C(0x1010101000000000)) >> 32)
      | ((data & UINT64_C(0x2020200000000000)) >> 40)
      | ((data & UINT64_C(0x4040000000000000)) >> 48)
      | ((data & UINT64_C(0x8000000000000000)) >> 56);
}

//  | a| b| c| d| e| f| g| h|
// -+--+--+--+--+--+--+--+--+
// 1| 0| 1| 2| 3| 4| 5| 6| 7|
// 2| 8| 9|10|11|12|13|14|15|
// 3|16|17|18|19|20|21|22|23|
// 4|24|25|26|27|28|29|30|31|
// 5|32|33|34|35|36|37|38|39|
// 6|40|41|42|43|44|45|46|47|
// 7|48|49|50|51|52|53|54|55|
// 8|56|57|58|59|60|61|62|63|
//
// ||||||||||||||||||||||||||
// vvvvvvvvvvvvvvvvvvvvvvvvvv
//
//  | a| b| c| d| e| f| g| h|
// -+--+--+--+--+--+--+--+--+
// 1| 7|  |  |  |  |  |  |  |
// 2| 6|15|  |  |  |  |  |  |
// 3| 5|14|23|  |  |  |  |  |
// 4| 4|13|22|31|  |  |  |  |
// 5| 3|12|21|30|39|  |  |  |
// 6| 2|11|20|29|38|47|  |  |
// 7| 1|10|19|28|37|46|55|  |
// 8| 0| 9|18|27|36|45|54|63|
uint64_t DistortBitsWhite2(uint64_t data) {
// phase 1
  data = (data & UINT64_C(0x80C0E0F0F0F0F0F0))
      | ((data & UINT64_C(0x00000000080C0E0F)) << 32);
  data = (data & UINT64_C(0x88CCCCCCC0C0C0C0))
      | ((data & UINT64_C(0x2233303030300000)) << 16);
  data = (data & UINT64_C(0xAAAAA8A8A0A08080))
      | ((data & UINT64_C(0x0055545450504040)) << 8);
// phase 2
  data = (data & UINT64_C(0xFFFEFCF800000000))
      | ((data & UINT64_C(0x00000000F0E0C080)) >> 4);
  data = (data & UINT64_C(0xFFFE00000F0E0000))
      | ((data & UINT64_C(0x0000FCF800000C08)) >> 2);
  data = (data & UINT64_C(0xFF003F000F000300))
      | ((data & UINT64_C(0x00FE003E000E0002)) >> 1);
  return data;
//  return DistortBitsBlack1(FlipHorizonalBits(data));
}

uint64_t DistortBitsBlack1(uint64_t data) {
  return UndistortBitsWhite1(data);
}

// TODO : 高速な実装に書き換える
uint64_t DistortBitsBlack2(uint64_t data) {
// phase 1
  data = (data & UINT64_C(0xF0F0F0F0F0E0C080))
      | ((data & UINT64_C(0x0F0E0C0800000000)) >> 32);
  data = (data & UINT64_C(0xC0C0C0C0CCCCCC88))
      | ((data & UINT64_C(0x3030303033220000)) >> 16);
  data = (data & UINT64_C(0x8080A0A0A8A8AAAA))
      | ((data & UINT64_C(0x4040505054545500)) >> 8);
// phase 2
  data = (data & UINT64_C(0xF0F0F0F0F0E0C080))
      | ((data & UINT64_C(0x0F0E0C0800000000)) >> 32);
  data = (data & UINT64_C(0xC0C0C0C0CCCCCC88))
      | ((data & UINT64_C(0x3030303033220000)) >> 16);
  data = (data & UINT64_C(0x8080A0A0A8A8AAAA))
      | ((data & UINT64_C(0x4040505054545500)) >> 8);
//  return DistortBitsWhite1(FlipHorizonalBits(data));
}

// TODO : 分割統治法を使うように変更する
uint64_t UndistortBitsWhite1(const uint64_t data) {
  return (data & UINT64_C(0x0101010101010101))
      | ((data & UINT64_C(0x0002020202020202)) << 8)
      | ((data & UINT64_C(0x0000040404040404)) << 16)
      | ((data & UINT64_C(0x0000000808080808)) << 24)
      | ((data & UINT64_C(0x0000000010101010)) << 32)
      | ((data & UINT64_C(0x0000000000202020)) << 40)
      | ((data & UINT64_C(0x0000000000004040)) << 48)
      | ((data & UINT64_C(0x0000000000000080)) << 56);
}

// TODO : 高速な実装に書き換える
uint64_t UndistortBitsWhite2(const uint64_t data) {
  return TransposeBits(UndistortBitsWhite1(data));
}

uint64_t UndistortBitsBlack1(const uint64_t data) {
  return DistortBitsWhite1(data);
}

// TODO : 高速な実装に書き換える
uint64_t UndistortBitsBlack2(const uint64_t data) {
  return FlipHorizonalBits(UndistortBitsWhite1(data));
}


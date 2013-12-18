#include "GameState.hpp"

namespace othello {
namespace gamestate {

// public methods
LineState basic_GameState::operator[](const int index) const {
  return GetRow(index);
}

State basic_GameState::atPosition(const Position position) const {
  if (black_state_ & (UINT64_C(0x1) << position))
    return State::BLACK;
  else if (white_state_ & (UINT64_C(0x1) << position))
    return State::WHITE;
  else
    return State::NONE;
}

uint8_t basic_GameState::Column2Uint8(uint64_t data) {
  data = (data & UINT64_C(0x0001000100010001)) |
        ((data & UINT64_C(0x0100010001000100)) >> 7);
  data = (data & UINT64_C(0x0000000300000003)) |
        ((data & UINT64_C(0x0003000000030000)) >> 14);
  data = (data & UINT64_C(0x000000000000000F)) |
        ((data & UINT64_C(0x0000000F00000000)) >> 28);
  return static_cast<uint8_t>(data);
}

uint8_t basic_GameState::CrossWhite2Uint8(uint64_t data) {
  data = (data & UINT64_C(0x0040001000040001)) |
        ((data & UINT64_C(0x8000200008000200)) >> 8);
  data = (data & UINT64_C(0x0000003000000003)) |
        ((data & UINT64_C(0x00C00000000C0000)) >> 16);
  data = (data & UINT64_C(0x000000000000000F)) |
        ((data & UINT64_C(0x000000F000000000)) >> 32);
  return static_cast<uint8_t>(data);
}

uint8_t basic_GameState::CrossBlack2Uint8(uint64_t data) {
  data = ((data & UINT64_C(0x0002000800200080)) >> 1) |
         ((data & UINT64_C(0x0100040010004000)) >> 7);
  data = ((data & UINT64_C(0x0000000C000000C0)) >> 2) |
         ((data & UINT64_C(0x0003000000300000)) >> 14);
  data = ((data & UINT64_C(0x00000000000000F0)) >> 4) |
         ((data & UINT64_C(0x0000000F00000000)) >> 28);
  return static_cast<uint8_t>(data);
}

LineState basic_GameState::GetRow(const int index) const {
  return LineState(static_cast<uint8_t>(black_state_ >> (index * 8)),
                   static_cast<uint8_t>(white_state_ >> (index * 8)));
}

LineState basic_GameState::GetColumn(const int index) const {
  return LineState(Column2Uint8(black_state_ >> index),
                   Column2Uint8(white_state_ >> index));
}

LineState basic_GameState::GetCross(const int index,
                                    const Direction direction) const {
  const int shift_row = abs(index) * 8;
  if (direction == Direction::WHITE_LINE) {
    constexpr uint64_t mask = UINT64_C(0x8040201008040201);
    if (index > 0) {
      return LineState(
          CrossWhite2Uint8((black_state_ & (mask >> shift_row)) >> index),
          CrossWhite2Uint8((white_state_ & (mask >> shift_row)) >> index));
    } else {
      return LineState(
          CrossWhite2Uint8((black_state_ >> shift_row) & mask),
          CrossWhite2Uint8((white_state_ >> shift_row) & mask));
    }
  } else { // BLACK_LINE
    constexpr uint64_t mask = UINT64_C(0x0102040810204080);
    if (index > 0) {
      return LineState(
          CrossBlack2Uint8((black_state_ & (mask >> shift_row)) << index),
          CrossBlack2Uint8((white_state_ & (mask >> shift_row)) << index));
    } else {
      return LineState(CrossBlack2Uint8((black_state_ >> shift_row) & mask),
                       CrossBlack2Uint8((white_state_ >> shift_row) & mask));
    }
  }
}

uint64_t basic_GameState::Uint82Column(const uint8_t data) {
  uint64_t result = static_cast<uint64_t>(data);
  result = (result & UINT64_C(0x000000000000000F)) |
          ((result & UINT64_C(0x00000000000000F0)) << 28);
  result = (result & UINT64_C(0x0000000300000003)) |
          ((result & UINT64_C(0x0000000C0000000C)) << 14);
  result = (result & UINT64_C(0x0001000100010001)) |
          ((result & UINT64_C(0x0002000200020002)) << 7);
  return result;
}

uint64_t basic_GameState::Uint82CrossWhite(const uint8_t data) {
  uint64_t result = static_cast<uint64_t>(data);
  result = (result & UINT64_C(0x000000000000000F)) |
          ((result & UINT64_C(0x00000000000000F0)) << 32);
  result = (result & UINT64_C(0x0000003000000003)) |
          ((result & UINT64_C(0x000000C00000000C)) << 16);
  result = (result & UINT64_C(0x0040001000040001)) |
          ((result & UINT64_C(0x0080002000080002)) << 8);
  return result;
}

uint64_t basic_GameState::Uint82CrossBlack(const uint8_t data) {
  uint64_t result = static_cast<uint64_t>(data);
  result = ((result & UINT64_C(0x000000000000000F)) << 4) |
           ((result & UINT64_C(0x00000000000000F0)) << 28);
  result = ((result & UINT64_C(0x0000000300000030)) << 2) |
           ((result & UINT64_C(0x0000000C000000C0)) << 14);
  result = ((result & UINT64_C(0x0001000400100040)) << 1) |
           ((result & UINT64_C(0x0002000800200080)) << 7);
  return result;
}

basic_GameState& basic_GameState::SetRow(const int index, const LineState line_state) {
  black_state_ &= UINT64_C(0x00000000000000FF) << (index * 8);
  black_state_ |= static_cast<uint64_t>(line_state.GetBlack()) << (index * 8);
  white_state_ &= UINT64_C(0x00000000000000FF) << (index * 8);
  white_state_ |= static_cast<uint64_t>(line_state.GetWhite()) << (index * 8);
  return *this;
}

basic_GameState& basic_GameState::SetColumn(const int index, const LineState line_state) {
  black_state_ &= UINT64_C(0xFEFEFEFEFEFEFEFE) << index;
  black_state_ |= Uint82Column(line_state.GetBlack()) << index;
  white_state_ &= UINT64_C(0xFEFEFEFEFEFEFEFE) << index;
  white_state_ |= Uint82Column(line_state.GetWhite()) << index;
  return *this;
}

basic_GameState& basic_GameState::SetCross(const int index,
    const Direction direction, const LineState line_state) {
  const int shift_row = abs(index) * 8;
  uint64_t black_bits;
  uint64_t white_bits;
  uint64_t mask;
  if (direction == Direction::WHITE_LINE) {
    mask = UINT64_C(0x7FBFDFEFF7FBFDFE);
    black_bits = Uint82CrossWhite(line_state.GetBlack());
    white_bits = Uint82CrossWhite(line_state.GetWhite());
  } else {
    mask = UINT64_C(0xFEFDFBF7EFDFBF7F);
    black_bits = Uint82CrossBlack(line_state.GetBlack());
    white_bits = Uint82CrossBlack(line_state.GetWhite());
  }
  if (index > 0) {
    black_state_ &= mask >> shift_row;
    black_state_ |= black_bits << index;
    white_state_ &= mask >> shift_row;
    white_state_ |= white_bits << index;
  } else {
    black_state_ &= mask << shift_row;
    black_state_ |= black_bits << shift_row;
    white_state_ &= mask << shift_row;
    white_state_ |= white_bits << shift_row;
  }
  return *this;
}

bool basic_GameState::isPuttable(const Position position,
                                 const State state) const {
  std::pair<int, int> xy_pair = Pos2XY(position);
  if(GetRow(xy_pair.second).isPuttable(xy_pair.first, state, 8))
    return true;
  if(GetColumn(xy_pair.first).isPuttable(xy_pair.second, state, 8))
    return true;
  const int cross_white_index = xy_pair.first-xy_pair.second;
  const int cross_white_line_length = 8 - abs(cross_white_index);
  const LineState cross_white = GetCross(cross_white_index,
                                         Direction::WHITE_LINE);
  if(cross_white.isPuttable(xy_pair.second - std::max(-cross_white_index, 0),
      state, cross_white_line_length))
    return true;
  const int cross_black_index = (7-xy_pair.first)-xy_pair.second;
  const int cross_black_line_length = 8 - abs(cross_black_index);
  const LineState cross_black = GetCross(cross_black_index,
                                         Direction::BLACK_LINE);
  if(cross_black.isPuttable(xy_pair.second - std::max(-cross_black_index, 0),
      state, cross_black_line_length))
    return true;
  return false;
}

uint64_t basic_GameState::LinePuttableBits(const uint64_t black_state,
    const uint64_t white_state, const int index, const int line_length,
    const State state) {
  return static_cast<uint64_t>(
      LineState(MaskShiftRow8(black_state, index),
                MaskShiftRow8(white_state, index))
          .GetPuttable(state, line_length)) << (index * 8);
}

// white1 : defined
// white2 := white1 . transpose
// black1 : defined
// black2 := white1 . flip
uint64_t basic_GameState::GetPuttable(const State state) const {
  uint64_t columns_data_black = TransposeBits(black_state_);
  uint64_t columns_data_white = TransposeBits(white_state_);
  uint64_t crosses_white_data1_black = DistortBitsWhite1(black_state_);
  uint64_t crosses_white_data1_white = DistortBitsWhite1(white_state_);
  uint64_t crosses_white_data2_black = DistortBitsWhite1(columns_data_black);
  uint64_t crosses_white_data2_white = DistortBitsWhite1(columns_data_white);
  uint64_t crosses_black_data1_black = DistortBitsBlack1(black_state_);
  uint64_t crosses_black_data1_white = DistortBitsBlack1(white_state_);
  uint64_t crosses_black_data2_black = DistortBitsBlack2(black_state_);
  uint64_t crosses_black_data2_white = DistortBitsBlack2(white_state_);
  uint64_t rows_puttables = 0;
  uint64_t columns_puttables = 0;
  uint64_t crosses_white1_puttables = 0;
  uint64_t crosses_white2_puttables = 0;
  uint64_t crosses_black1_puttables = 0;
  uint64_t crosses_black2_puttables = 0;
  for (int i = 0; i < 8; ++i) {
    rows_puttables |=
        LinePuttableBits(black_state_, white_state_, i, 8, state);
    columns_puttables |=
        LinePuttableBits(columns_data_black, columns_data_white, i, 8, state);
    crosses_white1_puttables |= LinePuttableBits(
        crosses_white_data1_black,
        crosses_white_data1_white, i, 8-i, state);
    crosses_white2_puttables |= LinePuttableBits(
        crosses_white_data2_black,
        crosses_white_data2_white, i, 8-i, state);
    crosses_black1_puttables |= LinePuttableBits(
        crosses_black_data1_black,
        crosses_black_data1_white, i, i+1, state);
    crosses_black2_puttables |= LinePuttableBits(
        crosses_black_data2_black,
        crosses_black_data2_white, i, i+1, state);
  }
  return rows_puttables
      | TransposeBits(columns_puttables)
      | UndistortBitsWhite1(crosses_white1_puttables)
      | UndistortBitsWhite2(crosses_white2_puttables)
      | UndistortBitsBlack1(crosses_black1_puttables)
      | UndistortBitsBlack2(crosses_black2_puttables);
}

std::string basic_GameState::ToString() const {
  std::string str = "";
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      switch (this->atPosition(i*8+j)) {
       case State::NONE:
        str += '.';
        break;
       case State::BLACK:
        str += 'x';
        break;
       case State::WHITE:
        str += 'o';
        break;
      }
    }
    str += '\n';
  }
  return str;
}

std::string basic_GameState::ToString(const State state) const {
  uint64_t puttable_bits = GetPuttable(state);
  std::string str = "";
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      switch (this->atPosition(i*8+j)) {
       case State::NONE:
        if (puttable_bits >> (i*8+j) & 1)
          str += '*';
        else
          str += '.';
        break;
       case State::BLACK:
        str += 'x';
        break;
       case State::WHITE:
        str += 'o';
        break;
      }
    }
    str += '\n';
  }
  return str;
}

// private methods
uint64_t basic_GameState::GetPuttableSimple(const State state) const {
  uint64_t data = 0;
  for (Position i = 0; i < 64; ++i)
    data |= static_cast<uint64_t>(isPuttable(i, state)) << i;
  return data;
}

uint8_t basic_GameState::MaskShiftRow8(const uint64_t data,
                                       const int index) {
  return static_cast<uint8_t>(MaskShiftRow64(data, index));
}

uint64_t basic_GameState::MaskShiftRow64(const uint64_t data,
                                        const int index) {
  return (data >> (index * 8)) & 0xFF;
}

LineState basic_GameState::PutOrIdCross(LineState line, const int index,
    const int line_length, const std::pair<int, int> xy_pair,
    const State state) {
  if (line.isPuttable(xy_pair.second - std::max(-index, 0), state, line_length))
    line.Put(xy_pair.second - std::max(-index, 0), state, line_length);
  return line;
}

LineState basic_GameState::PutOrIdWhite(const std::pair<int, int> xy_pair,
                                        const State state) {
  const int index = xy_pair.first-xy_pair.second;
  const int line_length = 8 - abs(index);
  LineState cross_white = GetCross(index, Direction::WHITE_LINE);
  return PutOrIdCross(cross_white, index, line_length, xy_pair, state);
}

LineState basic_GameState::PutOrIdBlack(const std::pair<int, int> xy_pair,
                                        const State state) {
  const int index = (7-xy_pair.first)-xy_pair.second;
  const int line_length = 8 - abs(index);
  LineState cross_black = GetCross(index, Direction::BLACK_LINE);
  return PutOrIdCross(cross_black, index, line_length, xy_pair, state);
}

basic_GameState& basic_GameState::Put(const Position position, const State state) {
  std::pair<int, int> xy_pair = Pos2XY(position);
  LineState row = GetRow(xy_pair.second);
  if (row.isPuttable(xy_pair.first, state, 8))
    row.Put(xy_pair.first, state, 8);
  LineState column = GetColumn(xy_pair.first);
  if (column.isPuttable(xy_pair.second, state, 8))
    column.Put(xy_pair.second, state, 8);
  LineState cross_white = PutOrIdWhite(xy_pair, state);
  LineState cross_black = PutOrIdBlack(xy_pair, state);
  black_state_ &= ~GetBitMask(position);
  black_state_ |= GetPutBits(row.GetBlack(), column.GetBlack(),
      cross_white.GetBlack(), cross_black.GetBlack(), xy_pair);
  white_state_ &= ~GetBitMask(position);
  white_state_ |= GetPutBits(row.GetWhite(), column.GetWhite(),
      cross_white.GetWhite(), cross_black.GetWhite(), xy_pair);
  return *this;
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
uint64_t basic_GameState::GetBitMask(const Position position) {
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

uint64_t basic_GameState::GetPutBits(const uint8_t row, const uint8_t column,
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

uint64_t basic_GameState::TransposeBits(uint64_t data) {
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

uint64_t basic_GameState::FlipHorizonalBits(uint64_t data) {
  data = ((data & UINT64_C(0xAAAAAAAAAAAAAAAA)) >> 1)
       | ((data & UINT64_C(0x5555555555555555)) << 1);
  data = ((data & UINT64_C(0xCCCCCCCCCCCCCCCC)) >> 2)
       | ((data & UINT64_C(0x3333333333333333)) << 2);
  data = ((data & UINT64_C(0xF0F0F0F0F0F0F0F0)) >> 4)
       | ((data & UINT64_C(0x0F0F0F0F0F0F0F0F)) << 4);
  return data;
}

// TODO : 分割統治法を使うように変更
uint64_t basic_GameState::DistortBitsWhite1(const uint64_t data) {
  return (data & UINT64_C(0x0101010101010101))
      | ((data & UINT64_C(0x0202020202020200)) >> 8)
      | ((data & UINT64_C(0x0404040404040000)) >> 16)
      | ((data & UINT64_C(0x0808080808000000)) >> 24)
      | ((data & UINT64_C(0x1010101000000000)) >> 32)
      | ((data & UINT64_C(0x2020200000000000)) >> 40)
      | ((data & UINT64_C(0x4040000000000000)) >> 48)
      | ((data & UINT64_C(0x8000000000000000)) >> 56);
}

// TODO : 高速な実装に書き換える
uint64_t basic_GameState::DistortBitsWhite2(const uint64_t data) {
  return DistortBitsBlack1(FlipHorizonalBits(data));
}

uint64_t basic_GameState::DistortBitsBlack1(const uint64_t data) {
  return UndistortBitsWhite1(data);
}

// TODO : 高速な実装に書き換える
uint64_t basic_GameState::DistortBitsBlack2(const uint64_t data) {
  return DistortBitsWhite1(FlipHorizonalBits(data));
}

// TODO : 分割統治法を使うように変更
uint64_t basic_GameState::UndistortBitsWhite1(const uint64_t data) {
  return (data & UINT64_C(0x0101010101010101))
      | ((data & UINT64_C(0x0002020202020202)) << 8)
      | ((data & UINT64_C(0x0000040404040404)) << 16)
      | ((data & UINT64_C(0x0000000808080808)) << 24)
      | ((data & UINT64_C(0x0000000010101010)) << 32)
      | ((data & UINT64_C(0x0000000000202020)) << 40)
      | ((data & UINT64_C(0x0000000000004040)) << 48)
      | ((data & UINT64_C(0x0000000000000080)) << 56);
}

uint64_t basic_GameState::UndistortBitsWhite2(const uint64_t data) {
  return TransposeBits(UndistortBitsWhite1(data));
}

uint64_t basic_GameState::UndistortBitsBlack1(const uint64_t data) {
  return DistortBitsWhite1(data);
}

uint64_t basic_GameState::UndistortBitsBlack2(const uint64_t data) {
  return FlipHorizonalBits(UndistortBitsWhite1(data));
}


} // namespace gamestate
} // namespace othello


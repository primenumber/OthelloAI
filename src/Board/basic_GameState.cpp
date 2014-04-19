#include "GameState.hpp"
#include "Lib/BitwiseOperations.hpp"

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

LineState basic_GameState::GetRow(const int index) const {
  return LineState(static_cast<uint8_t>(black_state_ >> (index * 8)),
                   static_cast<uint8_t>(white_state_ >> (index * 8)));
}

LineState basic_GameState::GetColumn(const int index) const {
  using bitwise::Column2Uint8;
  return LineState(Column2Uint8(black_state_ >> index),
                   Column2Uint8(white_state_ >> index));
}

LineState basic_GameState::GetCross(const int index,
                                    const Direction direction) const {
  using bitwise::CrossWhite2Uint8;
  using bitwise::CrossBlack2Uint8;
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

basic_GameState& basic_GameState::SetRow(const int index,
                                         const LineState line_state) {
  black_state_ &= UINT64_C(0x00000000000000FF) << (index * 8);
  black_state_ |= static_cast<uint64_t>(line_state.GetBlack()) << (index * 8);
  white_state_ &= UINT64_C(0x00000000000000FF) << (index * 8);
  white_state_ |= static_cast<uint64_t>(line_state.GetWhite()) << (index * 8);
  return *this;
}

basic_GameState& basic_GameState::SetColumn(const int index,
                                            const LineState line_state) {
  using bitwise::Uint82Column;
  black_state_ &= UINT64_C(0xFEFEFEFEFEFEFEFE) << index;
  black_state_ |= Uint82Column(line_state.GetBlack()) << index;
  white_state_ &= UINT64_C(0xFEFEFEFEFEFEFEFE) << index;
  white_state_ |= Uint82Column(line_state.GetWhite()) << index;
  return *this;
}

basic_GameState& basic_GameState::SetCross(const int index,
    const Direction direction, const LineState line_state) {
  using bitwise::Uint82CrossWhite;
  using bitwise::Uint82CrossBlack;
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

//uint64_t basic_GameState::LinePuttableBits(const uint64_t black_state,
//    const uint64_t white_state, const int index, const int line_length,
//    const State state) {
//  using bitwise::MaskShiftRow8;
//  return static_cast<uint64_t>(
//      LineState(MaskShiftRow8(black_state, index),
//                MaskShiftRow8(white_state, index))
//          .GetPuttable(state, line_length)) << (index * 8);
//}

// white1 : defined
// white2 := white1 . transpose
// black1 : defined
// black2 := white1 . flip
uint64_t basic_GameState::GetPuttable(const State state) const {
  using bitwise::TransposeBits;
  using bitwise::DistortBitsWhite1;
  using bitwise::DistortBitsWhite2;
  using bitwise::DistortBitsBlack1;
  using bitwise::DistortBitsBlack2;
  using bitwise::UndistortBitsWhite1;
  using bitwise::UndistortBitsWhite2;
  using bitwise::UndistortBitsBlack1;
  using bitwise::UndistortBitsBlack2;
  using bitwise::LinePuttableBits;
  uint64_t columns_data_black = TransposeBits(black_state_);
  uint64_t columns_data_white = TransposeBits(white_state_);
  uint64_t crosses_white_data1_black = DistortBitsWhite1(black_state_);
  uint64_t crosses_white_data1_white = DistortBitsWhite1(white_state_);
  uint64_t crosses_white_data2_black = DistortBitsWhite2(black_state_);
  uint64_t crosses_white_data2_white = DistortBitsWhite2(white_state_);
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
        crosses_white_data2_white, i, i+1, state);
    crosses_black1_puttables |= LinePuttableBits(
        crosses_black_data1_black,
        crosses_black_data1_white, i, i+1, state);
    crosses_black2_puttables |= LinePuttableBits(
        crosses_black_data2_black,
        crosses_black_data2_white, i, 8-i, state);
  }
  return rows_puttables
      | TransposeBits(columns_puttables)
      | UndistortBitsWhite1(crosses_white1_puttables)
      | UndistortBitsWhite2(crosses_white2_puttables)
      | UndistortBitsBlack1(crosses_black1_puttables)
      | UndistortBitsBlack2(crosses_black2_puttables);
}

int basic_GameState::CountBlack() const {
  using bitwise::CountBits;  
  return CountBits(black_state_);
}

int basic_GameState::CountWhite() const {
  using bitwise::CountBits;  
  return CountBits(white_state_);
}

basic_GameState& basic_GameState::Put(const Position position,
                                      const State state) {
  using bitwise::GetPutBits;
  using bitwise::GetBitMask;
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

} // namespace gamestate
} // namespace othello


#include "GameState.hpp"

namespace othello {
namespace gamestate {

LineState::AllTableType<uint8_t> LineState::puttable_table;
LineState::AllTableType<std::array<LineState, 8>> LineState::put_table;

LineState::LineState(int base3) : black_state_(0), white_state_(0) {
  for (int i = 0; i < 8; ++i) {
    int r = base3 % 3;
    if (r == 1)
      black_state_ |= 1 << i;
    else if (r == 2)
      white_state_ |= 1 << i;
    base3 /= 3;
  }
}

State LineState::operator[](const int index) const {
  if (black_state_ & (1 << index))
    return State::BLACK;
  else if (white_state_ & (1 << index))
    return State::WHITE;
  else
    return State::NONE;
}

bool LineState::isPuttable(const int index, const State state,
                           const int line_length) const {
  if ((*this)[index] != State::NONE) return false;
  std::array<uint8_t, 2>& puttable_pair =
      puttable_table[line_length - 1][black_state_][white_state_];
  if (state == State::BLACK)
    return (puttable_pair[0] >> index) & 1;
  else
    return (puttable_pair[1] >> index) & 1;
}

bool LineState::isPuttableImpl(const int index, const State state,
                               const int line_length) const {
  if ((*this)[index] != State::NONE) return false;
  bool flag = false;
  State state_inv = InvertState(state);
  for (int i = index - 1; i >= 0; --i) {
    if ((*this)[i] == state) {
      if (flag) {
        return true;
      } else {
        break;
      }
    } else if ((*this)[i] == state_inv) {
      flag = true;
    } else {
      break;
    }
  }
  flag = false;
  for (int i = index + 1; i < line_length; ++i) {
    if ((*this)[i] == state) {
      if (flag) {
        return true;
      } else {
        break;
      }
    } else if ((*this)[i] == state_inv) {
      flag = true;
    } else {
      break;
    }
  }
  return false;
}

uint8_t LineState::GetPuttable(const State state, const int line_length) const {
  if (state == State::BLACK)
    return puttable_table[line_length - 1][black_state_][white_state_][0];
  else
    return puttable_table[line_length - 1][black_state_][white_state_][1];
}

uint8_t LineState::GetPuttableImpl(const State state,
                                   const int line_length) const {
  uint8_t data = 0;
  for (int i = 0; i < line_length; ++i)
    data |= isPuttableImpl(i, state, line_length) << i;
  return data;
}

LineState& LineState::Put(const int index, const State state,
                          const int line_length) {
  auto& put_pair = put_table[line_length - 1][black_state_][white_state_];
  if (state == State::BLACK) {
    return *this = put_pair[0][index];
  } else {
    return *this = put_pair[1][index];
  }
}

LineState& LineState::PutImpl(const int index, const State state,
                              const int line_length) {
  if ((*this)[index] != State::NONE) {
    std::cerr << "unputtable" << std::endl;
    throw;
  }
  bool flag = false;
  State state_inv = InvertState(state);
  for (int i = index - 1; i >= 0; --i) {
    if ((*this)[i] == state) {
      if (flag) {
        uint8_t mask = ~(0xFF << index) & (0xFF << (i+1));
        if (state == State::BLACK) {
          black_state_ |= mask;
          white_state_ &= ~mask;
        } else {
          black_state_ &= ~mask;
          white_state_ |= mask;
        }
      }
      break;
    } else if ((*this)[i] == state_inv) {
      flag = true;
    } else {
      break;
    }
  }
  flag = false;
  for (int i = index + 1; i < line_length; ++i) {
    if ((*this)[i] == state) {
      if (flag) {
        uint8_t mask = ~static_cast<uint8_t>(0xFF << i)
            & static_cast<uint8_t>(0xFF << (index+1));
        if (state == State::BLACK) {
          black_state_ |= mask;
          white_state_ &= ~mask;
        } else {
          black_state_ &= ~mask;
          white_state_ |= mask;
        }
      }
      break;
    } else if ((*this)[i] == state_inv) {
      flag = true;
    } else {
      break;
    }
  }
  if (state == State::BLACK) {
    black_state_ |= 1 << index;
  } else {
    white_state_ |= 1 << index;
  }
  return *this;
}

std::string LineState::ToString(int line_length) const {
  std::string str;
  for (int i = 0; i < line_length; ++i) {
    switch ((*this)[i]) {
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
  return str;
}

void LineState::Init() {
  for (int i = 1; i <= 8; ++i)
    InitImpl(i);
}

void LineState::InitImpl(int length) {
  for (int i = 0; i < Pow3(length); ++i) {
    std::pair<uint8_t, uint8_t> uint8_pair = Base3ToUint8Pair(i);
    std::array<uint8_t, 2>& puttable_pair =
        puttable_table[length - 1][uint8_pair.first][uint8_pair.second];
    LineState tmp(i);
    puttable_pair[0] = tmp.GetPuttableImpl(State::BLACK, length);
    puttable_pair[1] = tmp.GetPuttableImpl(State::WHITE, length);
    for (int k = 0; k < length; ++k) {
      std::array<std::array<LineState, 8>, 2>& put_pair = 
          put_table[length - 1][uint8_pair.first][uint8_pair.second];
      if ((puttable_pair[0] >> k) & 1)
        put_pair[0][k] = LineState(tmp).PutImpl(k, State::BLACK, length);
      if ((puttable_pair[1] >> k) & 1)
        put_pair[1][k] = LineState(tmp).PutImpl(k, State::WHITE, length);
    }
  }
}

} // namespace gamestate
} // namespace othello


#include "GameState.hpp"

namespace othello {
namespace gamestate {

LineState::LineState(int base3) {
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
  uint8_t data = 0;
  for (int i = 0; i < line_length; ++i)
    data |= isPuttable(i, state, line_length) << i;
  return data;
}

LineState& LineState::Put(const int index, const State state,
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

} // namespace gamestate
} // namespace othello


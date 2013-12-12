#include "LineStateTable.hpp"

constexpr int pow3(const int index) {
  return index ? 3 * pow3(index - 1) : 1;
}

static void Init() {
  for (int i = 1; i <= 8; ++i) {
    puttable_table_black_[i-1] = std::vector<uint8_t>(pow3(i));
    puttable_table_white_[i-1] = std::vector<uint8_t>(pow3(i));
    put_table_black_[i-1] = std::vector<std::vector<LineState>>(pow3(i));
    put_table_white_[i-1] = std::vector<std::vector<LineState>>(pow3(i));
    for (int j = 0; j < pow3(i); ++j) {
      LineState line_state(j);
      puttable_table_black_[i-1][j] = line_state.GetPuttable(State::BLACK, i);
      puttable_table_white_[i-1][j] = line_state.GetPuttable(State::WHITE, i);
      put_table_black_[i-1][j] = std::vector<LineState>(i);
      put_table_white_[i-1][j] = std::vector<LineState>(i);
      for (int k = 0; k < i; ++k) {
        LineState put_black_state(line_state);
        LineState put_white_state(line_state);
        put_black_state.Put(k, State::BLACK, i);
        put_white_state.Put(k, State::WHITE, i);
        put_table_black_[i-1][j][k] = put_black_state;
        put_table_white_[i-1][j][k] = put_white_state;
      }
    }
  }
}

int BW2Base3(uint8_t black_state, uint8_t white_state) {
}

static std::pair<uint8_t, uint8_t> GetPuttable(const uint8_t black_state,
    const uint8_t white_state) const {
  
}

static std::pair<uint8_t, uint8_t> GetPut(const uint8_t black_state,
    const uint8_t white_state, const int index, const State state) const {
  
}

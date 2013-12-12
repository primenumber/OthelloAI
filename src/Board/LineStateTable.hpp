#pragma once
#include <array>
#include <vector>
#include "GameState.hpp"

class LineStateTable {
 public:
  static void Init();
  static std::pair<uint8_t, uint8_t> GetPuttable(const uint8_t black_state,
      const uint8_t white_state) const;
  static std::pair<uint8_t, uint8_t> GetPut(const uint8_t black_state, const uint8_t white_state,
      const int index, const State state) const;
 private:
  static std::array<std::vector<uint8_t>, 8> puttable_table_black_;
  static std::array<std::vector<uint8_t>, 8> puttable_table_white_;
  static std::array<std::vector<std::vector<LineState>>, 8> put_table_black_;
  static std::array<std::vector<std::vector<LineState>>, 8> put_table_white_;
}

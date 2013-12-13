#pragma once
#include <cstdint>
#include <functional>
#include <boost/optional.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <utility>

namespace othello {
namespace gamestate {

enum class State {
  NONE,
  BLACK,
  WHITE
};

State InvertState(const State state);

// White Line  Black Line
//  \.......    ......./
//  .\......    ....../.
//  ..\.....    ...../..
//  ...\....    ..../...
//  ....\...    .../....
//  .....\..    ../.....
//  ......\.    ./......
//  .......\    /.......
//
enum class Direction {
  WHITE_LINE,
  BLACK_LINE
};

typedef int Position;

std::pair<int, int> Pos2XY(const Position position);
int Pos2X(const Position position);
int Pos2Y(const Position position);

class LineState {
 public:
  LineState(const uint8_t black_state, const uint8_t white_state)
      : black_state_(black_state), white_state_(white_state) {}
  // base3 => LineState (NONE:0 BLACK:1 WHITE:2)
  LineState(int base3);
  State operator[](const int index) const;
  bool isPuttable(const int index, const State state, const int line_length) const;
  uint8_t GetPuttable(const State state, const int line_length) const;
  LineState& Put(const int index, const State state, const int line_length);
  uint8_t GetBlack() const { return black_state_; }
  uint8_t GetWhite() const { return white_state_; }
  std::string ToString(int line_length) const;
 private:
  uint8_t black_state_;
  uint8_t white_state_;
};

// Position Index
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
class basic_GameState {
 public:
  basic_GameState()
      : black_state_(UINT64_C(0x0000000810000000)),
        white_state_(UINT64_C(0x0000001008000000)) {}
  basic_GameState(const uint64_t black_state, const uint64_t white_state)
      : black_state_(black_state), white_state_(white_state) {}
  basic_GameState(const basic_GameState&) = default;
  basic_GameState(basic_GameState&&) = default;
  basic_GameState& operator=(const basic_GameState& lhs) = default;
  basic_GameState& operator=(basic_GameState&& lhs) = default;
  virtual ~basic_GameState() = default;
  LineState operator[](const int index) const;
  State atPosition(const Position position) const;
  LineState GetRow(const int index) const;
  LineState GetColumn(const int index) const;
  LineState GetCross(const int index, const Direction direction) const;
  basic_GameState& SetRow(const int index, const LineState line_state);
  basic_GameState& SetColumn(const int index, const LineState line_state);
  basic_GameState& SetCross(const int index, const Direction direction,
                					  const LineState line_state);
  bool isPuttable(const Position position, const State state) const;
  uint64_t GetPuttable(const State state) const;
//  std::vector<Position> GetPuttablePosition(State state) const;
//  std::vector<GameState> GetPuttableGameState(State state) const;
  basic_GameState& Put(const Position position, const State state);
  std::string ToString() const;
 private:
  uint64_t black_state_;
  uint64_t white_state_;
  LineState PutOrIdWhite(const std::pair<int, int> xy_pair,
                         const State state);
  LineState PutOrIdBlack(const std::pair<int, int> xy_pair,
                         const State state);
  LineState PutOrIdCross(LineState line, const int index,
      const int line_length, const std::pair<int, int> xy_pair,
      const State state);
  uint64_t GetBitMask(const Position position) const;
  uint64_t GetPutBits(const uint8_t row, const uint8_t column,
    const uint8_t cross_white, const uint8_t cross_black,
    const std::pair<int, int> xy_pair) const;
  uint8_t Column2Uint8(uint64_t data) const;
  uint8_t CrossWhite2Uint8(uint64_t data) const;
  uint8_t CrossBlack2Uint8(uint64_t data) const;
  uint64_t Uint82Column(const uint8_t data) const;
  uint64_t Uint82CrossWhite(const uint8_t data) const;
  uint64_t Uint82CrossBlack(const uint8_t data) const;
};

class GameState : private basic_GameState {
 public:
  constexpr static int kValueMax = 1000000;
  GameState() : basic_GameState(), state_(State::BLACK), recent_position_() {};
  GameState(const uint64_t black_state, const uint64_t white_state, State state,
            const Position recent_position)
      : basic_GameState(black_state, white_state), state_(state),
        recent_position_(recent_position) {}
  GameState(const GameState&) = default;
  GameState(GameState&&) = default;
  GameState& operator=(const GameState& lhs) = default;
  GameState& operator=(GameState&& lhs) = default;
  State GetState() const { return state_; }
  bool isPuttable(const Position position) const {
    return basic_GameState::isPuttable(position, state_);
  }
  uint64_t GetPuttable() const { return basic_GameState::GetPuttable(state_); }
//  std::vector<Position> GetPuttablePosition();
//  std::vector<GameState> GetPuttableGameState();
  GameState& Put(const Position position);
  GameState& Pass();
//  GameState PutCopy(Position position) const;
  std::vector<GameState> NextGameStates() const;
  int NegaMax(const int depth,
              const std::function<int (const GameState&)> value_func) const;
  int AlphaBeta(const int depth,
                const std::function<int (const GameState&)> value_func,
                int alpha, const int beta) const;
  boost::optional<Position> OptimalPut(const int depth,
      const std::function<int (const GameState&)> value_func) const;
  using basic_GameState::ToString;
  using basic_GameState::operator[];
  using basic_GameState::atPosition;
 private:
  State state_;
  boost::optional<Position> recent_position_;
};

} // namespace gamestate
} // namespace othello


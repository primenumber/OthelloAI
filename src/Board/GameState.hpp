#pragma once
#include <cstdint>
#include <array>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <boost/optional.hpp>

namespace othello {
namespace gamestate {

enum class State {
  NONE,
  BLACK,
  WHITE
};

State InvertState(const State state);
bool IsBlack(const State state);
bool IsWhite(const State state);

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
std::string ToString(const boost::optional<Position>& opt_position);

constexpr int Pow3(const int index) {
  return index ? Pow3(index - 1) * 3 : 1;
}
std::pair<uint8_t, uint8_t> Base3ToUint8Pair(int base3);

class LineState {
 public:
  template <typename T>
  using StateTableType = typename std::array<std::array<T, 256>, 256>;
  template <typename T>
  using AllTableType = typename std::array<StateTableType<std::array<T, 2>>, 8>;
  LineState() = default;
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
  static void Init();
 private:
  uint8_t black_state_;
  uint8_t white_state_;
  static AllTableType<uint8_t> puttable_table;
  static AllTableType<std::array<LineState, 8>> put_table;
  static void InitImpl(int length);
  bool isPuttableImpl(const int index, const State state, const int line_length) const;
  uint8_t GetPuttableImpl(const State state, const int line_length) const;
  LineState& PutImpl(const int index, const State state, const int line_length);
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
  uint64_t GetBlack() const { return black_state_; }
  uint64_t GetWhite() const { return white_state_; }
  int CountBlack() const;
  int CountWhite() const;
  std::pair<int, int> CountStones() const {
    return make_pair(CountBlack(), CountWhite());
  }
  basic_GameState& Put(const Position position, const State state);
  std::string ToString() const;
  std::string ToString(const State state) const;
 private:
  uint64_t black_state_;
  uint64_t white_state_;
  uint64_t GetPuttableSimple(const State state) const;
  LineState PutOrIdWhite(const std::pair<int, int> xy_pair,
                         const State state);
  LineState PutOrIdBlack(const std::pair<int, int> xy_pair,
                         const State state);
  LineState PutOrIdCross(LineState line, const int index,
      const int line_length, const std::pair<int, int> xy_pair,
      const State state);
//  static uint8_t MaskShiftRow8(const uint64_t data, const int index);
//  static uint64_t MaskShiftRow64(const uint64_t data, const int index);
//  static uint64_t GetBitMask(const Position position);
//  static uint64_t GetPutBits(const uint8_t row, const uint8_t column,
//    const uint8_t cross_white, const uint8_t cross_black,
//    const std::pair<int, int> xy_pair);
//  static uint8_t Column2Uint8(uint64_t data);
//  static uint8_t CrossWhite2Uint8(uint64_t data);
//  static uint8_t CrossBlack2Uint8(uint64_t data);
//  static uint64_t Uint82Column(const uint8_t data);
//  static uint64_t Uint82CrossWhite(const uint8_t data);
//  static uint64_t Uint82CrossBlack(const uint8_t data);
//  static uint64_t TransposeBits(uint64_t data);
//  static uint64_t FlipHorizonalBits(uint64_t data);
//  static uint64_t DistortBitsWhite1(uint64_t data);
//  static uint64_t DistortBitsWhite2(uint64_t data);
//  static uint64_t DistortBitsBlack1(uint64_t data);
//  static uint64_t DistortBitsBlack2(uint64_t data);
//  static uint64_t UndistortBitsWhite1(uint64_t data);
//  static uint64_t UndistortBitsWhite2(uint64_t data);
//  static uint64_t UndistortBitsBlack1(uint64_t data);
//  static uint64_t UndistortBitsBlack2(uint64_t data);
//  static uint64_t LinePuttableBits(const uint64_t black_state,
//      const uint64_t white_state, const int index, const int line_length,
//      const State state);
};

class GameState : public basic_GameState {
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
              const std::function<int (const GameState&)>& value_func) const;
  int AlphaBeta(const int depth,
                const std::function<int (const GameState&)>& value_func,
                int alpha, const int beta) const;
  boost::optional<Position> GetOptimalPut(const int depth,
      const std::function<int (const GameState&)>& value_func) const;
  boost::optional<Position> GetRecentPut() const { return recent_position_; }
  bool IsPassed() const { return !static_cast<bool>(recent_position_); }
 protected:
  State state_;
  boost::optional<Position> recent_position_;
};

class GameTree : private GameState {
 public:
  using GameState::GetBlack;
  using GameState::GetWhite;
  using GameState::ToString;
  using GameState::operator[];
  using GameState::atPosition;
  using GameState::GetState;
  using GameState::GetRecentPut;
  GameTree() = default;
  GameTree(const GameState& that) : GameState(that) {};
  GameTree(GameState&& that) : GameState(std::move(that)) {};
  GameTree(const GameTree& that) = default;
  GameTree(GameTree&& that) = default;
  std::unique_ptr<GameTree> Put(const Position position);
  std::unique_ptr<GameTree> Pass();
  std::vector<std::unique_ptr<GameTree>>& GetChildren();
  int SearchTree(const int tree_depth, const int search_depth,
      const std::function<int (const GameState&)>& value_func);
  int GetValue() const { return value_; }
 protected:
  std::vector<std::unique_ptr<GameTree>> children_;
  int value_;
  void MakeChildren();
  int SearchTreeImpl(const int tree_depth, const int search_depth,
      const std::function<int (const GameState&)>& value_func,
      int alpha, const int beta);
};

bool operator<(const std::unique_ptr<GameTree>& lhs,
               const std::unique_ptr<GameTree>& rhs);

} // namespace gamestate
} // namespace othello


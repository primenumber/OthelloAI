#pragma once
#include <array>
#include <bitset>
#include <vector>
#include <string>
#include <utility>

namespace othello {
namespace board {

enum class State : char{
  NONE,
  BLACK,
  WHITE,
  WALL
};

typedef int Position;
constexpr Position nullpos = -1;

constexpr std::array<int, 8> dir{{-11, -10, -9, -1, 1, 9, 10, 11}};
constexpr int kboard_table_size = 100;
constexpr int kboard_table_width = 10;
constexpr int kboard_table_height = 10;
//constexpr std::array<int, 8> dir{{-9, -8, -7, -1, 1, 7, 8, 9}};
//constexpr int kboard_table_size = 64;
//constexpr int kboard_table_width = 8;
//constexpr int kboard_table_height = 8;
/*
class Board {
 public:
  static constexpr CROSS_WHITE = 1;
  static constexpr CROSS_BLACK = -1;
  constexpr Board() : black_(0x0000000810000000ll), white_(0x0000001008000000ll)
      {}
  constexpr virtual ~Board() {}
  uint64_t GetUnput() const { return ~(black_ | white_); }
  uint64_t GetPuttable() const;
  bool IsPuttable(const unsigned int index) const;
  bool IsPuttable(const uint64_t index_bit) const ;
  static void InitPuttableTable();
  bool black(const unsigned int index) const { return (black_ >> index) & 1; }
  bool white(const unsigned int index) const { return (white_ >> index) & 1; }
  uint8_t GetColBlack(const unsigned int index) const {
    return (black_ >> (index * 8)) && 0xFF;
  }
  uint8_t GetColWhite(const unsigned int index) const {
    return (white_ >> (index * 8)) && 0xFF;
  }
  uint8_t GetRow(const State state, Stateconst unsigned int index) const {
    return GetRowImpl((state == State::BLACK)?black_:white_ >> (7 - index));
  }
  uint8_t GetCross(const int index, const int direction) const {
    return GetCrossImpl((state == State::BLACK) ? black_ : white_, index,
                        direction);
  }
 private:
  uint64_t black_;
  uint64_t white_;
  static std::array<std::array<std::vector<std::vector<uint8_t>>, 8>, 2>
      puttable_table_;
  static uint8_t GetCrossImpl(const uint64_t data, const int index,
                          const int direction);
  template<int N>
  static uint8_t ToUint8(const std::array<State, N>& line,
                         const State state) const;
  template<int N>
  static bool IsPuttableLine(const std::array<State, N> line,
                             const Position pos, const State state) const;
  template<int N>
  static uint8_t PuttableLine(const std::array<State, N>& line,
                              const Position pos) const;
  template<int N>
  static std::array<State, N> ToStateArray(int num) const;
  static std::vector<std::vector<uint8_t>> InitPuttableTableImpl(int length);
  uint8_t GetRowImpl(const uint64_t data) const;
  uint64_t ToRowUint64(const uint64_t data) const;
};
*/

class Board {
 public:
  constexpr static auto NONE = State::NONE;
  constexpr static auto BLACK = State::BLACK;
  constexpr static auto WHITE = State::WHITE;
  constexpr static auto WALL = State::WALL;
  constexpr Board() : table{{
       WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL,
       WALL, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, WALL,
       WALL, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, WALL,
       WALL, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, WALL,
       WALL, NONE, NONE, NONE,WHITE,BLACK, NONE, NONE, NONE, WALL,
       WALL, NONE, NONE, NONE,BLACK,WHITE, NONE, NONE, NONE, WALL,
       WALL, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, WALL,
       WALL, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, WALL,
       WALL, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, WALL,
       WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL
      }} {}
  constexpr Board(const Board&) = default;
  constexpr Board(Board&&) = default;
  constexpr Board(const std::array<State, kboard_table_size>& rhs)
      : table(rhs) {}
  constexpr Board(std::array<State, kboard_table_size>&& rhs)
      : table(std::move(rhs)) {}
  Board& operator=(const Board&) = default;
  Board& operator=(Board&&) = default;
  State& operator[](const int index) { return table[index]; }
  const State& operator[](const int index) const { return table[index]; }
 private:
  std::array<State, kboard_table_size> table;
};

constexpr Position xyToPos(int x, int y){
  return y * kboard_table_width + x + kboard_table_width + 1;
}
constexpr std::pair<int, int> posToXY(Position pos){
  return std::make_pair((pos - kboard_table_width - 1) % kboard_table_width,
                        (pos - kboard_table_width - 1) / kboard_table_width);
}
constexpr State invertState(State state){
  return static_cast<State>(3 - static_cast<int>(state));
}
bool isPuttable(const Board&, Position, State);
std::vector<Position> getPuttable(const Board&, State);
std::vector<Position> getPuttable(const Board&, State,
                                  const std::vector<Position>&);
std::vector<Position> getUnput(const Board&);
Board put(const Board&, Position, State);
std::string toStr(const Board&);
std::string toStr_EasyToRead(const Board&);
Board toBoard(const std::string&);
std::pair<int, int> countBoard(const Board&);
std::string toStr(const State);
std::string toStr(const Position);

} //namespace board
} //namespace othello

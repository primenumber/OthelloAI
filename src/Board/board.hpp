#pragma once
#include <array>
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

class Board {
 public:
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
  constexpr static auto NONE = State::NONE;
  constexpr static auto BLACK = State::BLACK;
  constexpr static auto WHITE = State::WHITE;
  constexpr static auto WALL = State::WALL;
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

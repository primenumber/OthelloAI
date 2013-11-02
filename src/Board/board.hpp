#pragma once
#include <array>
#include <vector>
#include <string>
#include <utility>

namespace othello {
namespace board {

enum class State {
  NONE,
  BLACK,
  WHITE,
  WALL
};

typedef int Position;
constexpr Position nullpos = -1;
//enum Position {
//  W00, W01, W02, W03, W04, W05, W06, W07, W08, W09,
//  W10,  A1,  B1,  C1,  D1,  E1,  F1,  G1,  H1, W19,
//  W20,  A2,  B2,  C2,  D2,  E2,  F2,  G2,  H2, W29,
//  W30,  A3,  B3,  C3,  D3,  E3,  F3,  G3,  H3, W39,
//  W40,  A4,  B4,  C4,  D4,  E4,  F4,  G4,  H4, W49,
//  W50,  A5,  B5,  C5,  D5,  E5,  F5,  G5,  H5, W59,
//  W60,  A6,  B6,  C6,  D6,  E6,  F6,  G6,  H6, W69,
//  W70,  A7,  B7,  C7,  D7,  E7,  F7,  G7,  H7, W79,
//  W80,  A8,  B8,  C8,  D8,  E8,  F8,  G8,  H8, W89,
//  W90, W91, W92, W93, W94, W95, W96, W97, W98, W99,
//};

constexpr std::array<int, 8> dir{{-11, -10, -9, -1, 1, 9, 10, 11}};
constexpr int kboard_table_size = 100;
constexpr int kboard_table_width = 10;
constexpr int kboard_table_height = 10;

class Board : public std::array<State, kboard_table_size> {
 public:
  constexpr Board() : std::array<State, kboard_table_size>{{
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
      : std::array<State, kboard_table_size>(rhs) {}
  constexpr Board(std::array<State, kboard_table_size>&& rhs)
      : std::array<State, kboard_table_size>(std::move(rhs)) {}
  Board& operator=(const Board&) = default;
  Board& operator=(Board&&) = default;
 private:
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
std::string toStr(const State&);

} //namespace board
} //namespace othello

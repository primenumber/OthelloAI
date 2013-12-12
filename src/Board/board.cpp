#include <sstream>
#include "board.hpp"

namespace othello {
namespace board {
/*
constexpr int pow3(int index) {
  return (index > 0) ? pow3(index-1) * 3 : 1;
}

bool Board::IsPuttable(const uint64_t index_bit) const {
  return 
}

uint64_t Board::GetPuttable(const State state) const {
  uint64_t puttable = 0ll;
  int state_num = (state == State::BLACK) ? 0 : 1;
  for (int i = 0; i < 8; ++i) {
    puttable |= puttable_table_[state_num][7][GetColBlack(i)][GetColWhite(i)]
        << (7 - i) * 8;
    puttable |= ToRowUint64(
        puttable_table_[state_num][7][GetRowBlack(i)][GetRowWhite(i)])
        << (7 - i);
  }
  for (int i = -5; i <= 5; ++i) {
    puttable |= ToCrossUint64(
        puttable_table_[state_num][8 - abs(i)]
            [GetCross(State::BLACK, i, 1)][GetCross(State::WHITE, i, 1)],
        1)
        << (7 - i) * 8;
  }
  return puttable;
}

void Board::InitPuttableTable() {
  for (int i = 3; i <= 8; ++i)
    InitPuttableTableImpl(i);
}

uint8_t Board::GetCrossImpl(const uint64_t data, const int index,
                        const int direction) const {
  uint8_t result = 0;
  if (direction == WHITE_LINE) {
    for (int i = 0; i < 8 - abs(index); ++i)
      result |= (data >> ((i + std::max(index, 0)) * 8)) && (1 << i);
  } else {
    for (int i = 0; i < 8 - abs(index); ++i)
      result |= (data >> (((7 - i) + std::max(index, 0)) * 6)) && (1 << i);
  }
  return result;
}

template<int N>
uint8_t Board::ToUint8(const std::array<State, N>& line,
                       const State state) const {
  uint8_t result = 0;
  for (int i = 0; i < N; ++i)
    result |= (line[i] == state) ? (1 << i) : 0;
  return result;
}

template<int N>
bool Board::IsPuttableLine(std::array<State, N> line, Position position,
                           State state) const {
  bool flag = false;
  for (int k = position - 1; k >= 0; --k) {
    if (state == table[k]) {
      if (flag == true) {
        return true;
      }
      break;
    } else {
      flag = true;
    }
  }
  flag = false;
  for (int k = position + 1; k < length; ++k) {
    if (state == table[k]) {
      if (flag == true) {
        return true;
      }
      break;
    } else {
      flag = true;
    }
  }
  return false;
}

template<int N>
uint8_t Board::PuttableLine(const std::array<State, N>& line,
                            const State state) const {
  uint8_t array;
  for (int i = 0; i < N; ++i)
    array |= IsPutableLine(line, i, state) << i;
  return array;
}

template<int N>
std::array<State, N> Board::ToStateArray(int num) const {
  std::array<State, N> array;
  for (int i = 0; i < N; ++i) {
    int mod = num % 3;
    num /= 3;
    switch (mod) {
     case 0:
      array[i] = State::NONE;
      break;
     case 1:
      array[i] = State::BLACK;
      break;
     case 2:
      array[i] = State::WHITE;
      break;
    }
  }
  return array;
}

void Board::InitPuttableTableImpl(int length) {
  puttable_table_[0][length - 1] = std::vector<std::vector<uint8_t>>(
      pow3(length), std::vector<uint8_t>(pow3(length)));
  puttable_table_[1][length - 1] = std::vector<std::vector<uint8_t>>(
      pow3(length), std::vector<uint8_t>(pow3(length)));
  for (int i = 0; i < pow3(length); ++i) {
    std::array<State, length> table = ToStateArray(i);
    puttable_table_[0][length - 1][ToUint8(table,State::BLACK)]
        [ToUint8(table,State::WHITE)] = PuttableLine(table, State::BLACK);
    puttable_table_[1][length - 1][ToUint8(table,State::BLACK)]
        [ToUint8(table,State::WHITE)] = PuttableLine(table, State::WHITE);
  }
}

uint8_t Board::GetRowImpl(const uint64_t data) const {
  uint8_t result = 0;
  for (int i = 0; i < 8; ++i)
    result |= (data >> (i * 7)) && (1 << i);
  return result;
}

uint64_t Board::ToRowUint64(const uint64_t data) const {
  uint64_t result;
  for (int i = 0; i < 8; ++i) {
    result |= (data & (1 << i)) << (i * 7);
  }
}
*/
bool isPuttable(const Board& board, const Position pos,
                const State state) {
  if (board[pos] != State::NONE)
    return false;
  else {
    for (auto d : dir) {
      bool isptb = false;
      Position q = pos;
      while (1) {
        q += d;
        if (board[q] == state) {
          if (isptb)
            return true;
          break;
        }
        else if (board[q] == invertState(state))
          isptb=true;
        else
          break;
      }
    }
    return false;
  }
}

std::vector<Position> getPuttable(const Board& board, State state) {
  std::vector<Position> ans;
  ans.reserve(16);
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      Position pos = xyToPos(j, i);
      if (isPuttable(board, pos, state))
        ans.push_back(pos);
    }
  }
  return ans;
}

std::vector<Position> getPuttable(const Board& board, State state, 
															    const std::vector<Position>& nonevec) {
  std::vector<Position> ans;
  ans.reserve(nonevec.size());
  for (auto pos : nonevec)
    if (isPuttable(board, pos, state))
      ans.push_back(pos);
  return ans;
}

Board put(const Board& board, Position pos, State state) {
  Board ans(board);
  ans[pos] = state;
  for (auto d : dir) {
    bool isptb = false;
    Position q = pos;
    while (1) {
      q += d;
      if (ans[q] == state) {
        if (isptb) {
          q -= d;
          while (q != pos) {
            ans[q] = state;
            q -= d;
          }
        }
        break;
      }
      else if (ans[q] == invertState(state))
        isptb=true;
      else
        break;
    }
  }
  return ans;
}

std::vector<Position> getUnput(const Board& board) {
  std::vector<Position> ans;
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      Position pos = xyToPos(j, i);
      if (board[pos] == State::NONE)
        ans.push_back(pos);
    }
  }
  return ans;
}

std::string toStr(const Board& board) {
  std::stringstream ss;
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      Position pos = xyToPos(j, i);
      switch (board[pos]) {
       case State::NONE:
        ss << '.';
        break;
       case State::BLACK:
        ss << 'x';
        break;
       case State::WHITE:
        ss << 'o';
        break;
        default:;
      }
    }
    ss << '\n';
  }
  return ss.str();
}

std::string toStr_EasyToRead(const Board& board) {
  std::stringstream ss;
  ss << " |a b c d e f g h\n";
  ss << "-+---------------\n";
  for (int i = 0; i < 8; ++i) {
    ss << (i+1) << '|';
    for (int j = 0; j < 8; ++j) {
      if(j)
        ss << ' ';
      Position pos = xyToPos(j, i);
      switch (board[pos]) {
       case State::NONE:
        ss << '.';
        break;
       case State::BLACK:
        ss << 'x';
        break;
       case State::WHITE:
        ss << 'o';
        break;
        default:;
      }
    }
    ss << '\n';
  }
  return ss.str();
}

Board toBoard(const std::string& str) {
  std::stringstream ss;
  ss << str;
  char buf[9];
  Board bd;
  for (int i = 0; i < 8; ++i) {
    ss.getline(buf,9);
    for (int j = 0; j < 8; ++j) {
      switch (buf[j]) {
       case 'x':
        bd[xyToPos(j, i)] = State::BLACK;
        break;
       case 'o':
        bd[xyToPos(j, i)] = State::WHITE;
        break;
       case '.':
        bd[xyToPos(j, i)] = State::NONE;
        break;
      }
    }
  }
  return bd;
}

std::pair<int,int> countBoard(const Board& board) {
  int b = 0;
  int w = 0;
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      switch (board[xyToPos(j, i)]) {
       case State::BLACK:
        ++b;
        break;
       case State::WHITE:
        ++w;
        break;
       default:;
      }
    }
  }
  return std::make_pair(b, w);
}

Position rotatePos(const int x, const int y, int times) {
  times %= 4;
  switch (times) {
   case 0:
    return xyToPos(x, y);
    break;
   case 1:
    return xyToPos(7-y, x);
    break;
   case 2:
    return xyToPos(7-x, 7-y);
    break;
   case 3:
    return xyToPos(y, 7-x);
    break;
  }
}

Board rotateBoard(const Board& board, int times) {
  Board rotate;
  for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j)
      rotate[rotatePos(i, j, times)] = board[xyToPos(i, j)];
  return rotate;
}

Board reverseBoard(const Board& board) {
  Board reverse;
  for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j)
      reverse[xyToPos(7-i, j)] = board[xyToPos(i, j)];
  return reverse;
}

std::string toStr(const State state) {
  switch(state) {
   case State::NONE:
    return "NONE";
   case State::BLACK:
    return "BLACK";
   case State::WHITE:
    return "WHITE";
   case State::WALL:
    return "WALL";
  }
}

std::string toStr(const Position position) {
  auto xy = posToXY(position);
  char col = xy.first + 'a';
  char row = xy.second + '1';
  return std::string("") + col + row;
}

} // namespace board
} // namespace othello

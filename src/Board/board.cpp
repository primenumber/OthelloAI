#include <sstream>
#include "board.hpp"

namespace othello {
namespace board {

bool isPuttable(const Board & board, Position pos, State state) {
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

std::vector<Position> getPuttable(const Board & board, State state) {
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

std::vector<Position> getPuttable(const Board & board, State state, 
															 const std::vector<Position> & nonevec) {
  std::vector<Position> ans;
  ans.reserve(nonevec.size());
  for (auto pos : nonevec)
    if (isPuttable(board, pos, state))
      ans.push_back(pos);
  return ans;
}

Board put(const Board & board, Position pos, State state) {
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

std::vector<Position> getUnput(const Board & board) {
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

std::string toStr(const Board & board) {
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

std::string toStr_EasyToRead(const Board & board) {
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

Board toBoard(const std::string & str) {
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

std::pair<int,int> countBoard(const Board & board) {
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

std::string toStr(const State& state) {
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

} // namespace board
} // namespace othello

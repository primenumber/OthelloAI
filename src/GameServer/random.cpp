#include <algorithm>
#include <iostream>
#include <string>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>
#include "Board/board.hpp"

int main() {
  using othello::board::getPuttable;
  using othello::board::toBoard;
  using othello::board::Position;
  using othello::board::State;
  using othello::board::xyToPos;
  using othello::board::posToXY;
  using std::cin;
  using std::cout;
  using std::endl;
  srand(getpid());
  char buf[81];
  sprintf(buf, "log/rndo_log_%d.log", getpid());
  FILE* fp = fopen(buf, "w");
  std::string player;
  cin >> player;
  State state;
  if (player == "x") {
    fprintf(fp, "black\n");
    state = State::BLACK;
  } else {
    fprintf(fp, "white\n");
    state = State::WHITE;
  }
  while (1) {
    std::string rt_str;
    long long remain_time;
    cin >> rt_str;
    fprintf(fp, "remain:%s\n", rt_str.c_str());
    remain_time = atoll(rt_str.c_str());
    if (remain_time == 0)
      break;
    std::string board_str;
    for (int i = 0; i < 8; ++i) {
      std::string str;
      cin >> str;
      board_str += str + '\n';
    }
    std::vector<Position> puttable = getPuttable(toBoard(board_str), state);
    int num = puttable.size();
    if (num) {
      Position pos = puttable[rand()%num];
      char col = posToXY(pos).first + 'a';
      char row = posToXY(pos).second + '1';
      cout << col << row << endl;
      fprintf(fp, "%c%c\n", col, row);
    } else {
      fprintf(fp, "pass\n");
    }
  }
  fprintf(fp, "--game set--\n");
  fclose(fp);
  return 0;
}


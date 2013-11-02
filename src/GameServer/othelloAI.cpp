#include "othelloAI.hpp"

namespace othello {
namespace server {

bool OthelloAI::Init(board::State state, std::chrono::nanoseconds init_time) {
  using std::chrono::duration_cast;
  using std::chrono::nanoseconds;
  using std::chrono::seconds;
  int isntexist = access(named_pipe_name_.c_str(), R_OK);
  if(isntexist == -1) { 
    int error = mkfifo(named_pipe_name_.c_str(), 0666);
    if (error != 0) {
      std::cerr << "make pipe failed:" << named_pipe_name_ << std::endl;
      return false;
    }
  }
  std::string call_str = ai_name_ + " > " + named_pipe_name_;
  FILE* fp = popen(call_str.c_str(), "w");
  if (fp == nullptr) {
    std::cerr << "open ai failed" << std::endl;
    return false;
  }
  ai_pipe_.reset(fp);
  if (state == board::State::BLACK) {
    fprintf(ai_pipe_.get(), "x\n");
  } else {
    fprintf(ai_pipe_.get(), "o\n");
  }
  timespec wait_tv;
  wait_tv.tv_sec = duration_cast<seconds>(init_time).count();
  wait_tv.tv_nsec = init_time.count() % 1000000000;
  nanosleep(&wait_tv, nullptr);
  ai_file_descriptor_ = open(named_pipe_name_.c_str(), O_RDONLY);
  state_ = state;
  return true;
}

void OthelloAI::Reset() {
  ai_file_descriptor_.close();
  pclose(ai_pipe_.release());
}

board::Position OthelloAI::Play(board::Board& board,
                                std::chrono::nanoseconds remain_time) {
  using std::chrono::duration_cast;
  using board::xyToPos;
  using board::toStr;
  using std::chrono::seconds;
  using std::chrono::nanoseconds;
  using std::chrono::duration;
  std::fprintf(ai_pipe_.get(),
               "%lld\n%s",
               duration_cast<nanoseconds>(remain_time).count(), 
               toStr(board).c_str());
  fflush(ai_pipe_.get());
  fflush(stdout);
  fd_set out_pipe_set;
  FD_ZERO(&out_pipe_set);
  FD_SET(ai_file_descriptor_, &out_pipe_set);
  timespec wait_tv;
  wait_tv.tv_sec = duration_cast<seconds>(remain_time).count();
  wait_tv.tv_nsec = duration_cast<nanoseconds>(remain_time).count() % 1000000000;
  int ready = pselect(ai_file_descriptor_ + 1, &out_pipe_set, nullptr, nullptr,
                      &wait_tv, nullptr);
  if (ready == 1) {
    const int pos_string_size = 3;
    char buf[pos_string_size];
    read(ai_file_descriptor_, buf, pos_string_size);
    int col = buf[0] - 'a';
    int row = buf[1] - '1';
    if (col < 0 || col >= 8 || row < 0 || row >= 8) {
      std::cerr << "out of range" << col << ' ' << row << std::endl;
      return board::nullpos;
    } else if(isPuttable(board, xyToPos(col, row), state_))
      return xyToPos(col, row);
    else {
      std::cerr << "unputtable output" << std::endl;
      return board::nullpos;
     }
  } else {
    return board::nullpos;
  }
}

void OthelloAI::End() {
  std::fprintf(ai_pipe_.get(), "%lld\n", 0);
}

board::Position Human::Play(board::Board& board,
                            std::chrono::nanoseconds remain_time) {
  using std::cout;
  using std::cin;
  using std::endl;
  using std::find;
  using board::xyToPos;
  using board::toStr_EasyToRead;
  std::vector<board::Position> puttable = getPuttable(board, state_);
  cout << toStr_EasyToRead(board) << endl;
  board::Position pos;
  do {
    cout << "Input Position ("
         << static_cast<std::chrono::duration<double>>(remain_time).count() 
         << " seconds remain)." << endl;
    std::string input;
    cin >> input;
    if (input.size() < 2) continue;
    int col = input[0] - 'a';
    int row = input[1] - '1';
    pos = xyToPos(col, row);
  } while (find(puttable.begin(), puttable.end(), pos) == puttable.end());
  return pos;
}

} // namespace server
} // namespace othello

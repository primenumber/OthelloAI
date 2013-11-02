#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "Board/board.hpp"
#include "gametimer.hpp"
#include "othelloAI.hpp"

namespace othello {
namespace server {

char toBase64(unsigned int n)
{
  if (n < 26)
    return n + 'A';
  else if (n < 52)
    return n - 26 + 'a';
  else if (n < 62)
    return n - 52 + '0';
  else if (n == 62)
    return '+';
  else if (n == 63)
    return '/';
  return '\0';
}

class GameServer {
 public:
  GameServer(std::unique_ptr<OthelloAI>&& ai_black,
             std::unique_ptr<OthelloAI>&& ai_white) noexcept
//      : pipe_name_black_(pipe_name_black),
//        pipe_name_white_(pipe_name_white), 
//        ai_name_black_(ai_name_black),
//        ai_name_white_(ai_name_white),
//        ai_black_(board::State::BLACK),
//        ai_white_(board::State::WHITE),
      : ai_black_(std::move(ai_black)),
        ai_white_(std::move(ai_white)),
        board_(),
        state_(board::State::BLACK),
        pass(false),
        record_(),
        time_limit_(5000),
        game_timer_(std::chrono::seconds(time_limit_)),
        is_initialized_(false),
        is_game_ended_(false) {}
  GameServer(const GameServer&) = delete;
  GameServer& operator=(const GameServer&) = delete;
  virtual ~GameServer() {}
  bool Init();
  bool Start();
  bool ShowResult();
 private:
   std::unique_ptr<OthelloAI> ai_black_;
   std::unique_ptr<OthelloAI> ai_white_;
//  std::string pipe_name_black_;
//  std::string pipe_name_white_;
//  std::string ai_name_black_;
//  std::string ai_name_white_;
//  OthelloAI ai_black_;
//  OthelloAI ai_white_;
  board::Board board_;
  board::State state_;
  bool pass;
  std::string record_;
  int64_t time_limit_;
  GameTimer game_timer_;
  bool is_initialized_;
  bool is_game_ended_;
  bool GamePlay();
  void Toggle();
};

bool GameServer::Init() {
  std::cerr << "initializing..." << std::endl;
  if (!ai_black_->Init(board::State::BLACK, //wait 1000ms for initialization
                       std::chrono::nanoseconds(1000000000))) {
    return false;
  }
  if (!ai_white_->Init(board::State::WHITE, //wait 1000ms for initialization
                       std::chrono::nanoseconds(1000000000))) {
    return false;
  }
  is_initialized_ = true;
  return true;
}

bool GameServer::Start() {
  using std::chrono::duration_cast;
  using std::chrono::nanoseconds;
  using std::chrono::seconds;
  if (!is_initialized_ || is_game_ended_)
    return false;
  game_timer_.Start();
  while (GamePlay()) Toggle();
  ai_black_->End();
  ai_white_->End();
  is_game_ended_ = true;
  return true;
}

bool GameServer::GamePlay() {
  using std::cout;
  using std::cerr;
  using std::endl;
  using board::toStr;
  using board::getPuttable;
  auto v = getPuttable(board_, state_);
  if (v.empty()) {
    if (pass) {
      cerr << "no puttable position" << endl;
      return false;
    } else {
      pass = true;
      return true;
    }
  }
  pass = false;
  std::string call_str;
  board::Position pos;
  cerr << toStr(board_) << endl;
  if (state_ == board::State::BLACK) {
    pos = ai_black_->Play(board_, game_timer_.GetRemainNowPlaying());
  } else {
    pos = ai_white_->Play(board_, game_timer_.GetRemainNowPlaying());
  }
  if (pos != board::nullpos) {
    board_ = put(board_, pos, state_);
    return true;
  } else {
    cerr << "unexpected output" << endl;
    return false;
  }
}

void GameServer::Toggle() {
  state_ = board::invertState(state_);
  game_timer_.Toggle();
}

bool GameServer::ShowResult() {
  using std::cout;
  using std::endl;
  using board::toStr;
  cout << "<Game Set>" << endl;
  cout << toStr(board_) << endl;
  return true;
}

} //namespace server
} //namespace othello

int main(int argc, char** argv)
{
  using othello::server::OthelloAI;
  using othello::server::GameServer;
  using othello::board::State;
  if (argc < 3) {
    fprintf(stderr,"error: no input programs\nusage:\nothello PROG1 PROG2\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }
  std::string ai_name_black = argv[1];
  std::string ai_name_white = argv[2];
  std::unique_ptr<OthelloAI> ai_black(new OthelloAI(ai_name_black,
                                                     "othello_black.pipe",
                                                     State::BLACK));
  std::unique_ptr<OthelloAI> ai_white(new OthelloAI(ai_name_white,
                                                     "othello_white.pipe",
                                                     State::WHITE));
  GameServer game_server(std::move(ai_black), std::move(ai_white));
  bool i = game_server.Init();
  bool s = false;
  if(i) {
    s = game_server.Start();
    game_server.ShowResult();
  }
  return 0;
}

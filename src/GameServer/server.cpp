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
  GameServer(int time_limit_seconds,
             std::unique_ptr<OthelloAI>&& ai_black,
             std::unique_ptr<OthelloAI>&& ai_white) noexcept
      : ai_black_(std::move(ai_black)),
        ai_white_(std::move(ai_white)),
        board_(),
        state_(board::State::BLACK),
        pass(false),
        record_(),
        time_limit_(time_limit_seconds),
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
  using board::toStr_EasyToRead;
  using board::posToXY;
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
  if(pass) {
    auto invert_state = GameTimer::InvertState(game_timer_.GetState());
    if (invertState(state_) == board::State::BLACK) {
      ai_black_->Pass(board_, game_timer_.GetRemain(invert_state));
    } else {
      ai_white_->Pass(board_, game_timer_.GetRemain(invert_state));
    }
    pass = false;
  }
  std::string call_str;
  board::Position pos;
  cerr << toStr_EasyToRead(board_) << endl;
  if (state_ == board::State::BLACK) {
    pos = ai_black_->Play(board_, game_timer_.GetRemainNowPlaying());
  } else {
    pos = ai_white_->Play(board_, game_timer_.GetRemainNowPlaying());
  }
  if (pos != board::nullpos) {
    auto xy = posToXY(pos);
    char col = xy.first + 'a';
    char row = xy.second + '1';
    cerr << toStr(state_) << " : " << col << row << endl;
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
  auto stones = countBoard(board_);
  cout << "<Game Set>" << endl;
  cout << toStr_EasyToRead(board_) << endl;
  cout << "Black : " << stones.first << " White : " << stones.second << endl;
  if (stones.first > stones.second) {
    cout << "Winner : Black" << endl;
  } else if ( stones.first < stones.second) {
    cout << "Winner : White" << endl;
  } else {
    cout << "Draw" << endl;
  }
  return true;
}

} //namespace server
} //namespace othello

int main(int argc, char** argv)
{
  using othello::server::OthelloAI;
  using othello::server::Human;
  using othello::server::GameServer;
  using othello::board::State;
  using std::cerr;
  using std::endl;
  if (argc < 4) {
    cerr << "error: no input programs" << endl;
    cerr << "usage: ogs PROG1 PROG2" << endl;
    cerr << "if PROG1=human, Black will play human," << endl;
    cerr << "if PROG2=human, White will play human." << endl;
    exit(EXIT_FAILURE);
  }
  int time_limit = atoi(argv[1]);
  std::string ai_name_black = argv[2];
  std::string ai_name_white = argv[3];
  std::unique_ptr<OthelloAI> ai_black;
  std::unique_ptr<OthelloAI> ai_white;
  if(ai_name_black != "human") {
    ai_black = std::unique_ptr<OthelloAI>(new OthelloAI(ai_name_black,
        "othello_black.pipe", State::BLACK));
  } else {
    ai_black = std::unique_ptr<OthelloAI>(new Human(State::BLACK));
  }
  if(ai_name_white != "human") {
    ai_white = std::unique_ptr<OthelloAI>(new OthelloAI(ai_name_white,
        "othello_white.pipe", State::WHITE));
  } else {
    ai_white = std::unique_ptr<OthelloAI>(new Human(State::WHITE));
  }
  GameServer game_server(time_limit, std::move(ai_black), std::move(ai_white));
  bool i = game_server.Init();
  bool s = false;
  if(i) {
    s = game_server.Start();
    game_server.ShowResult();
  }
  return 0;
}

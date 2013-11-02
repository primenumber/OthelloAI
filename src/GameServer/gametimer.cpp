#include "gametimer.hpp"

void GameTimer::Start() {
  if (!is_running_) {
    rap_ = std::chrono::system_clock::now();
    is_running_ = true;
  }
}

bool GameTimer::Toggle() {
  using std::chrono::duration_cast;
  auto tmp = std::chrono::system_clock::now();
  auto duration = tmp - rap_;
  if (state_ == State::FIRST) {
    timer_first_ += duration;
    if (timer_first_ >= time_limit_) {
      is_running_ = false;
      return false;
    }
    state_ = State::SECOND;
  } else {
    timer_second_ += duration;
    if (timer_second_ >= time_limit_) {
      is_running_ = false;
      return false;
    }
    state_ = State::FIRST;
  }
  rap_ = tmp;
  return true;
}

void GameTimer::Stop() {
  is_running_ = false;
}

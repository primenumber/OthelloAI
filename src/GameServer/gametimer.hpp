#pragma once

#include <chrono>

class GameTimer {
 public:
 	enum class State {
 		FIRST,
 		SECOND
 	};
  GameTimer(std::chrono::nanoseconds time_limit)
      : state_(State::FIRST),
        time_limit_(time_limit),
        timer_first_(),
        timer_second_(),
        rap_(),
        is_running_(false) {}
  virtual ~GameTimer() = default;
  void Start();
  void Stop();
  void Reset();
  bool Toggle();
  std::chrono::nanoseconds GetRemainFirst() { return time_limit_ - timer_first_; }
  std::chrono::nanoseconds GetRemainSecond() { return time_limit_ - timer_second_; }
  std::chrono::nanoseconds GetRemainNowPlaying() {
		return state_ == State::FIRST ? 
										 GetRemainFirst() : GetRemainSecond();
  }
 private:
  State state_;
  std::chrono::nanoseconds time_limit_;
  std::chrono::nanoseconds timer_first_;
  std::chrono::nanoseconds timer_second_;
  std::chrono::time_point<std::chrono::system_clock> rap_;
  bool is_running_;
};


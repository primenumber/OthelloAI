#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <utility>
#include "Board/board.hpp"
#include "fileDescriptor.hpp"
#include "gametimer.hpp"

namespace othello {
namespace server {

class OthelloAI {
 public:
  OthelloAI(board::State state)
      : state_(state), ai_name_(), named_pipe_name_(), ai_file_descriptor_(-1),
        ai_pipe_(nullptr, pclose) {}
  OthelloAI(const std::string& ai_name, const std::string& named_pipe_name,
            board::State state)
      : state_(state), ai_name_(ai_name), named_pipe_name_(named_pipe_name),
        ai_file_descriptor_(-1), ai_pipe_(nullptr, pclose) {}
  virtual ~OthelloAI() {}
  virtual OthelloAI& operator=(OthelloAI&& rhs) = default;
  virtual bool Init(board::State state, std::chrono::nanoseconds init_time);
  virtual void Reset();
  virtual board::Position Play(board::Board& board,
                               std::chrono::nanoseconds remain_time);
  virtual void Pass(board::Board& board, std::chrono::nanoseconds remain_time);
  virtual void End();
 protected:
  board::State state_;
 private:
  std::string ai_name_;
  std::string named_pipe_name_;
  lib::FileDescriptor ai_file_descriptor_;
  std::unique_ptr<FILE, decltype( &pclose )> ai_pipe_;
};

class Human : public OthelloAI {
 public:
  Human(board::State state) : OthelloAI(state) {}
  bool Init(board::State /*state*/, std::chrono::nanoseconds /*init_time*/) {
    return true;
  }
  void Reset() {}
  board::Position Play(board::Board& board,
                       std::chrono::nanoseconds remain_time);
  void Pass(board::Board& board, std::chrono::nanoseconds remain_time) {};
  void End() {}
};

} // namespace server
} // namespace othello

#pragma once

#include <unistd.h>

namespace lib {

class FileDescriptor {
 public:
  FileDescriptor(int file_descriptor) : file_descriptor_(file_descriptor) {}
  virtual ~FileDescriptor() {
    if (file_descriptor_ >= 0)
      ::close(file_descriptor_);
  }
  void close() {
    if (file_descriptor_ >= 0) {
      ::close(file_descriptor_);
      file_descriptor_ = -1;
    }
  }
  operator int() { return file_descriptor_; }
  FileDescriptor& operator=(FileDescriptor&& rhs) {
    file_descriptor_ = rhs.file_descriptor_;
    rhs.file_descriptor_ = -1;
    return *this;
  }
 private:
  int file_descriptor_;
};

} // namespace lib

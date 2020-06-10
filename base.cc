/*
 * Copyright (C) 2020 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "base.h"

#include <string.h>
#include <sys/socket.h>

namespace evilbc {
namespace {
Semantics get_semantics() {
  char* e = getenv("EVILBC_SEMANTICS");
  if (e == nullptr) {
    return kGlibc;
  }

  if (strcasecmp(e, "glibc") == 0) {
    return kGlibc;
  }
  if (strcasecmp(e, "posix") == 0) {
    return kPosix;
  }
  abort();
}
}  // namespace
thread_local ThreadState thread_state;

std::mt19937& ThreadState::rand() { return rand_; }

size_t ThreadState::randomize_size(size_t size) {
  if (size < 2) {
    return size;
  }

  std::uniform_int_distribution d(static_cast<size_t>(1u), size);
  return d(rand());
}

Semantics semantics() {
  static Semantics s = get_semantics();
  return s;
}

Scope::Scope() : prev_(thread_state.in_evilbc_) {
  thread_state.in_evilbc_ = true;
}
Scope::~Scope() { thread_state.in_evilbc_ = prev_; }

bool is_stream_sock(int sockfd) {
  int socktype;
  socklen_t socktype_len = sizeof(socktype);
  return (getsockopt(sockfd, SOL_SOCKET, SO_TYPE, &socktype, &socktype_len) !=
              -1 &&
          socktype == SOCK_STREAM);
}

}  // namespace evilbc

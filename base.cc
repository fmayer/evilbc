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

namespace evilbc {
namespace {
bool get_strict_posix() {
  char* e = getenv("EVILBC_STRICT_POSIX");
  if (e == nullptr) {
    return false;
  }
  return strncmp(e, "1", 1) == 0;
}
}  // namespace
thread_local ThreadState thread_state;

std::mt19937& ThreadState::rand() { return rand_; }

bool is_strict_posix() {
  static bool strict = get_strict_posix();
  return strict;
}

Scope::Scope() : prev_(thread_state.in_evilbc_) {
  thread_state.in_evilbc_ = true;
}
Scope::~Scope() { thread_state.in_evilbc_ = prev_; }
}  // namespace evilbc

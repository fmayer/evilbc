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

#include <unistd.h>

#include "base.h"

namespace evilbc {
EVILBC_EXPORT
unsigned int sleep(unsigned int seconds) {
  if (thread_state.in_evilbc()) {
    return EVILBC_RUN_LIBC(sleep, seconds);
  }

  Scope s;

  if (seconds == 0) {
    return 0;
  }
  // If we sleep up  to 400us, the rounding logic will return the same value as
  // before.
  usleep(400);
  return seconds;
}
}  // namespace evilbc

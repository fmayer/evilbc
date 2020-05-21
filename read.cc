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

#include <dlfcn.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "base.h"

namespace evilbc {
EVILBC_EXPORT
extern "C" ssize_t read(int fd, void *buf, size_t count) {
  if (thread_state.in_evilbc) {
    return EVILBC_RUN_LIBC(read, fd, buf, count);
  }

  Scope s;

  struct stat statbuf;
  if (fstat(fd, &statbuf) == -1) {
    errno = EBADF;
    return -1;
  }
  if (S_TYPEISSHM(&statbuf)) {
    abort();
  }
  if (is_strict_posix() || S_ISSOCK(statbuf.st_mode) ||
      S_ISFIFO(statbuf.st_mode)) {
    if (rand_bool()) {
      errno = EINTR;
      return -1;
    }
    return EVILBC_RUN_LIBC(read, fd, buf, count - 1);
  }
  return EVILBC_RUN_LIBC(read, fd, buf, count);
}
}  // namespace evilbc

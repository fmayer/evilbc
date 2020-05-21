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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

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
  switch (statbuf.st_mode & S_IFMT) {
  case S_IFSOCK:
  case S_IFIFO:
    return EVILBC_RUN_LIBC(read, fd, buf, count - 1);
  default:
    return EVILBC_RUN_LIBC(read, fd, buf, count);
  }
}
}

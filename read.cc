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

#include <assert.h>
#include <dlfcn.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "base.h"

namespace evilbc {
EVILBC_EXPORT
extern "C" ssize_t read(int fd, void *buf, size_t count) {
  if (thread_state.in_evilbc()) {
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

  // Regular files don't appear to get short reads on Linux, but POSIX has
  // nothing to say on the matter.
  // It states simply:
  //   If a read() is interrupted by a signal before it reads any data,
  //   it shall return -1 with errno set to [EINTR].
  //
  //   If a read() is interrupted by a signal after it has successfully read
  //   some data, it shall return the number of bytes read.
  // -- https://pubs.opengroup.org/onlinepubs/009695399/functions/read.html
  if (semantics() == kPosix ||
      (S_ISSOCK(statbuf.st_mode) && is_stream_sock(fd))) {
    // Bias towards EINTR, so every callsite probably gets one.
    if (thread_state.biased_rand_bool()) {
      errno = EINTR;
      return -1;
    }
    return EVILBC_RUN_LIBC(read, fd, buf, thread_state.randomize_size(count));
  }
  if (S_ISFIFO(statbuf.st_mode)) {
    // On Linux 4.x, which is new enough to still care about, read(2) on a pipe
    // can return after only having read one of its buffers. On 5.x it only
    // returns after the pipe is empty or the requested number of bytes have
    // been read.
    // These buffers are page sized, so we simulate by reading some number
    // of pages.
    // https://elixir.bootlin.com/linux/v4.20.17/source/fs/pipe.c#L324
    long page_sz = sysconf(_SC_PAGESIZE);
    size_t count_pages = count / page_sz;
    if (count % page_sz) count_pages++;

    size_t read_pages = thread_state.randomize_size(count_pages);
    size_t read_bytes =
        read_pages == count_pages ? count : count_pages * page_sz;
    assert(read_bytes <= count);
    return EVILBC_RUN_LIBC(read, fd, buf, read_bytes);
  }
  return EVILBC_RUN_LIBC(read, fd, buf, count);
}
}  // namespace evilbc

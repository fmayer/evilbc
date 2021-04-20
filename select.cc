/*
 * Copyright (C) 2021 The Android Open Source Project
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

#include <sys/select.h>

#include "base.h"

namespace evilbc {
namespace {
int count_fds(int nfds, fd_set *fds) {
  int n = 0;
  for (int i = 0; i < nfds; ++i) {
    if (FD_ISSET(i, fds)) {
      n++;
    }
  }
  return n;
}
}  // namespace

EVILBC_EXPORT
int select(int nfds, fd_set *__restrict readfds, fd_set *__restrict writefds,
           fd_set *__restrict exceptfds, struct timeval *__restrict timeout) {
  if (thread_state.in_evilbc()) {
    return EVILBC_RUN_LIBC(select, nfds, readfds, writefds, exceptfds, timeout);
  }

  Scope s;

  // Bias towards unexpected behaviour, so every callsite probably gets one.
  if (thread_state.biased_rand_bool()) {
    if (thread_state.unbiased_rand_bool()) {
      errno = EINTR;
      return -1;
    } else {
      // Spuriously wake up all the FDs.
      int n = 0;
      n += count_fds(nfds, readfds);
      n += count_fds(nfds, writefds);
      n += count_fds(nfds, exceptfds);
      // Only if we had some FDs empty, otherwise the user is abusing this
      // as usleep, and we shouldn't spuriously wake up.
      if (n > 0) {
        return n;
      }
    }
  }
  return EVILBC_RUN_LIBC(select, nfds, readfds, writefds, exceptfds, timeout);
}

EVILBC_EXPORT
int pselect(int nfds, fd_set *__restrict readfds, fd_set *__restrict writefds,
            fd_set *__restrict exceptfds,
            const struct timespec *__restrict timeout,
            const sigset_t *__restrict sigmask) {
  if (thread_state.in_evilbc()) {
    return EVILBC_RUN_LIBC(pselect, nfds, readfds, writefds, exceptfds, timeout,
                           sigmask);
  }

  Scope s;

  // Bias towards unexpected behaviour, so every callsite probably gets one.
  if (thread_state.biased_rand_bool()) {
    if (thread_state.unbiased_rand_bool()) {
      errno = EINTR;
      return -1;
    } else {
      // Spuriously wake up all the FDs.
      int n = 0;
      n += count_fds(nfds, readfds);
      n += count_fds(nfds, writefds);
      n += count_fds(nfds, exceptfds);
      // Only if we had some FDs empty, otherwise the user is abusing this
      // as usleep, and we shouldn't spuriously wake up.
      if (n > 0) {
        return n;
      }
    }
  }
  return EVILBC_RUN_LIBC(pselect, nfds, readfds, writefds, exceptfds, timeout,
                         sigmask);
}
}  // namespace evilbc
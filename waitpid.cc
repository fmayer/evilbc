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

#include <sys/types.h>
#include <sys/wait.h>

#include "base.h"

namespace evilbc {

EVILBC_EXPORT
extern "C" pid_t wait(int *wstatus) { return waitpid(-1, wstatus, 0); }

EVILBC_EXPORT
extern "C" pid_t waitpid(pid_t pid, int *wstatus, int options) {
  if (thread_state.in_evilbc()) {
    return EVILBC_RUN_LIBC(waitpid, pid, wstatus, options);
  }
  Scope s;
  if ((options & WNOHANG) == 0 && thread_state.biased_rand_bool()) {
    errno = EINTR;
    return -1;
  }
  return EVILBC_RUN_LIBC(waitpid, pid, wstatus, options);
}

EVILBC_EXPORT
extern "C" int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options) {
  if (thread_state.in_evilbc()) {
    return EVILBC_RUN_LIBC(waitid, idtype, id, infop, options);
  }
  if ((options & WNOHANG) == 0 && thread_state.biased_rand_bool()) {
    errno = EINTR;
    return -1;
  }
  return EVILBC_RUN_LIBC(waitid, idtype, id, infop, options);
}

}  // namespace evilbc

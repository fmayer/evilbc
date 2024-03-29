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
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "base.h"

namespace evilbc {
EVILBC_EXPORT
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen) {
  if (thread_state.in_evilbc()) {
    return EVILBC_RUN_LIBC(recvfrom, sockfd, buf, len, flags, src_addr,
                           addrlen);
  }

  Scope s;

  if (!is_stream_sock(sockfd)) {
    return EVILBC_RUN_LIBC(recvfrom, sockfd, buf, len, flags, src_addr,
                           addrlen);
  }

  // Bias towards EINTR, so every callsite probably gets one.
  if (thread_state.biased_rand_bool()) {
    errno = EINTR;
    return -1;
  }
  return EVILBC_RUN_LIBC(recvfrom, sockfd, buf,
                         thread_state.randomize_size(len), flags, src_addr,
                         addrlen);
}

EVILBC_EXPORT
ssize_t recv(int sockfd, void *buf, size_t len, int flags) {
  return recvfrom(sockfd, buf, len, flags, nullptr, nullptr);
}

EVILBC_EXPORT
ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags) {
  if (thread_state.in_evilbc()) {
    return EVILBC_RUN_LIBC(recvmsg, sockfd, msg, flags);
  }

  Scope s;

  if (!is_stream_sock(sockfd)) {
    return EVILBC_RUN_LIBC(recvmsg, sockfd, msg, flags);
  }

  size_t total_size = 0;
  for (size_t i = 0; i < msg->msg_iovlen; ++i) {
    total_size += msg->msg_iov[i].iov_len;
  }
  if (total_size == 0) {
    // Call the underlying implementation with length 0 so that msg->controllen
    // etc. get set correctly.
    return EVILBC_RUN_LIBC(recvmsg, sockfd, msg, flags);
  }

  // Bias towards EINTR, so every callsite probably gets one.
  if (thread_state.biased_rand_bool()) {
    errno = EINTR;
    return -1;
  }
  struct msghdr new_msg = *msg;
  new_msg.msg_iovlen = thread_state.randomize_size(new_msg.msg_iovlen);
  struct iovec &last_iovec = new_msg.msg_iov[new_msg.msg_iovlen - 1];
  if (last_iovec.iov_len == 0) {
    errno = EINTR;
    return -1;
  }
  last_iovec.iov_len = thread_state.randomize_size(last_iovec.iov_len);
  ssize_t r = EVILBC_RUN_LIBC(recvmsg, sockfd, &new_msg, flags);
  memcpy(msg->msg_name, new_msg.msg_name, new_msg.msg_namelen);
  msg->msg_namelen = new_msg.msg_namelen;
  msg->msg_control = new_msg.msg_control;
  msg->msg_controllen = new_msg.msg_controllen;
  return r;
}
}  // namespace evilbc

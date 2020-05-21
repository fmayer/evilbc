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

#define EVILBC_EXPORT __attribute__((visibility("default")))
#define EVILBC_RUN_LIBC(name, ...) \
  decltype (&name)(dlsym(RTLD_NEXT, #name))(__VA_ARGS__)

namespace evilbc {
struct ThreadState {
  bool in_evilbc = false;
};
extern thread_local ThreadState thread_state;

// Scope that indicates we are within evilbc. Any evilbc functions will
// forward their arguments to the backing libc.
class Scope {
 public:
  Scope();
  ~Scope();

 private:
  bool prev_;
};

}  // namespace evilbc

#  Copyright (C) 2020 The Android Open Source Project              
#  Licensed under the Apache License, Version 2.0 (the "License");                                                                    
#
#  you may not use this file except in compliance with the License.                                                                   
#  You may obtain a copy of the License at                         
#
#       http://www.apache.org/licenses/LICENSE-2.0                 
#
#  Unless required by applicable law or agreed to in writing, software                                                                
#  distributed under the License is distributed on an "AS IS" BASIS,                                                                  
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.                                                           
#  See the License for the specific language governing permissions and                                                                
#  limitations under the License.                                  
cmake_minimum_required(VERSION 3.9)
project(evilbc)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/out)
set(CMAKE_SHARED_LINKER_FLAGS "-static-libstdc++ -ldl -Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/version_script")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -fno-exceptions -fno-unwind-tables")

add_library(
        evilbc
        SHARED
        base.h
        base.cc
        read.cc
        recv.cc
        sleep.cc
)

target_link_libraries(
        evilbc
)

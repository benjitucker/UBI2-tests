#  Copyright (C) 2009 Corentin Chary
# 
#  This file is part of libubiio.
#
#  libubiio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
# 
#  This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#  You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

cmake_minimum_required(VERSION 2.6)

#### PROJECT CONFIG ####
project(libubiio C)
set(CPACK_VERSION_MAJOR "0")
set(CPACK_VERSION_MINOR "1")
set(CPACK_VERSION_PATCH "0")
set(CPACK_PACKAGE_CONTACT "UFFS <contact@uffs.org>")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/GPL.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/INSTALL")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A library providing the same UBI API the kernel does")
set(CPACK_PACKAGE_VENDOR "http://uffs.org/")

set(VERSION "${CPACK_VERSION_MAJOR}.${CPACK_VERSION_MINOR}.${CPACK_VERSION_PATCH}")

if(CMAKE_SYSTEM_NAME MATCHES Linux)
  add_definitions (-D_XOPEN_SOURCE=500) # pread/pwrite
  include_directories(${libubiio_SOURCE_DIR}/)
else()
  message ("For now, libubio has only been tested under GNU/Linux. We would be really interested by your experience under other OS, if you have time to write us at <contact@uffs.org>")
endif(CMAKE_SYSTEM_NAME MATCHES Linux)

## GCC  ##
if(CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_C_FLAGS "$ENV{CFLAGS} -std=gnu99 -O2 -Wall -Wextra -Wcast-align -Wpointer-arith -Wbad-function-cast -Wno-sign-compare")
  set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -O0 -ggdb3 -Werror -DDEBUG=1")
endif(CMAKE_COMPILER_IS_GNUCC)

add_library(ubiio SHARED libubiio.c)
set_target_properties(ubiio PROPERTIES VERSION 0.1 SOVERSION 0)

## Installation ##
install(TARGETS ubiio RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(FILES ubi.h libubiio.h DESTINATION include)

## Tests ##
add_subdirectory(tests)

## Packages ##
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "UBI Input/Output library")
set(CPACK_PACKAGE_VENDOR "uffs.org")
set(CPACK_PACKAGE_VERSION_MAJOR ${CPACK_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${CPACK_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${CPACK_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/GPL.txt)
set(CPACK_SOURCE_IGNORE_FILES "~$" "^${PROJECT_SOURCE_DIR}/\\\\.git" "^${PROJECT_SOURCE_DIR}/build/")
set(CPACK_GENERATOR "DEB;TGZ;TBZ2")
set(CPACK_SOURCE_GENERATOR "TGZ;TBZ2")
## Debian specific ##
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.9-25)")

include(CPack)

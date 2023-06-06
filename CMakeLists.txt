# -----------------------------------------------------------------------------
#
# This file is part of the µOS++ distribution.
#   (https://github.com/micro-os-plus/)
# Copyright (c) 2021 Liviu Ionescu. All rights reserved.
#
# Permission to use, copy, modify, and/or distribute this software
# for any purpose is hereby granted, under the terms of the MIT license.
#
# If a copy of the license was not distributed with this file, it can
# be obtained from https://opensource.org/licenses/MIT/.
#
# -----------------------------------------------------------------------------

# This file is intended to be consumed by applications with:
#
# `add_subdirectory("xpacks/@xpacks/chan-fatfs")`
#
# The result is an interface library that can be added to the linker with:
#
# `target_link_libraries(your-target PUBLIC xpacks::chan-fatfs)`

# -----------------------------------------------------------------------------
## Preamble ##

# https://cmake.org/cmake/help/v3.20/
cmake_minimum_required(VERSION 3.20)

project(xpacks-chan-fatfs
  DESCRIPTION "Chan FAT-FS"
)

if(COMMAND xpack_get_package_name_and_version)
  xpack_get_package_name_and_version("${CMAKE_CURRENT_SOURCE_DIR}/package.json")
  message(VERBOSE "Processing xPack ${PACKAGE_JSON_NAME}@${PACKAGE_JSON_VERSION}...")
endif()

# -----------------------------------------------------------------------------
## The project library definitions ##

add_library(xpacks-chan-fatfs-interface INTERFACE EXCLUDE_FROM_ALL)

# -----------------------------------------------------------------------------
# Target settings.

target_include_directories(xpacks-chan-fatfs-interface INTERFACE
  "include"
)

target_sources(xpacks-chan-fatfs-interface INTERFACE
    source/ff.c
    source/ffunicode.c
    src/posix-io/chan-fatfs-directory.cpp
    src/posix-io/chan-fatfs-file-sytem.cpp
    src/posix-io/chan-fatfs-file.cpp
    src/posix-io/diskio.cpp
    src/posix-io/ffsystem.cpp
    src/posix-io/utils.cpp
)

target_compile_definitions(xpacks-chan-fatfs-interface INTERFACE
  # None.
)

target_compile_options(xpacks-chan-fatfs-interface INTERFACE
  # None.
)

target_link_libraries(xpacks-chan-fatfs-interface INTERFACE
  # None.
)

if (COMMAND xpack_display_target_lists)
  xpack_display_target_lists(xpacks-chan-fatfs-interface)
endif()

# -----------------------------------------------------------------------------
# Aliases.

# https://cmake.org/cmake/help/v3.20/command/add_library.html#alias-libraries
add_library(xpacks::chan-fatfs ALIAS xpacks-chan-fatfs-interface)
message(VERBOSE "> xpacks::chan-fatfs -> xpacks-chan-fatfs-interface")

# -----------------------------------------------------------------------------

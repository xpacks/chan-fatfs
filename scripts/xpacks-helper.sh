#!/bin/bash
#set -euo pipefail
#IFS=$'\n\t'

# -----------------------------------------------------------------------------
# Bash helper script used in project generate.sh scripts.
# -----------------------------------------------------------------------------

# Optional args: src folders, like posix-io, driver
do_add_chan_fatfs_xpack() {
  local pack_name='chan-fatfs'
  do_tell_xpack "${pack_name}-xpack"

  do_select_pack_folder "ilg/${pack_name}.git"

  do_prepare_dest "${pack_name}/include"
  do_add_content "${pack_folder}/include"/* 
  
  do_prepare_dest "${pack_name}/include/chan-fatfs"
  do_add_content "${pack_folder}/source"/*.h 

  do_prepare_dest "${pack_name}/src"
  do_add_content "${pack_folder}/src/posix-io"
  
  do_prepare_dest "${pack_name}/src/chan-fatfs"
  do_add_content "${pack_folder}/source"/*.c
  
  rm -rf "${dest_folder_path}/diskio.c"
  rm -rf "${dest_folder_path}/ffsystem.c"
}

# -----------------------------------------------------------------------------

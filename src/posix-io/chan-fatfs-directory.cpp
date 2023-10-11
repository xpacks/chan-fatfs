/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2018-2023 Liviu Ionescu. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose is hereby granted, under the terms of the MIT license.
 *
 * If a copy of the license was not distributed with this file, it can
 * be obtained from https://opensource.org/licenses/mit/.
 */

#include <cmsis-plus/posix-io/chan-fatfs-directory.h>

#include "chan-fatfs/utils.h"

#include <string.h>
#include <cerrno>

// ----------------------------------------------------------------------------

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wc++98-compat"
#endif

// ----------------------------------------------------------------------------

namespace os
{
  namespace posix
  {
    // ========================================================================

#pragma GCC diagnostic push
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wweak-template-vtables"
#endif

    // Explicit template instantiation.
    template class directory_implementable<chan_fatfs_directory_impl> ;

#pragma GCC diagnostic pop

    // ========================================================================

    chan_fatfs_directory_impl::chan_fatfs_directory_impl (/* class */ file_system& fs) :
        directory_impl
          { fs }
    {
#if defined(OS_TRACE_POSIX_IO_CHAN_FATFS)
      trace::printf ("chan_fatfs_directory_impl::%s()=@%p\n", __func__, this);
#endif

      memset (&ff_dir_, 0, sizeof(ff_dir_));
    }

    chan_fatfs_directory_impl::~chan_fatfs_directory_impl ()
    {
#if defined(OS_TRACE_POSIX_IO_CHAN_FATFS)
      trace::printf ("chan_fatfs_directory_impl::%s() @%p\n", __func__, this);
#endif
    }

    // ------------------------------------------------------------------------

    /* struct */ dirent*
    chan_fatfs_directory_impl::do_read (void)
    {
      FILINFO fno;

      FRESULT res = f_readdir (&ff_dir_, &fno);
      if (res != FR_OK)
        {
          errno = fatfs_compute_errno (res);
          return nullptr;
        }

      if (fno.fname[0] == '\0')
        {
          // When the end of the directory is encountered, a null pointer
          // shall be returned and errno is not changed.
          return nullptr;
        }

      // Copy the name from the stack FILINFO to the object member.
      strncpy (dir_entry_.d_name, fno.fname, sizeof(dir_entry_.d_name) - 1);
#pragma GCC diagnostic push
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif
      dir_entry_.d_name[sizeof(dir_entry_.d_name) - 1] = '\0';
#pragma GCC diagnostic pop

      return &dir_entry_;
    }

    void
    chan_fatfs_directory_impl::do_rewind (void)
    {
      FRESULT res = f_readdir (&ff_dir_, nullptr);
      if (res != FR_OK)
        {
          errno = fatfs_compute_errno (res);
          return;
        }
      return;
    }

    int
    chan_fatfs_directory_impl::do_close (void)
    {
      FRESULT res = f_closedir (&ff_dir_);

      // TODO: link to deferred list.
      if (res != FR_OK)
        {
          errno = fatfs_compute_errno (res);
          return -1;
        }
      return 0;
    }

  // ========================================================================
  } /* namespace posix */
} /* namespace os */

// ----------------------------------------------------------------------------

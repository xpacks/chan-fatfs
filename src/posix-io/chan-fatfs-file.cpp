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

#include <cmsis-plus/posix-io/chan-fatfs-file.h>
#include <cmsis-plus/posix-io/chan-fatfs-file-system.h>

#include <cmsis-plus/posix/sys/statvfs.h>

#include "chan-fatfs/utils.h"

#include <fcntl.h>
#include <cerrno>
#include <unistd.h>
#include <string.h>

// ----------------------------------------------------------------------------

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wc++98-compat"
#endif
#pragma GCC diagnostic ignored "-Wuseless-cast"

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
    template class file_implementable<chan_fatfs_file_impl> ;

#pragma GCC diagnostic pop

    // ========================================================================

    chan_fatfs_file_impl::chan_fatfs_file_impl (/* class */ file_system& fs) :
        file_impl
          { fs }
    {
#if defined(OS_TRACE_POSIX_IO_CHAN_FATFS)
      trace::printf ("chan_fatfs_file_impl::%s()=@%p\n", __func__, this);
#endif

      memset (&ff_fil_, 0, sizeof(ff_fil_));
    }

    chan_fatfs_file_impl::~chan_fatfs_file_impl ()
    {
#if defined(OS_TRACE_POSIX_IO_CHAN_FATFS)
      trace::printf ("chan_fatfs_file_impl::%s() @%p\n", __func__, this);
#endif
    }

    // ------------------------------------------------------------------------

    bool
    chan_fatfs_file_impl::do_is_opened (void)
    {
      if (ff_fil_.obj.fs != nullptr)
        {
          return true;
        }

      return false;
    }

    // http://pubs.opengroup.org/onlinepubs/9699919799/functions/read.html
    ssize_t
    chan_fatfs_file_impl::do_read (void* buf, std::size_t nbyte)
    {
      UINT br;

      FRESULT res = f_read (&ff_fil_, buf, static_cast<UINT> (nbyte), &br);
      if (res != FR_OK)
        {
          errno = fatfs_compute_errno (res);
          return -1;
        }
      return static_cast<ssize_t> (br);
    }

    // http://pubs.opengroup.org/onlinepubs/9699919799/functions/write.html
    ssize_t
    chan_fatfs_file_impl::do_write (const void* buf, std::size_t nbyte)
    {
      UINT bw;

      FRESULT res = f_write (&ff_fil_, buf, static_cast<UINT> (nbyte), &bw);
      if (res != FR_OK)
        {
          errno = fatfs_compute_errno (res);
          return -1;
        }
      return static_cast<ssize_t> (bw);
    }

    // http://pubs.opengroup.org/onlinepubs/9699919799/functions/fstat.html
    int
    chan_fatfs_file_impl::do_fstat (struct stat* buf __attribute__((unused)))
    {
      // Getting file status of an open file is not implemented.
      errno = ENOSYS;
      return -1;
    }

    // http://pubs.opengroup.org/onlinepubs/9699919799/functions/lseek.html
    off_t
    chan_fatfs_file_impl::do_lseek (off_t offset, int whence)
    {
      if (whence != SEEK_SET || offset < 0)
        {
          errno = EINVAL;
          return -1;
        }

      FRESULT res = f_lseek (&ff_fil_, static_cast<FSIZE_t> (offset));
      if (res != FR_OK)
        {
          errno = fatfs_compute_errno (res);
          return -1;
        }
      return offset;
    }

    // http://pubs.opengroup.org/onlinepubs/9699919799/functions/ftruncate.html
    int
    chan_fatfs_file_impl::do_ftruncate (off_t length)
    {
      // Since f_truncate() has no param, do it in two steps.
      FRESULT res = f_lseek (&ff_fil_, static_cast<FSIZE_t> (length));
      if (res != FR_OK)
        {
          errno = fatfs_compute_errno (res);
          return -1;
        }

      res = f_truncate (&ff_fil_);
      if (res != FR_OK)
        {
          errno = fatfs_compute_errno (res);
          return -1;
        }
      return 0;
    }

    // http://pubs.opengroup.org/onlinepubs/9699919799/functions/fsync.html
    int
    chan_fatfs_file_impl::do_fsync (void)
    {
      FRESULT res = f_sync (&ff_fil_);
      if (res != FR_OK)
        {
          errno = fatfs_compute_errno (res);
          return -1;
        }
      return 0;
    }

    // http://pubs.opengroup.org/onlinepubs/9699919799/functions/close.html
    int
    chan_fatfs_file_impl::do_close (void)
    {
      FRESULT res = f_close (&ff_fil_);

      // TODO: link to deferred list.
      if (res != FR_OK)
        {
          errno = fatfs_compute_errno (res);
          return -1;
        }
      return 0;
    }

  // ==========================--==============================================
  } /* namespace posix */
} /* namespace os */

// ----------------------------------------------------------------------------

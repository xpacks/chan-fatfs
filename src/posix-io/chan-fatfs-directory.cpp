/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2018 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <cmsis-plus/posix-io/chan-fatfs-directory.h>

#include "chan-fatfs/utils.h"

#include <string.h>
#include <cerrno>

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

    chan_fatfs_directory_impl::chan_fatfs_directory_impl (class file_system& fs) :
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

    struct dirent*
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
      dir_entry_.d_name[sizeof(dir_entry_.d_name) - 1] = '\0';

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

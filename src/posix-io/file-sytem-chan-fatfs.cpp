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

#include <cmsis-plus/posix-io/file-system-chan-fatfs.h>
#include <cmsis-plus/diag/trace.h>

#include "chan-fatfs/utils.h"

#include <sys/stat.h>

#include <cerrno>
#include <fcntl.h>

// ----------------------------------------------------------------------------

namespace os
{
  namespace posix
  {
    // ========================================================================

    // Explicit template instantiation.
    template class file_system_implementable<file_system_chan_fatfs_impl> ;

    // ========================================================================

    file_system_chan_fatfs_impl::file_system_chan_fatfs_impl (
        file_system& self, block_device& device) :
        file_system_impl
          { self, device }
    {
#if defined(OS_TRACE_POSIX_IO_CHAN_FATFS)
      trace::printf ("file_system_chan_fatfs_impl::%s()=@%p\n", __func__, this);
#endif
    }

    file_system_chan_fatfs_impl::~file_system_chan_fatfs_impl ()
    {
#if defined(OS_TRACE_POSIX_IO_CHAN_FATFS)
      trace::printf ("file_system_chan_fatfs_impl::%s() @%p\n", __func__, this);
#endif
    }

    // ------------------------------------------------------------------------

    /**
     * @details
     * The Chan FatFS mkfs requires the following mandatory arguments:
     *
     * - int partition
     * - size_t au_bytes
     * - void* work
     * - size_t size
     */
    int
    file_system_chan_fatfs_impl::do_vmkfs (int options, std::va_list args)
    {
      BYTE partition = static_cast<BYTE> (va_arg(args, int));
      BYTE opt = static_cast<BYTE> (options);
      DWORD au_bytes = va_arg(args, size_t);
      void* work = va_arg(args, void*);
      UINT size = va_arg(args, size_t);
      va_end(args);

      FRESULT res;
      res = f_mkfs (&device (), partition, opt, au_bytes, work, size);
      if (res != FR_OK)
        {
          errno = fatfs_compute_errno (res);
          return -1;
        }
      return 0;
    }

    /**
     * @details
     * The Chan FatFS mount accepts the following optionsl arguments:
     *
     * - int volume, 0=auto, parsed if flags include FF_MOUNT_FLAGS_HAS_VOLUME
     */
    int
    file_system_chan_fatfs_impl::do_vmount (unsigned int flags,
                                            std::va_list args)
    {
#if defined(OS_TRACE_POSIX_IO_CHAN_FATFS)
      trace::printf ("file_system_chan_fatfs_impl::%s(%u)\n", __func__, flags);
#endif

      BYTE vol = 0;
      if (flags & FF_MOUNT_FLAGS_HAS_VOLUME)
        {
          vol = static_cast<BYTE> (va_arg(args, int));
        }
      va_end(args);

      FRESULT res;
      res = f_mount (&device (), vol, &ff_fs_);
      if (res != FR_OK)
        {
          errno = fatfs_compute_errno (res);
          return -1;
        }
      return 0;
    }

    int
    file_system_chan_fatfs_impl::do_umount (
        unsigned int flags __attribute__((unused)))
    {
#if defined(OS_TRACE_POSIX_IO_CHAN_FATFS)
      trace::printf ("file_system_chan_fatfs_impl::%s(%u)\n", __func__, flags);
#endif

      FRESULT res;
      res = f_mount (nullptr, 0, &ff_fs_);
      if (res != FR_OK)
        {
          errno = fatfs_compute_errno (res);
          return -1;
        }
      return 0;
    }

    // ------------------------------------------------------------------------

    BYTE
    file_system_chan_fatfs_impl::compute_mode (int oflag)
    {
      BYTE mode = 0;

      if (oflag & O_RDWR)
        {
          // Open for reading and writing.
          mode |= FA_READ;
          mode |= FA_WRITE;
        }
      else if (oflag & O_WRONLY)
        {
          // Open for writing only.
          mode |= FA_WRITE;
        }
      else
        {
          // Open for reading only.
          mode |= FA_READ;
        }

      // O_APPEND - If set, the file offset shall be set to the end of the
      // file prior to each write.
      // O_CREAT - If the file exists, this flag has no effect except as
      // noted under O_EXCL below. Otherwise, if O_DIRECTORY is not set the
      // file shall be created as a regular file;
      // O_TRUNC - If the file exists and is a regular file, and the file
      // is successfully opened O_RDWR or O_WRONLY, its length shall be
      // truncated to 0, and the mode and owner shall be unchanged.
      // O_EXCL - If O_CREAT and O_EXCL are set, open() shall fail if the
      // file exists.
      if (oflag & O_CREAT)
        {
          if (oflag & O_EXCL)
            {
              mode |= FA_CREATE_NEW;
            }
          else if (oflag & O_APPEND)
            {
              mode |= FA_OPEN_APPEND;
            }
          else
            {
              mode |= FA_CREATE_ALWAYS;
            }
        }
      else
        {
          mode |= FA_OPEN_EXISTING;
        }

      return mode;
    }

    // http://pubs.opengroup.org/onlinepubs/9699919799/functions/open.html
    file*
    file_system_chan_fatfs_impl::do_vopen (
        const char* path, int oflag, std::va_list args __attribute__((unused)))
    {
      BYTE mode = compute_mode (oflag);

      file_type* fil = allocate_file<file_type> ();

      FIL* ff_fil = ((file_chan_fatfs_impl&) (fil->impl ())).impl_data ();
      FRESULT res = f_open (&ff_fs_, ff_fil, path, mode);

      if (res != FR_OK)
        {
          errno = fatfs_compute_errno (res);
          return nullptr;
        }

      return fil;
    }

    directory*
    file_system_chan_fatfs_impl::do_opendir (const char* dirname)
    {
      directory_type* dir = allocate_directory<directory_type> ();

      FFDIR* ff_dir = &(((directory_chan_fatfs_impl&) (dir->impl ())).ff_dir_);
      FRESULT res = f_opendir (&ff_fs_, ff_dir, dirname);

      if (res != FR_OK)
        {
          errno = fatfs_compute_errno (res);
          return nullptr;
        }

      return dir;
    }

    // ------------------------------------------------------------------------

    // http://pubs.opengroup.org/onlinepubs/9699919799/functions/chmod.html
    int
    file_system_chan_fatfs_impl::do_chmod (const char* path, mode_t mode)
    {
      BYTE attr = 0;
      BYTE mask = 0;
      // Only the user write bit is tested.
      if ((mode & S_IWUSR) == 0)
        {
          // User write bit is not set; read only.
          attr |= AM_RDO;
          mask |= AM_RDO;
        }

#if defined(OS_TRACE_POSIX_IO_CHAN_FATFS)
      trace::printf ("file_system_chan_fatfs_impl::%s(\"%s\") 0x%02X 0x%02X\n",
                     __func__, path, attr, mask);
#endif

      // POSIX does not define archive, system, hidden files.
      FRESULT res = f_chmod (&ff_fs_, path, attr, mask);
      if (res != FR_OK)
        {
          errno = fatfs_compute_errno (res);
          return -1;
        }
      return 0;
    }

    // http://pubs.opengroup.org/onlinepubs/9699919799/functions/stat.html
    int
    file_system_chan_fatfs_impl::do_stat (const char* path, struct stat* buf)
    {
      FILINFO fno;

      FRESULT res = f_stat (&ff_fs_, path, &fno);
      if (res != FR_OK)
        {
          errno = fatfs_compute_errno (res);
          return -1;
        }

      buf->st_size = static_cast<off_t> (fno.fsize);
      mode_t mode = S_IRUSR;
      if ((fno.fattrib & AM_RDO) == 0)
        {
          mode = S_IWUSR;
        }
      buf->st_mode = mode;

      DWORD mstime = static_cast<DWORD> ((fno.fdate << 16) | (fno.ftime));
      buf->st_mtime = fatfs_from_mstime (mstime);
      return 0;
    }

    // http://pubs.opengroup.org/onlinepubs/9699919799/functions/truncate.html
    int
    file_system_chan_fatfs_impl::do_truncate (
        const char* path __attribute__((unused)),
        off_t length __attribute__((unused)))
    {
      // TODO: open, lseek, truncate, close.
      errno = ENOSYS; // Not implemented
      return -1;
    }

    // http://pubs.opengroup.org/onlinepubs/9699919799/functions/rename.html
    int
    file_system_chan_fatfs_impl::do_rename (const char* existing,
                                            const char* _new)
    {
      FRESULT res = f_rename (&ff_fs_, existing, _new);
      if (res != FR_OK)
        {
          errno = fatfs_compute_errno (res);
          return -1;
        }
      return 0;
    }

    // http://pubs.opengroup.org/onlinepubs/9699919799/functions/unlink.html
    int
    file_system_chan_fatfs_impl::do_unlink (const char* path)
    {
      FILINFO fno;
      FRESULT res = f_stat (&ff_fs_, path, &fno);
      if (res != FR_OK)
        {
          errno = fatfs_compute_errno (res);
          return -1;
        }
      if ((fno.fattrib & AM_DIR) != 0)
        {
          // Fail on folders.
          errno = ENOENT;
          return -1;
        }

      res = f_unlink (&ff_fs_, path);
      if (res != FR_OK)
        {
          errno = fatfs_compute_errno (res);
          return -1;
        }
      return 0;
    }

    // http://pubs.opengroup.org/onlinepubs/9699919799/functions/utime.html
    int
    file_system_chan_fatfs_impl::do_utime (const char* path,
                                           const struct utimbuf* times)
    {
      // FatFS has no access date, only modified date.
      DWORD mstime = fatfs_to_mstime (times->modtime);
      FILINFO fno;
      fno.fdate = static_cast<WORD> (mstime >> 16);
      fno.ftime = static_cast<WORD> (mstime);

#if defined(OS_TRACE_POSIX_IO_CHAN_FATFS)
      trace::printf ("file_system_chan_fatfs_impl::%s(\"%s\") 0x%8X 0x%08X\n",
                     __func__, path, fno.fdate, fno.ftime);
#endif

      FRESULT res = f_utime (&ff_fs_, path, &fno);
      if (res != FR_OK)
        {
          errno = fatfs_compute_errno (res);
          return -1;
        }

      return 0;
    }

    // http://pubs.opengroup.org/onlinepubs/9699919799/functions/mkdir.html
    int
    file_system_chan_fatfs_impl::do_mkdir (const char* path,
                                           mode_t mode __attribute__((unused)))
    {
#if defined(OS_TRACE_POSIX_IO_CHAN_FATFS)
      trace::printf ("file_system_chan_fatfs_impl::%s(\"%s\") 0x%02X 0x%02X\n",
                     __func__, path);
#endif

      FRESULT res = f_mkdir (&ff_fs_, path);
      if (res != FR_OK)
        {
          errno = fatfs_compute_errno (res);
          return -1;
        }
      return 0;
    }

    // http://pubs.opengroup.org/onlinepubs/9699919799/functions/rmdir.html
    int
    file_system_chan_fatfs_impl::do_rmdir (const char* path)
    {
#if defined(OS_TRACE_POSIX_IO_CHAN_FATFS)
      trace::printf ("file_system_chan_fatfs_impl::%s(\"%s\") 0x%02X 0x%02X\n",
                     __func__, path);
#endif

      FILINFO fno;
      FRESULT res = f_stat (&ff_fs_, path, &fno);
      if (res != FR_OK)
        {
          errno = fatfs_compute_errno (res);
          return -1;
        }
      if ((fno.fattrib & AM_DIR) == 0)
        {
          // Fail on files.
          errno = ENOENT;
          return -1;
        }

      res = f_unlink (&ff_fs_, path);
      if (res != FR_OK)
        {
          errno = fatfs_compute_errno (res);
          return -1;
        }
      return 0;
    }

    // http://pubs.opengroup.org/onlinepubs/9699919799/functions/sync.html
    void
    file_system_chan_fatfs_impl::do_sync (void)
    {
#if defined(OS_TRACE_POSIX_IO_CHAN_FATFS)
      trace::printf ("file_system_chan_fatfs_impl::%s()\n", __func__);
#endif

      fs_sync (&ff_fs_);
    }

  // ========================================================================
  } /* namespace posix */
} /* namespace os */

// ----------------------------------------------------------------------------

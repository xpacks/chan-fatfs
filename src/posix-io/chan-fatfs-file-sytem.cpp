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

#include <cmsis-plus/posix-io/chan-fatfs-file-system.h>
#include <cmsis-plus/posix-io/block-device.h>
#include <cmsis-plus/diag/trace.h>

#include "chan-fatfs/utils.h"

#include <sys/stat.h>

#include <cerrno>
#include <fcntl.h>
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
    template class file_system_implementable<chan_fatfs_file_system_impl> ;

#pragma GCC diagnostic pop

    // ========================================================================

    chan_fatfs_file_system_impl::chan_fatfs_file_system_impl (
        block_device& device) :
        file_system_impl
          { device }
    {
#if defined(OS_TRACE_POSIX_IO_CHAN_FATFS)
      trace::printf ("chan_fatfs_file_system_impl::%s()=@%p\n", __func__, this);
#endif
    }

    chan_fatfs_file_system_impl::~chan_fatfs_file_system_impl ()
    {
#if defined(OS_TRACE_POSIX_IO_CHAN_FATFS)
      trace::printf ("chan_fatfs_file_system_impl::%s() @%p\n", __func__, this);
#endif
      if (fs_ != nullptr)
        {
          fs_->deallocate_files<file_type> ();
          fs_->deallocate_directories<directory_type> ();
        }
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
    chan_fatfs_file_system_impl::do_vmkfs (int options, std::va_list args)
    {
      BYTE opt = static_cast<BYTE> (options);
      BYTE partition = static_cast<BYTE> (va_arg(args, int));
      DWORD au_bytes = va_arg(args, size_t);
      void* work = va_arg(args, void*);
      UINT size = static_cast<UINT> (va_arg(args, size_t));
      va_end(args);

      // Guarantee the volume is not mounted.
      ff_fs_.fs_type = 0;

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
    chan_fatfs_file_system_impl::do_vmount (unsigned int flags,
                                            std::va_list args)
    {
#if defined(OS_TRACE_POSIX_IO_CHAN_FATFS)
      trace::printf ("chan_fatfs_file_system_impl::%s(%u)\n", __func__, flags);
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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

    int
    chan_fatfs_file_system_impl::do_umount (unsigned int flags)
    {
#if defined(OS_TRACE_POSIX_IO_CHAN_FATFS)
      trace::printf ("chan_fatfs_file_system_impl::%s(%u)\n", __func__, flags);
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

#pragma GCC diagnostic pop

    // ------------------------------------------------------------------------

    BYTE
    chan_fatfs_file_system_impl::compute_mode (int oflag)
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
    chan_fatfs_file_system_impl::do_vopen (
        /* class */ file_system& fs, const char* path, int oflag,
        std::va_list args __attribute__((unused)))
    {
      fs_ = &fs;
      BYTE mode = compute_mode (oflag);

      file_type* fil = fs.allocate_file<file_type> ();

#pragma GCC diagnostic push
#if defined(__clang__)
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wuseless-cast"
#endif
      FIL* ff_fil =
          (static_cast<chan_fatfs_file_impl&> (fil->impl ())).impl_data ();
#pragma GCC diagnostic pop

      FRESULT res = f_open (&ff_fs_, ff_fil, path, mode);

      if (res != FR_OK)
        {
          errno = fatfs_compute_errno (res);
          return nullptr;
        }

      return fil;
    }

    directory*
    chan_fatfs_file_system_impl::do_opendir (/* class */ file_system& fs,
                                             const char* dirname)
    {
      fs_ = &fs;
      directory_type* dir = fs.allocate_directory<directory_type> ();

#pragma GCC diagnostic push
#if defined(__clang__)
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wuseless-cast"
#endif
      FFDIR* ff_dir =
          &((static_cast<chan_fatfs_directory_impl&> (dir->impl ())).ff_dir_);
#pragma GCC diagnostic pop

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
    chan_fatfs_file_system_impl::do_chmod (const char* path, mode_t mode)
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
      trace::printf ("chan_fatfs_file_system_impl::%s(\"%s\") 0x%02X 0x%02X\n",
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
    chan_fatfs_file_system_impl::do_stat (const char* path, struct stat* buf)
    {
      FILINFO fno;

      FRESULT res = f_stat (&ff_fs_, path, &fno);
      if (res != FR_OK)
        {
          errno = fatfs_compute_errno (res);
          return -1;
        }

      memset (buf, 0, sizeof(struct stat));

      buf->st_size = static_cast<off_t> (fno.fsize);
      mode_t mode = S_IRUSR;
      if ((fno.fattrib & AM_RDO) == 0)
        {
          mode = S_IWUSR;
        }
      if ((fno.fattrib & AM_DIR) != 0)
        {
          mode = S_IFDIR; // Directory
        }
      else
        {
          mode = S_IFREG; // Regular
        }
      buf->st_mode = mode;

      DWORD mstime = static_cast<DWORD> ((fno.fdate << 16) | (fno.ftime));
      buf->st_mtime = fatfs_from_mstime (mstime);

      return 0;
    }

    // http://pubs.opengroup.org/onlinepubs/9699919799/functions/truncate.html
    int
    chan_fatfs_file_system_impl::do_truncate (
        const char* path __attribute__((unused)),
        off_t length __attribute__((unused)))
    {
      // TODO: open, lseek, truncate, close.
      errno = ENOSYS; // Not implemented
      return -1;
    }

    // http://pubs.opengroup.org/onlinepubs/9699919799/functions/rename.html
    int
    chan_fatfs_file_system_impl::do_rename (const char* existing,
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
    chan_fatfs_file_system_impl::do_unlink (const char* path)
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
    chan_fatfs_file_system_impl::do_utime (const char* path,
                                           const /* struct */ utimbuf* times)
    {
      // FatFS has no access date, only modified date.
      DWORD mstime = fatfs_to_mstime (times->modtime);
      FILINFO fno;
      fno.fdate = static_cast<WORD> (mstime >> 16);
      fno.ftime = static_cast<WORD> (mstime);

#if defined(OS_TRACE_POSIX_IO_CHAN_FATFS)
      trace::printf ("chan_fatfs_file_system_impl::%s(\"%s\") 0x%8X 0x%08X\n",
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
    chan_fatfs_file_system_impl::do_mkdir (const char* path,
                                           mode_t mode __attribute__((unused)))
    {
#if defined(OS_TRACE_POSIX_IO_CHAN_FATFS)
      trace::printf ("chan_fatfs_file_system_impl::%s(\"%s\") 0x%02X 0x%02X\n",
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
    chan_fatfs_file_system_impl::do_rmdir (const char* path)
    {
#if defined(OS_TRACE_POSIX_IO_CHAN_FATFS)
      trace::printf ("chan_fatfs_file_system_impl::%s(\"%s\") 0x%02X 0x%02X\n",
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
    chan_fatfs_file_system_impl::do_sync (void)
    {
#if defined(OS_TRACE_POSIX_IO_CHAN_FATFS)
      trace::printf ("chan_fatfs_file_system_impl::%s()\n", __func__);
#endif

      fs_sync (&ff_fs_);
    }

    int
    chan_fatfs_file_system_impl::do_statvfs (struct statvfs* buf)
    {
#if defined(OS_TRACE_POSIX_IO_CHAN_FATFS)
      trace::printf ("chan_fatfs_file_system_impl::%s(%p)\n", __func__, buf);
#endif

      DWORD nclst;
      FRESULT res = f_getfree (&ff_fs_, &nclst);
      if (res != FR_OK)
        {
          errno = fatfs_compute_errno (res);
          return -1;
        }

#pragma GCC diagnostic push
#if defined(__clang__)
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wuseless-cast"
#endif

      buf->f_bsize = device ().block_logical_size_bytes ();
      buf->f_frsize = buf->f_bsize;
      buf->f_blocks = static_cast<fsblkcnt_t> (device ().blocks ());

      // Compute free blocks from free clusters.
      buf->f_bfree = static_cast<fsblkcnt_t> (ff_fs_.free_clst * ff_fs_.csize);
      buf->f_bavail = buf->f_bfree;

#pragma GCC diagnostic pop

      // Count of files not supported (FatFS does not use inodes).
      buf->f_files = 0;
      buf->f_ffree = 0;
      buf->f_favail = buf->f_ffree;

      // The file system ID is derived from a specific pointer.
      buf->f_fsid = reinterpret_cast<unsigned long> (this);

      // Flags not supported.
      buf->f_flag = 0;

      buf->f_namemax = FF_MAX_LFN;
      return 0;
    }

  // ========================================================================
  } /* namespace posix */
} /* namespace os */

// ----------------------------------------------------------------------------

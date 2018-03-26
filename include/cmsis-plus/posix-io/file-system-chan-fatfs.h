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

#ifndef CHAN_FATFS_POSIX_IO_FILE_SYSTEM_CHAN_FATFS_H_
#define CHAN_FATFS_POSIX_IO_FILE_SYSTEM_CHAN_FATFS_H_

#if defined(__cplusplus)

// ----------------------------------------------------------------------------

#include <cmsis-plus/posix-io/file-system.h>
#include <cmsis-plus/posix-io/file-chan-fatfs.h>
#include <cmsis-plus/posix-io/directory-chan-fatfs.h>

#include <chan-fatfs/ff.h>
#include <chan-fatfs/utils.h>

// ----------------------------------------------------------------------------

namespace os
{
  namespace posix
  {
    // ========================================================================

    class file_system_chan_fatfs_impl : public file_system_impl
    {
      // --------------------------------------------------------------------

    public:

      using file_type = file_implementable<file_chan_fatfs_impl>;
      using directory_type = directory_implementable<directory_chan_fatfs_impl>;

      // ----------------------------------------------------------------------
      /**
       * @name Constructors & Destructor
       * @{
       */

    public:

      file_system_chan_fatfs_impl (file_system& self, block_device& device);

      /**
       * @cond ignore
       */

      // The rule of five.
      file_system_chan_fatfs_impl (const file_system_chan_fatfs_impl&) = delete;
      file_system_chan_fatfs_impl (file_system_chan_fatfs_impl&&) = delete;
      file_system_chan_fatfs_impl&
      operator= (const file_system_chan_fatfs_impl&) = delete;
      file_system_chan_fatfs_impl&
      operator= (file_system_chan_fatfs_impl&&) = delete;

      /**
       * @endcond
       */

      virtual
      ~file_system_chan_fatfs_impl ();

      /**
       * @}
       */

      // ----------------------------------------------------------------------
      /**
       * @name Public Member Functions
       * @{
       */

    public:

      // ----------------------------------------------------------------------
      // Implementations.

      /**
       *
       * @param opt
       * @param args (int partition, size_t au_bytes, void* work, size_t size)
       * @return
       */
      virtual int
      do_vmkfs(int options, std::va_list args) override;

      virtual int
      do_vmount (unsigned int flags, std::va_list args) override;

      virtual int
      do_umount (unsigned int flags) override;

      virtual file*
      do_vopen (const char* path, int oflag, std::va_list args) override;

      virtual directory*
      do_opendir (const char* dirname) override;

      virtual int
      do_mkdir (const char* path, mode_t mode) override;

      virtual int
      do_rmdir (const char* path) override;

      virtual void
      do_sync (void) override;

      virtual int
      do_chmod (const char* path, mode_t mode) override;

      virtual int
      do_stat (const char* path, struct stat* buf) override;

      virtual int
      do_truncate (const char* path, off_t length) override;

      virtual int
      do_rename (const char* existing, const char* _new) override;

      virtual int
      do_unlink (const char* path) override;

      virtual int
      do_utime (const char* path, const struct utimbuf* times) override;

      /**
       * @}
       */

      // ----------------------------------------------------------------------
    protected:

      BYTE
      compute_mode (int oflag);

    protected:

      /**
       * @cond ignore
       */

      // Chan FatFS file system status.
      FATFS ff_fs_;

      /**
       * @endcond
       */

    };

    // ========================================================================

    template<typename L>
      class file_system_chan_fatfs_impl_lockable : public file_system_chan_fatfs_impl
      {
        // --------------------------------------------------------------------

      public:

        using lockable_type = L;

        using file_type = file_lockable<file_chan_fatfs_impl, L>;
        using directory_type = directory_lockable<directory_chan_fatfs_impl, L>;

        // ----------------------------------------------------------------------
        /**
         * @name Constructors & Destructor
         * @{
         */

      public:

        file_system_chan_fatfs_impl_lockable (file_system& self,
                                              block_device& device,
                                              lockable_type& locker);

        /**
         * @cond ignore
         */

        // The rule of five.
        file_system_chan_fatfs_impl_lockable (
            const file_system_chan_fatfs_impl_lockable&) = delete;
        file_system_chan_fatfs_impl_lockable (
            file_system_chan_fatfs_impl_lockable&&) = delete;
        file_system_chan_fatfs_impl_lockable&
        operator= (const file_system_chan_fatfs_impl_lockable&) = delete;
        file_system_chan_fatfs_impl_lockable&
        operator= (file_system_chan_fatfs_impl_lockable&&) = delete;

        /**
         * @endcond
         */

        virtual
        ~file_system_chan_fatfs_impl_lockable ();

        /**
         * @}
         */

        // ----------------------------------------------------------------------
        /**
         * @name Public Member Functions
         * @{
         */

      public:

        // ----------------------------------------------------------------------
        // Implementations.

        virtual file*
        do_vopen (const char* path, int oflag, std::va_list args) override;

        virtual directory*
        do_opendir (const char* dirname) override;

        // ----------------------------------------------------------------------

        lockable_type&
        locker (void);

        /**
         * @}
         */

      protected:

        /**
         * @cond ignore
         */

        lockable_type& locker_;

        /**
         * @endcond
         */

      };

    // ========================================================================

    // Explicit template instantiation.
    using file_system_chan_fatfs = file_system_implementable<file_system_chan_fatfs_impl>;

    template<typename L>
      using file_system_chan_fatfs_lockable = file_system_lockable<file_system_chan_fatfs_impl_lockable<L>, L>;

  // ========================================================================
  } /* namespace posix */
} /* namespace os */

// ===== Inline & template implementations ====================================

namespace os
{
  namespace posix
  {
    // ========================================================================

    template<typename L>
      file_system_chan_fatfs_impl_lockable<L>::file_system_chan_fatfs_impl_lockable (
          file_system& self, block_device& device, lockable_type& locker) :
          file_system_chan_fatfs_impl
            { self, device }, //
          locker_ (locker)
      {
#if defined(OS_TRACE_POSIX_IO_CHAN_FATFS)
        trace::printf ("file_system_chan_fatfs_impl_lockable::%s()=@%p\n",
                       __func__, this);
#endif
      }

    template<typename L>
      file_system_chan_fatfs_impl_lockable<L>::~file_system_chan_fatfs_impl_lockable ()
      {
#if defined(OS_TRACE_POSIX_IO_CHAN_FATFS)
        trace::printf ("file_system_chan_fatfs_impl_lockable::%s() @%p\n",
                       __func__, this);
#endif
      }

    template<typename L>
      file*
      file_system_chan_fatfs_impl_lockable<L>::do_vopen (
          const char* path, int oflag,
          std::va_list args __attribute__((unused)))
      {
        BYTE mode = compute_mode (oflag);

        file_type* fil = allocate_file<file_type> (locker_);

        FIL* ff_fil = ((file_chan_fatfs_impl&) (fil->impl ())).impl_data ();
        FRESULT res = f_open (&ff_fs_, ff_fil, path, mode);

        if (res != FR_OK)
          {
            errno = fatfs_compute_errno (res);
            return nullptr;
          }

        return fil;
      }

    template<typename L>
      directory*
      file_system_chan_fatfs_impl_lockable<L>::do_opendir (const char* dirname)
      {
        directory_type* dir = allocate_directory<directory_type> (locker_);

        FFDIR* ff_dir = &(((directory_chan_fatfs_impl&) (dir->impl ())).ff_dir_);
        FRESULT res = f_opendir (&ff_fs_, ff_dir, dirname);

        if (res != FR_OK)
          {
            errno = fatfs_compute_errno (res);
            return nullptr;
          }

        return dir;
      }

    template<typename L>
      inline typename file_system_chan_fatfs_impl_lockable<L>::lockable_type&
      file_system_chan_fatfs_impl_lockable<L>::locker (void)
      {
        return locker_;
      }

  // ========================================================================
  } /* namespace posix */
} /* namespace os */

// ----------------------------------------------------------------------------

#endif /* __cplusplus */

#endif /* CHAN_FATFS_POSIX_IO_FILE_SYSTEM_CHAN_FATFS_H_ */

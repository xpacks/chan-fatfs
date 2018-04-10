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

#ifndef CHAN_FATFS_POSIX_IO_FILE_CHAN_FATFS_H_
#define CHAN_FATFS_POSIX_IO_FILE_CHAN_FATFS_H_

#if defined(__cplusplus)

// ----------------------------------------------------------------------------

#include <cmsis-plus/posix-io/file.h>
#include <chan-fatfs/ff.h>

// ----------------------------------------------------------------------------

namespace os
{
  namespace posix
  {
    class chan_fatfs_file_impl;

    using chan_fatfs_file = file_implementable<chan_fatfs_file_impl>;

    // ========================================================================

    /**
     * @brief File class.
     * @headerfile file.h <cmsis-plus/posix-io/file.h>
     * @ingroup cmsis-plus-posix-io-base
     */
    class chan_fatfs_file_impl : public file_impl
    {
      // ----------------------------------------------------------------------
      /**
       * @name Constructors & Destructor
       * @{
       */

    public:

      chan_fatfs_file_impl (class file_system& fs);

      /**
       * @cond ignore
       */

      // The rule of five.
      chan_fatfs_file_impl (const chan_fatfs_file_impl&) = delete;
      chan_fatfs_file_impl (chan_fatfs_file_impl&&) = delete;
      chan_fatfs_file_impl&
      operator= (const chan_fatfs_file_impl&) = delete;
      chan_fatfs_file_impl&
      operator= (chan_fatfs_file_impl&&) = delete;

      /**
       * @endcond
       */

      virtual
      ~chan_fatfs_file_impl ();

      /**
       * @}
       */

      // ----------------------------------------------------------------------
      /**
       * @name Public Member Functions
       * @{
       */

    public:

      // Implementations
      virtual bool
      do_is_opened (void) override;

      virtual ssize_t
      do_read (void* buf, std::size_t nbyte) override;

      virtual ssize_t
      do_write (const void* buf, std::size_t nbyte) override;

      virtual int
      do_fstat (struct stat* buf) override;

      virtual off_t
      do_lseek (off_t offset, int whence) override;

      virtual int
      do_ftruncate (off_t length) override;

      virtual int
      do_fsync (void) override;

      virtual int
      do_close (void) override;

      FIL*
      impl_data (void);

      /**
       * @}
       */

      // ----------------------------------------------------------------------
    public:

      /**
       * @cond ignore
       */

      // Chan FatFS file status.
      FIL ff_fil_;

      /**
       * @endcond
       */
    };

  // ==========================================================================
  } /* namespace posix */
} /* namespace os */

// ===== Inline & template implementations ====================================

namespace os
{
  namespace posix
  {
    // ========================================================================

    inline FIL*
    chan_fatfs_file_impl::impl_data (void)
    {
      return &ff_fil_;
    }

  // ==========================================================================
  } /* namespace posix */
} /* namespace os */

// ----------------------------------------------------------------------------

#endif /* __cplusplus */

#endif /* CHAN_FATFS_POSIX_IO_FILE_CHAN_FATFS_H_ */

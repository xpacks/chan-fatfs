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

#ifndef CHAN_FATFS_POSIX_IO_DIRECTORY_CHAN_FATFS_H_
#define CHAN_FATFS_POSIX_IO_DIRECTORY_CHAN_FATFS_H_

#if defined(__cplusplus)

// ----------------------------------------------------------------------------

#include <cmsis-plus/posix-io/directory.h>
#include <chan-fatfs/ff.h>

// ----------------------------------------------------------------------------

#pragma GCC diagnostic push

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wc++98-compat"
#endif

namespace os
{
  namespace posix
  {
    // ------------------------------------------------------------------------

    class chan_fatfs_directory_impl;

    using chan_fatfs_directory = directory_implementable<chan_fatfs_directory_impl>;

    // ========================================================================

    class chan_fatfs_directory_impl : public directory_impl
    {
      // ----------------------------------------------------------------------
      /**
       * @name Constructors & Destructor
       * @{
       */

    public:

      chan_fatfs_directory_impl (class file_system& fs);

      /**
       * @cond ignore
       */

      // The rule of five.
      chan_fatfs_directory_impl (const chan_fatfs_directory_impl&) = delete;
      chan_fatfs_directory_impl (chan_fatfs_directory_impl&&) = delete;
      chan_fatfs_directory_impl&
      operator= (const chan_fatfs_directory_impl&) = delete;
      chan_fatfs_directory_impl&
      operator= (chan_fatfs_directory_impl&&) = delete;

      /**
       * @endcond
       */

      virtual
      ~chan_fatfs_directory_impl () override;

      /**
       * @}
       */

      // ----------------------------------------------------------------------
      /**
       * @name Public Member Functions
       * @{
       */

    public:

      // Implementations.

      virtual struct dirent*
      do_read (void) override;

      virtual void
      do_rewind (void) override;

      virtual int
      do_close (void) override;

      /**
       * @}
       */

      // ----------------------------------------------------------------------
    public:

      /**
       * @cond ignore
       */

      // Chan FatFS directory status.
      FFDIR ff_dir_;

      /**
       * @endcond
       */
    };

  // ========================================================================
  } /* namespace posix */
} /* namespace os */

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------

#endif /* __cplusplus */

#endif /* CHAN_FATFS_POSIX_IO_DIRECTORY_CHAN_FATFS_H_ */

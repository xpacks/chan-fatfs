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

namespace os
{
  namespace posix
  {
    // ------------------------------------------------------------------------

    class directory_chan_fatfs_impl;

    using directory_chan_fatfs = directory_implementable<directory_chan_fatfs_impl>;

    // ========================================================================

    class directory_chan_fatfs_impl : public directory_impl
    {
      // ----------------------------------------------------------------------

      /**
       * @cond ignore
       */

      // friend class file_system_chan_fatfs;
      // friend class file_system_impl_chan_fatfs;
      /**
       * @endcond
       */

      // ----------------------------------------------------------------------
      /**
       * @name Constructors & Destructor
       * @{
       */

    public:

      directory_chan_fatfs_impl (directory& self);

      /**
       * @cond ignore
       */

      // The rule of five.
      directory_chan_fatfs_impl (const directory_chan_fatfs_impl&) = delete;
      directory_chan_fatfs_impl (directory_chan_fatfs_impl&&) = delete;
      directory_chan_fatfs_impl&
      operator= (const directory_chan_fatfs_impl&) = delete;
      directory_chan_fatfs_impl&
      operator= (directory_chan_fatfs_impl&&) = delete;

      /**
       * @endcond
       */

      virtual
      ~directory_chan_fatfs_impl ();

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

      // ----------------------------------------------------------------------

      directory&
      self (void);

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

// ----------------------------------------------------------------------------

#endif /* __cplusplus */

#endif /* CHAN_FATFS_POSIX_IO_DIRECTORY_CHAN_FATFS_H_ */

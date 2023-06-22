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

#ifndef CHAN_FATFS_POSIX_IO_FILE_CHAN_FATFS_H_
#define CHAN_FATFS_POSIX_IO_FILE_CHAN_FATFS_H_

#if defined(__cplusplus)

// ----------------------------------------------------------------------------

#include <cmsis-plus/posix-io/file.h>
#include <chan-fatfs/ff.h>

// ----------------------------------------------------------------------------

#pragma GCC diagnostic push

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wc++98-compat"
#endif
#pragma GCC diagnostic ignored "-Wpadded"

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

      chan_fatfs_file_impl (/* class */ file_system& fs);

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
      ~chan_fatfs_file_impl () override;

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

#pragma GCC diagnostic push
#if defined(__clang__)
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wredundant-tags"
#endif
      virtual int
      do_fstat (struct stat* buf) override;
#pragma GCC diagnostic pop

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

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------

#endif /* __cplusplus */

#endif /* CHAN_FATFS_POSIX_IO_FILE_CHAN_FATFS_H_ */

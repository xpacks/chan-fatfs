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
#pragma GCC diagnostic ignored "-Wpadded"

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

      chan_fatfs_directory_impl (/* class */ file_system& fs);

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

      virtual /* struct */ dirent*
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

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

#include "chan-fatfs/utils.h"
#include <errno.h>

// ----------------------------------------------------------------------------

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wc++98-compat"
#endif

// ----------------------------------------------------------------------------

// Not POSIX, there is no official prototype.
extern "C" time_t
timegm (struct tm* tim_p);

// ----------------------------------------------------------------------------

DWORD
fatfs_to_mstime (time_t time)
{
  struct tm tm;
  gmtime_r (&time, &tm);

  WORD fdate = static_cast<WORD> (((tm.tm_year - (1980 - 1900)) << 9)
      | ((tm.tm_mon + 1) << 5) | (tm.tm_mday));
  WORD ftime = static_cast<WORD> (((tm.tm_hour) << 11) | ((tm.tm_min) << 5)
      | (tm.tm_sec / 2));

  return ((static_cast<DWORD> (fdate) << 16) | ftime);
}

time_t
fatfs_from_mstime (DWORD mstime)
{
  WORD fdate = static_cast<WORD> (mstime >> 16);
  WORD ftime = static_cast<WORD> (mstime);

  // http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/time.h.html
  struct tm tm;
  tm.tm_year = ((fdate >> 9) & 0x7F) + (1980 - 1900);
  tm.tm_mon = ((fdate >> 5) & 0x0F) - 1;
  tm.tm_mday = (fdate & 0x1F);

  tm.tm_hour = ((ftime >> 11) & 0x1F);
  tm.tm_min = ((ftime >> 5) & 0x3F);
  tm.tm_sec = (ftime & 0x1F) * 2;

  return timegm (&tm);
}

int
fatfs_compute_errno (FRESULT res)
{
  switch (res)
    {
    case FR_OK:
      return 0;

    case FR_INT_ERR:
    case FR_NOT_READY:
    case FR_INVALID_OBJECT:
    case FR_NOT_ENABLED:
    case FR_LOCKED:
    case FR_INVALID_PARAMETER:
      return EINVAL;

    case FR_MKFS_ABORTED:
      return ECANCELED;

    case FR_DISK_ERR:
      return EIO;

    case FR_DENIED:
    case FR_WRITE_PROTECTED:
      return EACCES;

    case FR_NO_FILE:
    case FR_NO_PATH:
    case FR_INVALID_NAME:
    case FR_INVALID_DRIVE:
      return ENOENT;

    case FR_TOO_MANY_OPEN_FILES:
      return ENFILE;

    case FR_EXIST:
      return EEXIST;

    case FR_NOT_ENOUGH_CORE:
      return ENOMEM;

    case FR_TIMEOUT:
      return EBUSY;

    case FR_NO_FILESYSTEM:
      return ENODATA;
    }
  return ENOTSUP;
}

DWORD
get_fattime ()
{
  return fatfs_to_mstime (time (nullptr));
}

// ----------------------------------------------------------------------------

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

#ifndef CHAN_FATFS_UTILS_H_
#define CHAN_FATFS_UTILS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "ff.h"   /* FatFs lower layer API */

#include <time.h>

  DWORD
  fatfs_to_mstime (time_t time);

  time_t
  fatfs_from_mstime (DWORD mstime);

  int
  fatfs_compute_errno (FRESULT res);

#ifdef __cplusplus
}
#endif

#endif /* CHAN_FATFS_UTILS_H_ */

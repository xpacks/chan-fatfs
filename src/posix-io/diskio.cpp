/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

// OS_USE_MICRO_OS_PLUS
//#include "diskio.h"		/* FatFs lower layer API */
#include "chan-fatfs/diskio.h"   /* FatFs lower layer API */
#include "chan-fatfs/ff.h"   /* FatFs lower layer API */

#include <cmsis-plus/rtos/os.h>
#include <cmsis-plus/posix-io/device-block.h>

#include <time.h>

// ----------------------------------------------------------------------------

/**
 *
 * @param pdrv
 * @return
 * - STA_NOINIT: Indicates that the device has not been initialized and
 * not ready to work. This flag is set on system reset, media removal or
 * failure of disk_initialize function. It is cleared on disk_initialize
 * function succeeded. Any media change that occurs asynchronously must
 * be captured and reflect it to the status flags, or auto-mount function
 * will not work correctly. If the system does not support media change
 * detection, application program needs to explicitly re-mount the volume
 * with f_mount function after each media change.
 * - STA_NODISK: Indicates that no medium in the drive. This is always
 * cleared at fixed disk drive. Note that FatFs does not refer this flag.
 * - STA_PROTECT: Indicates that the medium is write protected. This is
 * always cleared at the drives without write protect function. Not
 * valid if STA_NODISK is set.
 */
DSTATUS
disk_status (PDRV pdrv __attribute__((unused)))
{
  DSTATUS stat = 0;

  return stat;
}

/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS
disk_initialize (PDRV pdrv /* Pointer to block device */
)
{
  DSTATUS stat = 0;

  os::posix::device_block* pdb = (os::posix::device_block*) pdrv;
  int ret = pdb->open ();
  if (ret == -1)
    {
      stat |= STA_NOINIT;
    }

  return stat;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT
disk_read (PDRV pdrv, /* Pointer to block device */
           BYTE *buff, /* Data buffer to store read data */
           DWORD sector, /* Start sector in LBA */
           UINT count /* Number of sectors to read */
           )
{
  os::posix::device_block* pdb = (os::posix::device_block*) pdrv;
  int ret = pdb->read_block (buff, sector, count);
  if (ret > 0)
    {
      return RES_OK;
    }
  return RES_ERROR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT
disk_write (PDRV pdrv, /* Pointer to block device */
            const BYTE *buff, /* Data to be written */
            DWORD sector, /* Start sector in LBA */
            UINT count /* Number of sectors to write */
            )
{
  os::posix::device_block* pdb = (os::posix::device_block*) pdrv;
  int ret = pdb->write_block (buff, sector, count);
  if (ret > 0)
    {
      return RES_OK;
    }
  return RES_ERROR;
}

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT
disk_ioctl (PDRV pdrv, /* Pointer to block device */
            BYTE cmd, /* Control code */
            void *buff /* Buffer to send/receive control data */
            )
{
  DRESULT res = RES_OK;
  os::posix::device_block* pdb = (os::posix::device_block*) pdrv;
  if (cmd == GET_SECTOR_COUNT)
    {
      DWORD* pdw = (DWORD*) buff;
      *pdw = pdb->blocks ();
    }
  else if (cmd == GET_SECTOR_SIZE)
    {
      WORD* pw = (WORD*) buff;
      *pw = (WORD) pdb->block_logical_size_bytes ();
    }
  else if (cmd == GET_BLOCK_SIZE)
    {
      DWORD* pdw = (DWORD*) buff;
      *pdw = pdb->block_physical_size_bytes ();
    }
  else if (cmd == CTRL_SYNC)
    {
      pdb->sync ();
    }
  return res;
}

DSTATUS
disk_deinitialize (PDRV pdrv /* Pointer to block device */
)
{
  DSTATUS stat = 0;

  os::posix::device_block* pdb = (os::posix::device_block*) pdrv;
  int ret = pdb->close ();
  if (ret == -1)
    {
      stat |= STA_NOINIT;
    }

  return stat;
}

// ----------------------------------------------------------------------------

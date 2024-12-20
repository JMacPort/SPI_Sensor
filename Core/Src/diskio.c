/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

extern uint8_t SD_Init(void);
extern uint8_t SD_ReadBlock(uint32_t, uint8_t*);
extern uint8_t SD_WriteBlock(uint32_t, const uint8_t*);

/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

static volatile DSTATUS Stat = STA_NOINIT;

// Only using drive 0; SD card only
DSTATUS disk_initialize(BYTE pdrv) {
    if (pdrv != 0) {
        printf("Init: Invalid drive number\r\n");
        return STA_NOINIT;
    }

    printf("Initializing disk...\r\n");
    if (SD_Init() == 0) {
        printf("Disk init successful\r\n");
        Stat &= ~STA_NOINIT;
        return 0;
    }

    printf("Disk init failed\r\n");
    return STA_NOINIT;
}

DSTATUS disk_status(BYTE pdrv) {
    if (pdrv != 0) return STA_NOINIT;
    return Stat;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(BYTE pdrv, BYTE* buff, LBA_t sector, UINT count) {
    printf("Reading sector %lu, count %u\r\n", sector, count);

    if (pdrv != 0 || !count) {
        printf("Read: Invalid parameters\r\n");
        return RES_PARERR;
    }
    if (Stat & STA_NOINIT) {
        printf("Read: Disk not initialized\r\n");
        return RES_NOTRDY;
    }

    while (count > 0) {
        printf("Reading sector %lu\r\n", sector);
        if (SD_ReadBlock(sector, buff) != 0) {
            printf("Read failed at sector %lu\r\n", sector);
            return RES_ERROR;
        }
        sector++;
        buff += 512;
        count--;
    }
    printf("Read completed successfully\r\n");
    return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write(BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count) {
    printf("Writing sector %lu, count %u\r\n", sector, count);

    if (pdrv != 0 || !count) {
        printf("Write: Invalid parameters\r\n");
        return RES_PARERR;
    }
    if (Stat & STA_NOINIT) {
        printf("Write: Disk not initialized\r\n");
        return RES_NOTRDY;
    }

    while (count > 0) {
        printf("Writing sector %lu\r\n", sector);
        if (SD_WriteBlock(sector, buff) != 0) {
            printf("Write failed at sector %lu\r\n", sector);
            return RES_ERROR;
        }
        sector++;
        buff += 512;
        count--;
    }
    printf("Write completed successfully\r\n");
    return RES_OK;
}

DRESULT disk_ioctl (
    BYTE pdrv,
    BYTE cmd,
    void *buff
)
{
    if (pdrv != 0) return RES_PARERR;
    if (Stat & STA_NOINIT) return RES_NOTRDY;

    DRESULT res = RES_ERROR;

    switch (cmd) {
        case CTRL_SYNC:
            res = RES_OK;
            break;

        case GET_SECTOR_COUNT:
            *(DWORD*)buff = 0x800000;
            res = RES_OK;
            break;

        case GET_SECTOR_SIZE:
            *(WORD*)buff = 512;
            res = RES_OK;
            break;

        case GET_BLOCK_SIZE:
            *(DWORD*)buff = 1;
            res = RES_OK;
            break;
    }

    return res;
}

DWORD get_fattime(void)
{
    return    ((DWORD)(2024 - 1980) << 25)    // Year 2024
            | ((DWORD)1 << 21)                // Month 1
            | ((DWORD)1 << 16)                // Day 1
            | ((DWORD)12 << 11)               // Hour 12
            | ((DWORD)0 << 5)                 // Min 0
            | ((DWORD)0 >> 1);                // Sec 0
}

/*
 * Copyright (c) International Business Machines Corp., 2006
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Author: Artem Bityutskiy
 *
 * UBI (Unsorted Block Images) IO library.
 */

#ifndef __LIBUBIIO_INT_H__
#define __LIBUBIIO_INT_H__

#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define MIN(a ,b) ((a) < (b) ? (a) : (b))
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

/* Verbose messages */
#define verbose(verbose, fmt, ...) \
  do {								   \
    if (verbose)						   \
      printf(PROGRAM_NAME ": " fmt "\n", ##__VA_ARGS__);	   \
  } while(0)

/* Normal messages */
#define normsg(fmt, ...)				   \
	printf(PROGRAM_NAME ": " fmt "\n", ##__VA_ARGS__)

#define normsg_cont(fmt, ...)				\
	printf(PROGRAM_NAME ": " fmt, ##__VA_ARGS__)

/* Error messages */
#define errmsg(fmt, ...)						\
	fprintf(stderr, PROGRAM_NAME ": error!: " fmt "\n", ##__VA_ARGS__)

/* System error messages */
#define sys_errmsg(fmt, ...)						\
  do {									\
    int _err = errno;                                                   \
    size_t _i;								\
    fprintf(stderr, PROGRAM_NAME ": error!: " fmt "\n", ##__VA_ARGS__); \
    for (_i = 0; _i < sizeof(PROGRAM_NAME) + 1; _i++)                   \
      fprintf(stderr, " ");						\
    fprintf(stderr, "error %d (%s)\n", _err, strerror(_err));           \
  } while (0)


/* Warnings */
#define warnmsg(fmt, ...)						\
  fprintf(stderr, PROGRAM_NAME ": warning!: " fmt "\n", ##__VA_ARGS__)

/* debug */
#ifdef CONFIG_LIBUBIO_DEBUG
#define dbgmsg(fmt, ...)						\
  fprintf(stderr, PROGRAM_NAME ": debug!: " fmt "\n", ##__VA_ARGS__)
#else
#define dbgmsg(fmt, ...)	(void) fmt
#endif

/**
 * struct ubi_volume_desc - UBI volume information.
 * @fd: UBI volume file descriptor
 * @mode: volume open mode (%UBI_READONLY, %UBI_READWRITE, %UBI_EXCLUSIVE)
 * @vi: volume info structure
 * @di: device info structure
 */
  struct ubi_volume_desc
  {
    int fd;
    int mode;
    struct ubi_volume_info vi;
    struct ubi_device_info di;
  };

/*
 * The below are pre-define UBI file and directory names.
 *
 * Note, older kernels put 'ubiX_Y' directories straight to '/sys/class/ubi/'.
 * New kernels puts 'ubiX_Y' directories to '/sys/class/ubi/ubiX/', which is
 * saner. And for compatibility reasons it also puts symlinks to 'ubiX_Y'
 * directories to '/sys/class/ubi/'. For now libubi assumes old layout.
 * TODO: use new layout ?
 */

#define SYSFS_UBI         "class/ubi"
#define SYSFS_CTRL        "class/misc/ubi_ctrl"

#define CTRL_DEV          "dev"

#define UBI_VER           "version"
#define UBI_VOL_COUNT     "volumes_count"
#define UBI_DEV_NAME_PATT "ubi%d"

#define DEV_DEV           "dev"
#define DEV_AVAIL_EBS     "avail_eraseblocks"
#define DEV_TOTAL_EBS     "total_eraseblocks"
#define DEV_BAD_COUNT     "bad_peb_count"
#define DEV_EB_SIZE       "eraseblock_size"
#define DEV_MAX_EC        "max_ec"
#define DEV_MAX_RSVD      "reserved_for_bad"
#define DEV_MAX_VOLS      "max_vol_count"
#define DEV_MIN_IO_SIZE   "min_io_size"
#define DEV_MTD_NUM       "mtd_num"

#define UBI_VOL_NAME_PATT "ubi%d_%d"
#define VOL_TYPE          "type"
#define VOL_DEV           "dev"
#define VOL_ALIGNMENT     "alignment"
#define VOL_DATA_BYTES    "data_bytes"
#define VOL_RSVD_BYTES    "resvd_bytes"
#define VOL_RSVD_EBS      "reserved_ebs"
#define VOL_EB_SIZE       "usable_eb_size"
#define VOL_CORRUPTED     "corrupted"
#define VOL_UPD_MARKER    "upd_marker"
#define VOL_CORRUPTED     "corrupted"
#define VOL_NAME          "name"


/**
 * get_sys_dir_path - return the sys directory path
 */
static char *get_sys_dir_path();
/**
 * read_positive_ll - read a positive 'long long' value from a file.
 * @file: the file to read from
 * @value: the result is stored here
 *
 * This function reads file @file and interprets its contents as a positive
 * 'long long' integer. If this is not true, it fails with %EINVAL error code.
 * Returns %0 in case of success and %-1 in case of failure.
 */
static int read_positive_ll(const char *file, long long *value);
/**
 * read_positive_int - read a positive 'int' value from a file.
 * @file: the file to read from
 * @value: the result is stored here
 *
 * This function is the same as 'read_positive_ll()', but it reads an 'int'
 * value, not 'long long'.
 * Returns %0 in case of success and %-1 in case of failure.
 */
static int read_positive_int(const char *file, int *value);
/**
 * read_data - read data from a file.
 * @file: the file to read from
 * @buf: the buffer to read to
 * @buf_len: buffer length
 *
 * This function returns number of read bytes in case of success and %-1 in
 * case of failure. Note, if the file contains more then @buf_len bytes of
 * date, this function fails with %EINVAL error code.
 * Returns %0 in case of success and %-1 in case of failure.
 */
static int read_data(const char *file, void *buf, int buf_len);
/**
 * read_cdev - read major and minor numbers from a file.
 * @file: name of the file to read from
 * @pdev: device decription is returned here
 *
 * Returns %0 in case of succes, and %-1 in case of failure.
 */
static int read_cdev(const char *file, dev_t * pdev);

#ifdef __cplusplus
}
#endif
#endif				/* !__LIBUBIIO_INT_H__ */

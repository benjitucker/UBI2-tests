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
 * ubi (unsorted block images) io library.
 */

#ifndef __LIBUBIIO_H__
#define __LIBUBIIO_H__

/* sys/types.h for dev_t */
#include <sys/types.h>
/* stdint.h for int32/64_t */
#include <stdint.h>
#include <mtd/ubi-user.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* UBI version libubiio is made for */
#define LIBUBIIO_UBI_VERSION	1
#define UBI_VOL_NAME_MAX	127

#include "ubi.h"
  int ubi_get_vol_id_by_name(int ubi_num, const char *name);

#ifdef __cplusplus
}
#endif
#endif				/* !__LIBUBIIO_H__ */

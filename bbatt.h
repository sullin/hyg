/**
 *
 * Very simple "bit-banging" library for the following ATT packet types
 *	Origin: https://github.com/dlenski/ttblue, GPLv3
 *	Modified by Anti Sullin
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* use ATT protocol opcodes from bluez/src/shared/att-types.h */
#include "att-types.h"
#include <bluetooth/bluetooth.h>

int att_connect(bdaddr_t *dst, int sec);

int att_read(int fd, uint16_t handle, void *buf, int len);
int att_write(int fd, uint16_t handle, const void *buf, int length);
int att_wrreq(int fd, uint16_t handle, const void *buf, int length);
int att_read_not(int fd, uint16_t *handle, void *buf, int len);

const char *addr_type_name(int dst_type);
const char *att_ecode2str(uint8_t status); /* copied from bluez/attrib/att.c */

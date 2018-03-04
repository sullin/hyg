/**
 *
 * Xiaomi MI Bluetooth LE Hygrometer reader application
 * (c) Anti Sullin 2018
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/socket.h>
#include "bbatt.h"
#include "hwaddr.h"

void usage(const char *app) {
	printf("Xiaomi Mijia Hygrometer reader utility.\n");
	printf("Usage: %s <bdaddr>\n", app);
}

int main(int argc, char **argv) {
	if (argc != 2) {
		usage(argv[0]);
		return 1;
	}

	bdaddr_t dst_addr = {0};
    if (hwaddr_aton(argv[1], dst_addr.b) < 0) {
        fprintf(stderr, "Invalid device address\n");
        return 1;
    }

    fprintf(stderr, "Connecting...\n");
    int fd = att_connect(&dst_addr, BT_SECURITY_LOW);
    if (fd < 0) {
        if (errno!=ENOTCONN)
            fprintf(stderr, "Failed to connect\n");
        else
        	fprintf(stderr, "Device is not connectable\n");
        return 1;
    }

    fprintf(stderr, "Reading...\n");
    uint8_t val[2] = {0x01, 0x00};
    int ret = att_wrreq(fd, 0x10, val, sizeof(val));
    if (ret < 0) {
		fprintf(stderr, "Could not set device mode\n");
    	close(fd);
    	return 1;
    }

    uint8_t buf[32];
    uint16_t handle = 0;
    int len = att_read_not(fd, &handle, buf, sizeof(buf)-1);
	if (len < 0) {
		fprintf(stderr, "Could not read device information\n");
		return 1;
    }
	if (handle != 0x0e) {
		fprintf(stderr, "Invalid response\n");
		return 1;
	}
	buf[len] = '\0';

	float t=0, h=0;
	if (sscanf(buf, "T=%f H=%f", &t, &h) != 2) {
		fprintf(stderr, "Malformed response\n");
		return 1;
	}

	close(fd);
	printf("temp.value %.1f\n", t);
	printf("hum.value %.1f\n", h);
	return 0;
}

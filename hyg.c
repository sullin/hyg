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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <sys/socket.h>
#include "bbatt.h"
#include "hwaddr.h"

enum sensor_type {
	HYG_LYWSDCGQ,
	HYG_LYWSD02,		// fw pre 1.1.2_0042
	HYG_LYWSD02_0042,	// fw post 1.1.2_0042

	HYG_NUM_SENSOR_TYPES
};

struct lywsd_config {
	uint16_t cccd_handle;
	uint16_t data_handle;
	uint16_t clock_handle;
};

static const struct lywsd_config lywsd_configs[HYG_NUM_SENSOR_TYPES] = {
	[HYG_LYWSDCGQ] = {0x0010, 0x000E, 0},
	[HYG_LYWSD02] = {0x003D, 0x003C, 0x002F},
	[HYG_LYWSD02_0042] = {0x004C, 0x004B, 0x003E},
};

int usage(const char *app) {
	printf("Xiaomi Mijia Hygrometer reader utility.\n");
	printf("Usage: %s [args] <bdaddr>\n", app);
	printf("  -t CGQ       Select LYWSDCGQ sensor (default)\n");
	printf("  -t 02        Select LYWSD002 sensor\n");
	printf("  -t 02_0042   Select LYWSD002 sensor with post 1.1.2_0042 FW\n");
	printf("  -c           Set clock on device\n");
	return 1;
}

int read_notification(int fd, int cccd_handle, int data_handle, uint8_t *buf, int len) {
	uint8_t val[2] = {0x01, 0x00};
	int ret = att_wrreq(fd, cccd_handle, val, sizeof(val));
	if (ret < 0) {
		fprintf(stderr, "Could not enable notifications\n");
		return -1;
	}

	uint16_t handle = 0;
	ret = att_read_not(fd, &handle, buf, len);
	if (ret < 0) {
		fprintf(stderr, "Could not read notification\n");
		return -1;
	}
	if (handle != data_handle) {
		fprintf(stderr, "Invalid response\n");
		return -1;
	}
	return ret;
}

bool lywsd_read_data(int fd, enum sensor_type type) {
	uint8_t buf[32];
	int ret = read_notification(fd, lywsd_configs[type].cccd_handle,
		lywsd_configs[type].data_handle, buf, sizeof(buf));

	if (ret < 0) return false;

	float t=0, h=0;
	switch (type) {
	case HYG_LYWSDCGQ:
		buf[sizeof(buf)-1] = '\0';
		if (sscanf(buf, "T=%f H=%f", &t, &h) != 2) {
			fprintf(stderr, "Malformed response\n");
			return 1;
		}
		break;
	case HYG_LYWSD02:
	case HYG_LYWSD02_0042:
		if (ret != 3) {
			fprintf(stderr, "Malformed response\n");
			return false;
		}
		t = (buf[0] + buf[1]*256.0)/100.0;
		h = buf[2];
	}

	printf("temp.value %.1f\n", t);
	printf("hum.value %.1f\n", h);
	return true;
}

bool lywsd_set_time(int fd, enum sensor_type type) {
	if (lywsd_configs[type].clock_handle == 0) {
		fprintf(stderr, "Setting time not supported\n");
		return false;
	}

	time_t t = time(NULL);
	struct tm lt = {0};
	localtime_r(&t, &lt);

	uint8_t buf[5];
	buf[0] = t & 0xFF;
	buf[1] = (t >> 8) & 0xFF;
	buf[2] = (t >> 16) & 0xFF;
	buf[3] = (t >> 24) & 0xFF;
	buf[4] = (int8_t)(lt.tm_gmtoff / 3600);

	int ret = att_wrreq(fd, lywsd_configs[type].clock_handle,
		buf, sizeof(buf));
	if (ret < 0) {
		fprintf(stderr, "Could not set time\n");
		return false;
	}
	return true;
}

int main(int argc, char **argv) {
	enum sensor_type type = HYG_LYWSDCGQ;
	int opt;
	bool set_clock = false;

	while ((opt = getopt(argc, argv, "t:c")) != -1) {
		switch (opt) {
		case 't':
			if (strcmp(optarg, "CGQ") == 0) type = HYG_LYWSDCGQ;
			else if (strcmp(optarg, "02") == 0) type = HYG_LYWSD02;
			else if (strcmp(optarg, "02_0042") == 0) type = HYG_LYWSD02_0042;
			else return usage(argv[0]);
			break;
		case 'c':
			set_clock = true;
			break;
		default:
			return usage(argv[0]);
		}
	}

	if (optind >= argc) {
		return usage(argv[0]);
	}

	bdaddr_t dst_addr = {0};
	if (hwaddr_aton(argv[optind], dst_addr.b) < 0) {
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

	bool ret = true;
	if (set_clock) {
		fprintf(stderr, "Setting time...\n");
		ret &= lywsd_set_time(fd, type);
	}

	fprintf(stderr, "Reading data...\n");
	ret &= lywsd_read_data(fd, type);
	close(fd);

	return ret?0:1;
}

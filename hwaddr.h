/*
 * MAC address string functions, from wpa_supplicant
 * Copyright (c) 2018 Anti Sullin
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
 *
 */

#ifndef HWADDR_H_
#define HWADDR_H_

#include <stdint.h>

int hwaddr_aton(const char *txt, uint8_t *addr);
int hwaddr_ntoa(const uint8_t *addr, char *str);


#endif /* HWADDR_H_ */

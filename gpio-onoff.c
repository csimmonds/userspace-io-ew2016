/*
 * Copyright (c) 2016 Chris Simmonds, chris@2net.co.uk
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* This demonstrates the use of two gpios, one as input and the other as
   output. The input is triggered on an interrupt.

   For testing, use a BeagleBone Black with the default Debinan Linux.
   Mine is runing version 7.4 (cat /etc/debian.version).

   Connect an output (e.g. an LED) to P9_16 (GPIO 51) and connect the
   input (e.g. a push button switch) to P9_12 (GPIO 60).

   The script config_gpio.sh will configure them as needed.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>

#define GPIO_IN_PATH  "/sys/class/gpio/gpio60/value"
#define GPIO_OUT_PATH "/sys/class/gpio/gpio51/value"

int main (int argc, char *argv[])
{
	int f_in;
	int f_out;
	struct pollfd poll_fds [1];
	char buf [4];
	char out = '0';

	f_in = open(GPIO_IN_PATH, O_RDONLY);
	if (f_in == -1) {
		printf("Failed to open %s\n", GPIO_IN_PATH);
		exit(1);
	}
	f_out = open(GPIO_OUT_PATH, O_WRONLY);
	if (f_out == -1) {
		printf("Failed to open %s\n", GPIO_OUT_PATH);
		exit(1);
	}

	poll_fds[0].fd = f_in;
	poll_fds[0].events = POLLPRI | POLLERR;

	while (1) {
		printf("polling\n");
		if (poll(poll_fds, 1, -1) == 1) {
			printf("polled\n");
			read(f_in, buf, sizeof(buf));
			if (out == '0')
				out = '1';
			else
				out = '0';

			write(f_out, &out, 1);
		}
	}

	return 0;
}


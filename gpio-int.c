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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>

static void  wait_for_int (int f)
{
	struct pollfd poll_fds [1];
	int ret;
	char value[4];
	int n;

	printf("Waiting\n");

	poll_fds[0].fd = f;
	poll_fds[0].events = POLLPRI | POLLERR;

	ret = poll(poll_fds, 1, -1);
	if (ret > 0) {
		n = read(f, &value, sizeof(value));
		lseek(f, 0, SEEK_SET);
		printf("read %d bytes, value=%c\n", n, value[0]);
	}
}

int main (int argc, char *argv[])
{
	int f;
	/* Default gpio, 60, is P9_12 on BBB */
	char *gpio_dir = "/sys/class/gpio/gpio60";
	char gpio_value_path [128];

	if (argc == 2)
		gpio_dir = argv[1];

	sprintf(gpio_value_path, "%s/value", gpio_dir);
	f = open(gpio_value_path, O_RDONLY);
	if (f == -1) {
		printf("Failed to open %s\n", gpio_value_path);
		exit(1);
	}

	while (1)
		wait_for_int(f);

	return 0;
}


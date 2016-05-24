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

/* This demonstrates how to use the input from an i2c sensor to control
   the brightness of an LED.
   
   For testing, use a BeagleBone Black with the default Debinan Linux.
   Mine is runing version 7.4 (cat /etc/debian.version).

   The sensor is a TSL2651 luminosity sensor from Adafruit:
   https://www.adafruit.com/products/439?&main_page=product_info&products_id=439

   In my example, it is connected to controller I2C2 via these pins:

   P9_19 is I2C2_SCL
   P9_20 is I2C2_SDA

   For the LED output, I am using EHRPWM2 channel B, which is routed
   by the device tree overlays to P8_13. It is pwm6.

   The script config_pwm.sh will configure them as needed.
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

/* i2c address of chip */
#define TSL2651_ADDR 0x39

/* Registers */
#define TSL2561_REGISTER_CONTROL 0x00
#define TSL2561_REGISTER_ID      0x0a
#define TSL2561_REGISTER_CHAN0   0x0c
#define TSL2561_REGISTER_CHAN1   0x0e

/* command modifiers */
#define TSL2561_COMMAND_BIT 0x80
#define TSL2561_WORD_BIT    0x20

int main(int argc, char **argv)
{
	int f;
	int f_pwm;
	char buf[8];
	int light;
	char pwm_duty[32];

	f = open("/dev/i2c-1", O_RDWR);
	if (f == -1) {
		perror("Cannot open i2c device");
		return 1;
	}
	if (ioctl(f, I2C_SLAVE, TSL2651_ADDR) < 0) {
		perror("ioctl I2C_SLAVE");
		return 1;
	}
	f_pwm = open("/sys/class/pwm/pwm6/duty_ns", O_WRONLY);
	if (f_pwm == -1) {
		perror("/sys/class/pwm/pwm6/duty_ns");
		return 1;
	}

	buf[0] = TSL2561_REGISTER_ID | TSL2561_COMMAND_BIT;
	if (write(f, buf, 1) != 1) {
		perror("write");
		return 1;
	}
	if (read(f, buf, 1) != 1) {
		perror("read");
		return 1;
	}
	printf("ID 0x%x\n", buf [0]);

	/* Turn on light sensor */
	buf[0] = TSL2561_REGISTER_CONTROL | TSL2561_COMMAND_BIT;
	buf[1] = 0x03;
        if (write(f, buf, 2) != 2) {
                perror("write");
                return 1;
        }

	while (1) {
		usleep(500000);

		/* Get light sensor reading for chanel 0 (visible + IR) */
		buf[0] = TSL2561_REGISTER_CHAN0 | TSL2561_WORD_BIT |
			 TSL2561_COMMAND_BIT;
		if (write(f, buf, 1) != 1) {
			perror("write");
			return 1;
		}
		if (read(f, buf, 2) != 2) {
			perror("read");
			return 1;
		}
		/* printf("0x%x 0x%x\n", buf[1], buf[0]); */
		light = (buf[1] << 8) + buf[0];
		/* printf("Light 0x%x\n", light); */

		/* Adjust the LED's brightness in coordination with
		   ambient light registered by the sensor */

		/* The pwm range is 0 (off) to 1000000 (on), but
		   in reality, values above 200000 make little differnece
		   in the LED brighness.
		   The sensor returns numbers in the range 0 to 12000.
		   To convert between them, I simply multiply by scaling
		   factor of 10. I could permorm a more sophisticated
		   calculation, but for a simple demo, this works fine.
		 */

		sprintf(pwm_duty, "%d", light * 10);
		write(f_pwm, pwm_duty, strlen(pwm_duty));
	}

	close(f);
	return 0;
}

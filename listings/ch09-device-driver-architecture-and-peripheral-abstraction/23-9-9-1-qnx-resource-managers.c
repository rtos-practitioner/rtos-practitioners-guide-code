/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 9: Device Driver Architecture and Peripheral Abstraction
 * 9.9 QNX and Zephyr Portability Notes > 9.9.1 QNX: Resource Managers
 * Verification: not run (host command / other RTOS / configuration)
 * SPDX-License-Identifier: MIT
 */
/* QNX application code */
int fd = open("/dev/ser1", O_RDWR);
tcsetattr(fd, TCSANOW, &termios_config);
write(fd, data, len);
read(fd, buf, len);
devctl(fd, DCMD_CHR_LINESTATUS, &status, sizeof(status), NULL);
close(fd);

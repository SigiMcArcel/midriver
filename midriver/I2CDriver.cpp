#include "I2CDriver.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

int miDriver::I2CDriver::open()
{
	return ::open(I2CPath.c_str(), O_RDWR);
}

void miDriver::I2CDriver::close(int fd)
{
	::close(fd);
}

int miDriver::I2CDriver::I2CRead(int address, int len, unsigned char* data)
{
	int result = 0;
	int fd = open();
	if (fd < 0)
	{
		return fd;
	}
	result = ::ioctl(fd, I2C_SLAVE, address);
	if (result < 0)
	{
		close(fd);
		return result;
	}
	result = (int)::read(fd, data, len);
	if (result != len)
	{
		close(fd);
		return 0;
	}
	close(fd);
	return len;
}

int miDriver::I2CDriver::I2CWrite(int address, int len, unsigned char* data)
{
	int result = 0;
	int fd = open();
	if (fd < 0)
	{
		return fd;
	}
	result = ::ioctl(fd, I2C_SLAVE, address);
	if (result < 0)
	{
		close(fd);
		return result;
	}
	result = (int)::write(fd, data, len);
	if (result != len)
	{
		close(fd);
		return 0;
	}
	close(fd);
	return len;
}

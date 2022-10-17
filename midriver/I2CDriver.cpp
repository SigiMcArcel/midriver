#include "I2CDriver.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

miDriver::DriverResults miDriver::I2CDriver::open()
{
	if (::open(I2CPath.c_str(), O_RDWR) > -1)
	{
		return miDriver::DriverResults::Ok;
	}
	return miDriver::DriverResults::ErrorOpen;
}

void miDriver::I2CDriver::close(int fd)
{
	::close(fd);
}

miDriver::DriverResults miDriver::I2CDriver::I2CRead(int address, int len, unsigned char* data)
{
	int result = 0;
	int fd = open();
	if (fd < 0)
	{
		return miDriver::DriverResults::ErrorRead;
	}
	result = ::ioctl(fd, I2C_SLAVE, address);
	if (result < 0)
	{
		close(fd);
		return miDriver::DriverResults::ErrorRead;
	}
	result = (int)::read(fd, data, len);
	if (result != len)
	{
		close(fd);
		return miDriver::DriverResults::ErrorRead;
	}
	close(fd);
	return miDriver::DriverResults::Ok;
}

miDriver::DriverResults miDriver::I2CDriver::I2CWrite(int address, int len, unsigned char* data)
{
	int result = 0;
	int fd = open();
	if (fd < 0)
	{
		return miDriver::DriverResults::ErrorRead;
	}
	result = ::ioctl(fd, I2C_SLAVE, address);
	if (result < 0)
	{
		close(fd);
		return miDriver::DriverResults::ErrorWrite;
	}
	result = (int)::write(fd, data, len);
	if (result != len)
	{
		close(fd);
		return miDriver::DriverResults::ErrorWrite;
	}
	close(fd);
	return miDriver::DriverResults::Ok;
}

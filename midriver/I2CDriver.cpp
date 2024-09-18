#include "I2CDriver.h"
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>


miDriver::DriverResults miDriver::I2CDriver::open()
{
	int result = 0;
	if (_Handle != -1)
	{
		return miDriver::DriverResults::ErrorAllreadyOpen;
	}
	_Handle = ::open(_I2CPath.c_str(), O_RDWR);
	if(_Handle == -1)
	{
		printf("%s error %s %s\n", strerror(errno),__func__ _I2CPath.c_str());
		return miDriver::DriverResults::ErrorOpen;
	}
	result = ::ioctl(_Handle, I2C_SLAVE, _Address);
	if (result == -1)
	{
		printf("%s error %s %s\n", strerror(errno), __func__ _I2CPath.c_str());
		return miDriver::DriverResults::ErrorOpen;
	}
	return miDriver::DriverResults::Ok;
}

void miDriver::I2CDriver::close()
{
	::close(_Handle);
	_Handle = -1;
}

miDriver::DriverResults miDriver::I2CDriver::read(int len, unsigned char* data)
{
	int result = 0;
	if (_Handle == -1)
	{
		return miDriver::DriverResults::ErrorNotOpen;
	}
	result = (int)::read(_Handle, data, len);
	if (result != len)
	{
		printf("miDriver::I2CDriver::read  error %s adresse 0x%x\n", strerror(errno),_Address);
		return miDriver::DriverResults::ErrorRead;
	}
	return miDriver::DriverResults::Ok;
}

miDriver::DriverResults miDriver::I2CDriver::write(int len, unsigned char* data)
{
	int result = 0;
	if (_Handle == -1)
	{
		return miDriver::DriverResults::ErrorWrite;
	}
	
	result = (int)::write(_Handle, data, len);
	if (result == -1)
	{
		printf("miDriver::I2CDriver::write  error %s\n", strerror(errno));
		return miDriver::DriverResults::ErrorWrite;
	}
	return miDriver::DriverResults::Ok;
}

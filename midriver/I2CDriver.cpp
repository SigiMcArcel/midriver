#include "I2CDriver.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>


miDriver::DriverResults miDriver::I2CDriver::open()
{
	int result = 0;
	if (_Handle != _OpenErrorResult)
	{
		return miDriver::DriverResults::ErrorAllreadyOpen;
	}
	_Handle = ::open(_I2CPath.c_str(), O_RDWR);
	if(_Handle != _OpenErrorResult)
	{
		return miDriver::DriverResults::ErrorOpen;
	}
	result = ::ioctl(_Handle, I2C_SLAVE, _Address);
	if (result == _OpenErrorResult)
	{
		return miDriver::DriverResults::ErrorOpen;
	}
	return miDriver::DriverResults::Ok;
}

void miDriver::I2CDriver::close()
{
	::close(_Handle);
	_Handle == -1;
}

miDriver::DriverResults miDriver::I2CDriver::I2CRead(int len, unsigned char* data)
{
	int result = 0;
	if (_Handle == _OpenErrorResult)
	{
		return miDriver::DriverResults::ErrorNotOpen;
	}
	result = ::ioctl(_Handle, I2C_SLAVE, _Address);
	
	result = (int)::read(_Handle, data, len);
	if (result != len)
	{
		return miDriver::DriverResults::ErrorRead;
	}
	return miDriver::DriverResults::Ok;
}

miDriver::DriverResults miDriver::I2CDriver::I2CWrite(int len, unsigned char* data)
{
	int result = 0;
	if (_Handle == _OpenErrorResult)
	{
		return miDriver::DriverResults::ErrorRead;
	}
	result = (int)::write(_Handle, data, len);
	if (result != len)
	{
		return miDriver::DriverResults::ErrorWrite;
	}
	return miDriver::DriverResults::Ok;
}

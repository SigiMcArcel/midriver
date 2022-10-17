#pragma once
#include <string>
#include "DriverBase.h"

namespace miDriver
{
	class I2CDriver
	{
	private:
		const std::string I2CPath = "/dev/i2c-1";

		miDriver::DriverResults open();
		void close(int fd);
	public:
		I2CDriver()
		{

		}
		miDriver::DriverResults I2CRead(int address, int len, unsigned char* data);
		miDriver::DriverResults I2CWrite(int address, int len, unsigned char* data);

	};
}


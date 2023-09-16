#pragma once
#include <string>
#include "DriverBase.h"

namespace miDriver
{
	class I2CDriver
	{
	private:
		const int _OpenErrorResult = -1;
		const std::string I2CDefaultPath = "/dev/i2c-1";
		std::string _I2CPath;
		int _Handle;
		int _Address;

		
	public:
		I2CDriver(int address)
			:_I2CPath(I2CDefaultPath)
			,_Handle(_OpenErrorResult)
			,_Address(address)
		{
			
		}
		I2CDriver(std::string dev,int address)
		:_I2CPath(dev)
		, _Handle(_OpenErrorResult)
		, _Address(address)
		{

		}
		miDriver::DriverResults open();
		void close();
		miDriver::DriverResults I2CRead(int len, unsigned char* data);
		miDriver::DriverResults I2CWrite(int len, unsigned char* data);

	};
}


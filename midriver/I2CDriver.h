#pragma once
#include <string>
#include <mi/midriver/DriverBase.h>

namespace miDriver
{
	class I2CDriver
	{
	private:
		std::string _I2CPath;
		int _Handle;
		int _Address;
		

	public:
		I2CDriver(int address)
			:_I2CPath("/dev/i2c-1")
			,_Handle(-1)
			,_Address(address)
		{
			
		}
		I2CDriver(std::string dev,int address)
		:_I2CPath(dev)
		, _Handle(-1)
		, _Address(address)
		{

		}
		miDriver::DriverResults open();
		void close();
		miDriver::DriverResults read(int len, unsigned char* data);
		miDriver::DriverResults write(int len, unsigned char* data);

	};
}


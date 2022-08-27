#pragma once
#include <string>
namespace miDriver
{
	class I2CDriver
	{
	private:
		const std::string I2CPath = "/dev/i2c-1";

		int open();
		void close(int fd);
	public:
		I2CDriver()
		{

		}
		int I2CRead(int address, int len, unsigned char* data);
		int I2CWrite(int address, int len, unsigned char* data);

	};
}


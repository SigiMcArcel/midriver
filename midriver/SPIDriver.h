#pragma once
#include <string>
#include "DriverBase.h"

namespace miDriver
{
	typedef enum SPIModes_e
	{
		SPIMode0,
		SPIMode1,
		SPIMode2,
		SPIMode3
	}SPIModes;
	typedef enum SPIDevices_e
	{
		SPIDev0,
		SPIDev1
	}SPIDevices;
	typedef enum SPIBitsPerWord_e
	{
		SPI8Bits = 8,
		SPI116Bits = 16
	}SPIBitsPerWord;

	class SPIDriver
	{
	private:
		
		SPIModes _Mode;
		int _Speed;
		SPIBitsPerWord _Bits;
		std::string _Device;
		int _Fd;

		int configure(int fd);
	public:
		SPIDriver(const std::string& device)
			:_Mode(SPIModes::SPIMode0)
			, _Speed(1000000)
			, _Bits(SPIBitsPerWord::SPI8Bits)
			, _Device(device)
			, _Fd(-1)
		{

		}
		SPIDriver(SPIModes mode,int speed, SPIBitsPerWord bits, const std::string& device)
			:_Mode(mode)
			, _Speed(speed)
			, _Bits(bits)
			, _Device(device)
			, _Fd(-1)
		{

		}
		miDriver::DriverResults open();
		miDriver::DriverResults close();
		miDriver::DriverResults read(int address, int len, unsigned char* data);
		miDriver::DriverResults write(int address, int len, unsigned char* data);
		miDriver::DriverResults transfer(int address, int len, unsigned char* txdata, unsigned char* rxdata);
	};
}


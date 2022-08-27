#pragma once
#include <string>

namespace midriver
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
		const std::string SPI0Path = "/dev/spidev0.0";
		const std::string SPI1Path = "/dev/spidev0.1";

		SPIModes _Mode;
		int _Speed;
		SPIBitsPerWord _Bits;
		SPIDevices _Device;

		int open();
		void close(int fd);
		int configure(int fd);
	public:
		SPIDriver()
			:_Mode(SPIModes::SPIMode0)
			, _Speed(1000000)
			, _Bits(SPIBitsPerWord::SPI8Bits)
			, _Device(SPIDevices::SPIDev0)
		{

		}
		SPIDriver(SPIModes mode,int speed, SPIBitsPerWord bits, SPIDevices device)
			:_Mode(mode)
			, _Speed(speed)
			, _Bits(bits)
			, _Device(device)
		{

		}
		int SPIRead(int address, int len, unsigned char* data);
		int SPIWrite(int address, int len, unsigned char* data);
		int SPITransfer(int address, int len, unsigned char* txdata, unsigned char* rxdata);
	};
}


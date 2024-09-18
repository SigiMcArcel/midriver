#include "GPIODriver.h"
#include <string>
#include <iostream>                             // Standardstream-Funktionaliät einbinden
#include <fstream> 
#include <cerrno>
#include <cstring>

using namespace miDriver;

miDriver::DriverResults miDriver::GPIODriver::GpioEnable(int32_t pin, bool enable)
{
	std::fstream stream;
	std::string strPin = std::to_string(pin).c_str();
	std::string cmd = "/export";
	if (!enable)
	{
		cmd = "/unexport";
	}
	stream.open(gpioPath + std::string("/export"),std::fstream::out);
	if (stream.fail())
	{
		std::cerr << __PRETTY_FUNCTION__ << " Error: " << std::strerror(errno) << std::endl;
		return DriverResults::ErrorOpen;
	}
	stream.write(strPin.c_str(), strPin.size());
	if (stream.fail())
	{
		std::cerr << __PRETTY_FUNCTION__ << " Error: " << std::strerror(errno) << std::endl;
		stream.close();
		return DriverResults::ErrorOpen;
	}
	stream.close();
	return DriverResults::Ok;
}



miDriver::DriverResults miDriver::GPIODriver::GpioSetDirection(int32_t pin, miDriver::GPIODirection direction)
{
	std::fstream stream;
	std::string strCmd = string_format("%s/gpio%d/direction", gpioPath.c_str(), pin);
	std::string strVal = "in";
	if (direction == GPIODirection::Output)
	{
		strVal = "out";
	}

	stream.open(strCmd, std::fstream::out);
	if (stream.fail())
	{
		std::cerr << __PRETTY_FUNCTION__ << " Error: " << std::strerror(errno) << strCmd << std::endl;
		return DriverResults::ErrorOpen;

	}
	stream.write(strVal.c_str(), strVal.size());
	if (stream.fail())
	{
		std::cerr << __PRETTY_FUNCTION__ << " Error: " << std::strerror(errno) << std::endl;
		stream.close();
		return DriverResults::ErrorWrite;
	}
	stream.close();
	return DriverResults::Ok;
}

bool miDriver::GPIODriver::GpioRead(int32_t pin, DriverResults* result)
{
	std::fstream stream;
	std::string strCmd = string_format("%s/gpio%d/value", gpioPath.c_str(), pin);
	std::string strVal = "";
	int length = (int)gpioValueResultSize;
	char* buffer = new char[length];

	stream.open(strCmd, std::fstream::in);
	if (stream.fail())
	{
		std::cerr << __PRETTY_FUNCTION__ << " Error: " << std::strerror(errno) << strCmd << std::endl;
		if (result != nullptr)
			*result = DriverResults::ErrorOpen;

		return false;
	}

	stream.read(buffer, length);
	if (stream.fail())
	{ 
		std::cerr << __PRETTY_FUNCTION__ << " Error: " << std::strerror(errno) << strCmd << std::endl;
		stream.close();
		if (result != nullptr)
			*result = DriverResults::ErrorRead;

		stream.close();
		return false;
	}
	stream.close();
	if (buffer[0] == '1')
	{
		return true;
	}
	return false;
}

miDriver::DriverResults miDriver::GPIODriver::GpioWrite(int32_t pin, bool value)
{
	std::fstream stream;
	std::string strCmd = string_format("%s/gpio%d/value", gpioPath.c_str(), pin);
	std::string strVal = "0";
	if (value)
	{
		strVal = "1";
	}

	stream.open(strCmd, std::fstream::out);
	if (stream.fail())
	{
		std::cerr << __PRETTY_FUNCTION__ << " Error: " << std::strerror(errno) << strCmd << std::endl;
		return DriverResults::ErrorOpen;
	}
	stream.write(strVal.c_str(), strVal.size());
	if (stream.fail())
	{
		std::cerr << __PRETTY_FUNCTION__ << " Error: " << std::strerror(errno) << strCmd << std::endl;
		stream.close();
		return DriverResults::ErrorWrite;
	}
	stream.close();
	return DriverResults::Ok;
}

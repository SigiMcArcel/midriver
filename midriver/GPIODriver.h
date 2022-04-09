#pragma once
#include "DriverBase.h"

namespace miDriver
{
	class GPIODriver
	{
	private:
		const std::string gpioPath = "/sys/class/gpio";
		const size_t gpioValueResultSize = 1;

	public:
		miDriver::DriverResults GpioEnable(int32_t pin, bool enable);
		miDriver::DriverResults GpioSetDirection(int32_t pin, int32_t direction);

		bool GpioRead(int32_t pin, DriverResults* result = nullptr);
		miDriver::DriverResults GpioWrite(int32_t pin, bool value);

	};
}


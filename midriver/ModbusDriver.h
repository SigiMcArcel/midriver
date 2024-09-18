#pragma once
#include "DriverBase.h"
#include <modbus/modbus.h>

namespace miDriver
{
	enum class ModbusDriverAccessType_e : unsigned char
	{
		BIT_TO_BYTE,
		BITS
	};

	class ModbusDriver
	{
	private:
		std::string _Device;
		modbus_t* _Handle;
		

	protected:
		ModbusDriver()
			:_Device("/dev/ttyUSB0")
			,_Handle(nullptr)
		{
			
		}
		ModbusDriver(const std::string& dev)
			:_Device(dev)
			,_Handle(nullptr)
		{

		}
		static ModbusDriver* _ModbusDriverInstance;

	public:
		ModbusDriver(ModbusDriver& other) = delete;
		/**
		 * Singletons should not be assignable.
		 */
		void operator=(const ModbusDriver&) = delete;
		/**
		 * This is the static method that controls the access to the singleton
		 * instance. On the first run, it creates a singleton object and places it
		 * into the static field. On subsequent runs, it returns the client existing
		 * object stored in the static field.
		 */

		static ModbusDriver* GetInstance(const std::string& dev)
		{
			if (_ModbusDriverInstance == nullptr)
			{
				_ModbusDriverInstance = new ModbusDriver(dev);
			}
			return _ModbusDriverInstance;
		}

		miDriver::DriverResults open();
		void close();
		miDriver::DriverResults readInputBits(int address,int count, ModbusDriverAccessType_e type, unsigned char *data);
		miDriver::DriverResults readInputRegisters(int address, int count, unsigned char* data);
		miDriver::DriverResults readRegisters(int address, int count, unsigned char* data);
		miDriver::DriverResults writeBits(int address, ModbusDriverAccessType_e type, int count, unsigned char* data);
		miDriver::DriverResults writeRegister(int address, int count, unsigned char* data);

	};
}
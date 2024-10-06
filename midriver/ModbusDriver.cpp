#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "ModbusDriver.h"

miDriver::ModbusDriver* miDriver::ModbusDriver::_ModbusDriverInstance = nullptr;

miDriver::DriverResults miDriver::ModbusDriver::open()
{
	if (_Handle != nullptr)
	{
		return miDriver::DriverResults::ErrorAllreadyOpen;
	}
	_Handle = modbus_new_rtu(_Device.c_str(), 57600, 'N', 8, 1);
	if (_Handle == nullptr)
	{
		printf("miDriver::ModbusDriver::open  error %s %s\n", strerror(errno), _Device.c_str());
		return miDriver::DriverResults::ErrorOpen;
	}
	modbus_rtu_set_serial_mode(_Handle, MODBUS_RTU_RS485);
	modbus_rtu_set_rts_delay(_Handle, 100);
	//modbus_set_debug(_modbusHandle, 1);
	modbus_set_error_recovery(_Handle,
		static_cast<modbus_error_recovery_mode>(
			MODBUS_ERROR_RECOVERY_LINK | MODBUS_ERROR_RECOVERY_PROTOCOL));
	modbus_set_response_timeout(_Handle, 0, 100000);
	if (modbus_connect(_Handle) == -1)
	{
		printf("miDriver::ModbusDriver::open  error %s %s\n", strerror(errno), _Device.c_str());
		return miDriver::DriverResults::ErrorOpen;
	}

	return miDriver::DriverResults::Ok;
}

void miDriver::ModbusDriver::close()
{
	modbus_close(_Handle);
	modbus_free(_Handle);
	_Handle = nullptr;
}

miDriver::DriverResults miDriver::ModbusDriver::readInputBits(int address, int count, ModbusDriverAccessType_e type, unsigned char* data)
{
	int result = 0;
	uint8_t inp[32] = { 0 };

	if (_Handle == nullptr)
	{
		printf("miDriver::ModbusDriver::read  error : invalid handle\n");
		return miDriver::DriverResults::ErrorRead;
	}

	if (data == nullptr || address == 0 || count == 0)
	{
		printf("miDriver::ModbusDriver::read  error : invalid parameter\n");
		return miDriver::DriverResults::ErrorRead;
	}

	if (modbus_set_slave(_Handle, address) < 0)
	{
		printf("miDriver::ModbusDriver::read  error : set slave failed\n");
		return miDriver::DriverResults::ErrorRead;
	}
	memset(inp, 0, 32);
	
	result = modbus_read_input_bits(_Handle, 0, count * sizeof(uint8_t), inp);
	::usleep(1000);
	if(result < 0)
	{
		printf("miDriver::ModbusDriver::read  error : modbus_read_input_bits failed error %s\n", strerror(errno));
		return miDriver::DriverResults::ErrorRead;
	}

	if (type == ModbusDriverAccessType_e::BITS)
	{
		::memset(data, 0, 4);
		for (int i = 0; i < result; i++)
		{
			int byteNr = i / 8;
			int bitNr = i % 8;
		
			if (inp[i] > 0)
			{
				data[byteNr] |= (1 << bitNr);
			}
		}
	}
	else
	{
		memcpy(data, inp, result);
	}
	return miDriver::DriverResults::Ok;
}

miDriver::DriverResults miDriver::ModbusDriver::readInputRegisters(int address, int count, unsigned char* data)
{
	int result = 0;
	if (_Handle == nullptr)
	{
		printf("miDriver::ModbusDriver::read  error %s\n", strerror(errno));
		return miDriver::DriverResults::ErrorRead;
	}

	if (data == nullptr || address == 0 || count == 0)
	{
		printf("miDriver::ModbusDriver::read  error : invalid parameter\n");
		return miDriver::DriverResults::ErrorRead;
	}

	if (modbus_set_slave(_Handle, address) < 0)
	{
		printf("miDriver::ModbusDriver::read  error : set slave failed\n");
		return miDriver::DriverResults::ErrorRead;
	}

	result = modbus_read_input_registers(_Handle, address, count, reinterpret_cast <uint16_t*>(data));
	if (result < 0)
	{
		printf("miDriver::ModbusDriver::read  error : modbus_read_input_registers failed\n");
		return miDriver::DriverResults::ErrorRead;
	}

	return miDriver::DriverResults::Ok;
}

miDriver::DriverResults miDriver::ModbusDriver::readRegisters(int address, int count, unsigned char* data)
{
	int result = 0;
	if (_Handle == nullptr)
	{
		printf("miDriver::ModbusDriver::read  error : invalid handle\n");
		return miDriver::DriverResults::ErrorRead;
	}

	if (data == nullptr || address == 0 || count == 0)
	{
		printf("miDriver::ModbusDriver::read  error : invalid parameter\n");
		return miDriver::DriverResults::ErrorRead;
	}

	if (modbus_set_slave(_Handle, address) < 0)
	{
		printf("miDriver::ModbusDriver::read  error : set slave failed\n");
		return miDriver::DriverResults::ErrorRead;
	}

	result = modbus_read_input_registers(_Handle, address, count, reinterpret_cast <uint16_t*>(data));
	if (result < 0)
	{
		printf("miDriver::ModbusDriver::read  error : modbus_read_input_registers failed\n");
		return miDriver::DriverResults::ErrorRead;
	}

	return miDriver::DriverResults::Ok;
}

miDriver::DriverResults miDriver::ModbusDriver::writeBits(int address, ModbusDriverAccessType_e type, int count, unsigned char* data)
{
	int result = 0;
	uint8_t outp[32] = { 0 };
	if (_Handle == nullptr)
	{
		printf("miDriver::ModbusDriver::write  error : invalid handle\n");
		return miDriver::DriverResults::ErrorWrite;
	}

	if (data == nullptr || address == 0 || count == 0)
	{
		printf("miDriver::ModbusDriver::read  error : invalid parameter\n");
		return miDriver::DriverResults::ErrorWrite;
	}

	if (modbus_set_slave(_Handle, address) < 0)
	{
		printf("miDriver::ModbusDriver::read  error : set slave failed\n");
		return miDriver::DriverResults::ErrorRead;
	}

	if (type == ModbusDriverAccessType_e::BITS)
	{
		for (int i = 0; i < count; i++)
		{
			int byteNr = i / 8;
			int bitNr = i % 8;
			if ((data[byteNr] & (1 << bitNr)) > 0)
			{
				outp[i] = 1;
			}
			else
			{
				outp[i] = 0;
			}
		}
	}
	else
	{
		memcpy(outp, data, result);
	}

	result = modbus_write_bits(_Handle, 0, count, outp);
	if (result < 0)
	{
		printf("miDriver::ModbusDriver::read  error : modbus_read_input_registers failed\n");
		return miDriver::DriverResults::ErrorRead;
	}

	return miDriver::DriverResults::Ok;
}

miDriver::DriverResults miDriver::ModbusDriver::writeRegister(int address, int count, unsigned char* data)
{
	int result = 0;
	if (_Handle == nullptr)
	{
		printf("miDriver::ModbusDriver::write  error : invalid handle\n");
		return miDriver::DriverResults::ErrorWrite;
	}

	if (data == nullptr || address == 0 || count == 0)
	{
		printf("miDriver::ModbusDriver::read  error : invalid parameter\n");
		return miDriver::DriverResults::ErrorWrite;
	}

	if (modbus_set_slave(_Handle, address) < 0)
	{
		printf("miDriver::ModbusDriver::read  error : set slave failed\n");
		return miDriver::DriverResults::ErrorRead;
	}

	result = modbus_write_registers(_Handle, address, count, reinterpret_cast <uint16_t*>(data));
	if (result < 0)
	{
		printf("miDriver::ModbusDriver::read  error : modbus_read_input_registers failed\n");
		return miDriver::DriverResults::ErrorRead;
	}

	return miDriver::DriverResults::Ok;
}

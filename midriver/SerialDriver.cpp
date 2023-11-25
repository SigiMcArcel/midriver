#include "SerialDriver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <iostream>
#include <sys/ioctl.h>


miDriver::DriverResults miDriver::SerialDriver::open()
{
	if (_Handle != -1)
	{
		return miDriver::DriverResults::ErrorAllreadyOpen;
	}

	_Handle = ::open(_SerialDevPath.c_str(), O_RDWR);
	if (_Handle == -1)
	{
		printf("miDriver::SerialDriver::open  error %s %s\n", strerror(errno), _SerialDevPath.c_str());
		return miDriver::DriverResults::ErrorOpen;
	}

	_ReadTimer.Start(0, this);
	_WriteTimer.Start(0, this);

	return miDriver::DriverResults::Ok;
}

void miDriver::SerialDriver::close()
{
	_ReadTimer.Stop();
	_WriteTimer.Stop();

	::close(_Handle);
	_Handle = -1;
}

int miDriver::SerialDriver::read(int len, unsigned char* data)
{
	int result = 0;
	int bytesread = 0;
	if (_Handle == -1)
	{
		return miDriver::DriverResults::ErrorNotOpen;
	}
	do
	{
		result = (int)::read(_Handle, data, len);
		if (result == -1)
		{
			printf("miDriver::SerialDriver::read  error %s\n", strerror(errno));
			return result;
		}
		bytesread += result;
	} while (bytesread < len && result > 0);
	/*if (bytesread)
	{
		printf(" miDriver::SerialDriver::read  %d 0x%2x 0x%2x 0x%2x\n", bytesread, data[0], data[1], data[2]);
	}*/
	return bytesread;
}

miDriver::DriverResults miDriver::SerialDriver::write(int len, unsigned char* data)
{
	int result = 0;
	int bytesWritten = 0;
	if (_Handle == -1)
	{
		return miDriver::DriverResults::ErrorWrite;
	}
	//printf(" miDriver::SerialDriver::write  %d 0x%2x 0x%2x 0x%2x\n", len, data[0], data[1], data[2]);
	do
	{
		result = (int)::write(_Handle, data, len);
		if (result == -1)
		{
			printf("miDriver::SerialDriver::write  error %s Handle %d\n", strerror(errno), _Handle);
			return miDriver::DriverResults::ErrorWrite;
		}
		bytesWritten += result;
	} while (bytesWritten < len);

	return miDriver::DriverResults::Ok;
}

void miDriver::SerialDriver::readProc()
{
	unsigned char data[2] = { 0,0 };
	if (read(1, data) > 0)
	{
		_InputBuffer.push_back(data[0]);

		if (_StringReceivedEvent != nullptr)
		{
			if (std::string((char*)data) == _Delemiter)
			{
				std::list<unsigned char>::iterator iter;
				_StringReceivedEvent->DataReceived(std::string(_InputBuffer.begin(), _InputBuffer.end()));
				_InputBuffer.clear();
			}
		}

		if (_DataReceivedEvent != nullptr)
		{
			if (_InputBuffer.size() == _Threshold)
			{
				std::list<unsigned char>::iterator iter;
				_DataReceivedEvent->DataReceived(_InputBuffer);
				_InputBuffer.clear();
			}
		}


	}
}

void miDriver::SerialDriver::writeProc()
{
	_Semaphore.wait(0);
	
	while (!_OutputBuffer.empty())
	{
		
		unsigned char data = _OutputBuffer.front();
		_OutputBuffer.pop_front();
		write(1, &data);
	}
}

miDriver::DriverResults  miDriver::SerialDriver::serialWrite(const std::list<unsigned char>& data)
{
	_CriticalSection.EnterCriticalSection();
	_OutputBuffer.insert(_OutputBuffer.end(), data.begin(), data.end());
	_CriticalSection.LeaveCriticalSection();
	_Semaphore.set();
	return miDriver::DriverResults();
}

miDriver::DriverResults  miDriver::SerialDriver::serialWrite(const std::string& data)
{
	_CriticalSection.EnterCriticalSection();
	_OutputBuffer.insert(_OutputBuffer.end(), data.begin(), data.end());
	_CriticalSection.LeaveCriticalSection();
	_Semaphore.set();

	return miDriver::DriverResults();
}

miDriver::DriverResults  miDriver::SerialDriver::serialRead(std::list<unsigned char>& data)
{
	if (!_InputBuffer.empty())
	{
		data = _InputBuffer;
		_InputBuffer.clear();
		return miDriver::DriverResults_e::Ok;
	}
	return miDriver::DriverResults_e::ErrorBufferEmpty;
}

miDriver::DriverResults  miDriver::SerialDriver::serialRead(std::string& data)
{
	if (!_InputBuffer.empty())
	{
		data = std::string(_InputBuffer.begin(), _InputBuffer.end());
		_InputBuffer.clear();
		return miDriver::DriverResults_e::Ok;
	}
	return miDriver::DriverResults_e::ErrorBufferEmpty;
}

miDriver::DriverResults  miDriver::SerialDriver::serialRegister(SerialDriverDataReceivedEvent* callback, int threshold)
{
	if (callback == nullptr)
	{
		return miDriver::DriverResults::ErrorOpen;
	}
	_DataReceivedEvent = callback;
	return miDriver::DriverResults();
}

miDriver::DriverResults  miDriver::SerialDriver::serialRegister(SerialDriverStringReceivedEvent* callback, const std::string delemiter)
{
	if (callback == nullptr)
	{
		return miDriver::DriverResults::ErrorOpen;
	}
	_Delemiter = delemiter;
	_StringReceivedEvent = callback;
	return miDriver::DriverResults();
}



void miDriver::SerialDriver::eventOccured(void* sender, const std::string& name)
{

	if (name == "WriteTimer")
	{
		writeProc();
	}
	if (name == "ReadTimer")
	{

		readProc();
	}
}
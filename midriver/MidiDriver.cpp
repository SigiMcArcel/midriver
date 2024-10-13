#include "MidiDriver.h"
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>


miDriver::MidiDriver* miDriver::MidiDriver::_MidiDriverInstance = nullptr;

miDriver::DriverResults miDriver::MidiDriver::open()
{
	if (_Handle != -1)
	{
		return miDriver::DriverResults::ErrorAllreadyOpen;
	}
	
	_Handle = ::open(_MidiDevPath.c_str(), O_RDWR);
	if (_Handle == -1)
	{
		printf("miDriver::MidiDriver::open  error %s %s\n", strerror(errno), _MidiDevPath.c_str());
		return miDriver::DriverResults::ErrorOpen;
	}
	
		_ReadTimer.Start();
		_WriteTimer.Start();
	
	return miDriver::DriverResults::Ok;
}

void miDriver::MidiDriver::close()
{
	_ReadTimer.Stop();
	_WriteTimer.Stop();
	
	::close(_Handle);
	_Handle = -1;
}

miDriver::DriverResults miDriver::MidiDriver::read(int len, unsigned char* data)
{
	int result = 0;
	if (_Handle == -1)
	{
		return miDriver::DriverResults::ErrorNotOpen;
	}
	result = (int)::read(_Handle, data, len);
	if (result < len)
	{
		printf("miDriver::MidiDriver::read  error %s\n", strerror(errno));
		return miDriver::DriverResults::ErrorRead;
	}
	//printf(" miDriver::MidiDriver::read  %d 0x%2x 0x%2x 0x%2x\n", len, data[0], data[1], data[2]);
	return miDriver::DriverResults::Ok;
}

miDriver::DriverResults miDriver::MidiDriver::write(int len, unsigned char* data)
{
	int result = 0;
	if (_Handle == -1)
	{
		return miDriver::DriverResults::ErrorWrite;
	}
	
	result = (int)::write(_Handle, data, len);
	if (result == -1)
	{
		printf("miDriver::MidiDriver::write  error %s Handle %d\n", strerror(errno),_Handle);
		return miDriver::DriverResults::ErrorWrite;
	}
	return miDriver::DriverResults::Ok;
}

void miDriver::MidiDriver::timerEventOccured(void* sender, const std::string& name)
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

miDriver::DriverResults miDriver::MidiDriver::read(miDriver::MidiMessage& data)
{
	if (!_MidiInputBuffer.empty())
	{
		data = _MidiInputBuffer.back();
		_MidiInputBuffer.pop_back();
		return miDriver::DriverResults_e::Ok;
	}
	return miDriver::DriverResults_e::ErrorBufferEmpty;
}

miDriver::DriverResults miDriver::MidiDriver::write(const miDriver::MidiMessage& data)
{
	_CriticalSection.EnterCriticalSection();
	//printf(" miDriver::MidiDriver::write  %d 0x%2x 0x%2x 0x%2x\n", data.Len, data.U.MessageRaw[0], data.U.MessageRaw[1], data.U.MessageRaw[2]);
	_MidiOutputBuffer.push_back(data);
	_CriticalSection.LeaveCriticalSection();
	_Semaphore.set();
	
	return miDriver::DriverResults_e::Ok;
}

void miDriver::MidiDriver::readProc()
{
	unsigned char buff[3] = { 0 };
	while (read(3, buff) == miDriver::DriverResults_e::Ok)
	{
		MidiMessage data;
		data.Len = 3;
		::memcpy(data.U.MessageRaw, buff, 3);
		_MidiInputBuffer.push_back(data);
	}
}
void miDriver::MidiDriver::writeProc()
{
	_Semaphore.wait(0);
	while (!_MidiOutputBuffer.empty())
	{
		MidiMessage data = _MidiOutputBuffer.back();
		_MidiOutputBuffer.pop_back();
		
		write(static_cast<int>(data.Len), data.U.MessageRaw);
	}
}

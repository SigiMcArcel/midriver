#pragma once

#include "DriverBase.h"
#include <mi/miutils/Timer.h>
#include <mi/miutils/Semaphore.h>
#include <list>
#include <termios.h>

namespace miDriver
{
	class SerialDriverStringReceivedEvent
	{
	public:
		virtual void DataReceived(const std::string& data) = 0;
	};

	class SerialDriverDataReceivedEvent
	{
	public:
		virtual void DataReceived(const std::list<unsigned char>& data) = 0;

	};

	class SerialDriver : public miutils::TimerEventListener
	{
		std::string _SerialDevPath;
		int _Baudrate;
		int _Handle;
		std::list<unsigned char> _InputBuffer;
		std::list<unsigned char> _OutputBuffer;
		miutils::Timer _ReadTimer;
		miutils::Timer _WriteTimer;
		miutils::Semaphore _Semaphore;
		SerialDriverDataReceivedEvent* _DataReceivedEvent;
		SerialDriverStringReceivedEvent* _StringReceivedEvent;
		std::string _Delemiter;
		std::size_t _Threshold;
		miutils::CriticalSection _CriticalSection;
		bool _Stop;
		struct termios _oldTty;

		int read(int len, unsigned char* data);
		miDriver::DriverResults write(int len, unsigned char* data);
		void readProc();
		void writeProc();
		void configure();

	public:
		SerialDriver(int baudrate)
			:_SerialDevPath("/dev/tty0")
			, _Baudrate(baudrate)
			, _Handle(-1)
			, _InputBuffer()
			, _OutputBuffer()
			, _ReadTimer("ReadTimer", this)
			, _WriteTimer("WriteTimer", this)
			, _Semaphore()
			, _DataReceivedEvent(nullptr)
			, _StringReceivedEvent(nullptr)
			, _Delemiter("")
			, _Threshold(0)
			, _CriticalSection()
			, _Stop(false)
		{
		}

		SerialDriver(std::string dev, int baudrate)
			:_SerialDevPath(dev)
			, _Baudrate(baudrate)
			, _Handle(-1)
			, _InputBuffer()
			, _OutputBuffer()
			, _ReadTimer("ReadTimer", this)
			, _WriteTimer("WriteTimer", this)
			, _Semaphore()
			, _DataReceivedEvent(nullptr)
			, _StringReceivedEvent(nullptr)
			, _Delemiter("")
			, _Threshold(0)
			, _CriticalSection()
			, _Stop(false)
		{

		}

		SerialDriver(std::string dev, int baudrate,int prio)
			:_SerialDevPath(dev)
			, _Baudrate(baudrate)
			, _Handle(-1)
			, _InputBuffer()
			, _OutputBuffer()
			, _ReadTimer("ReadTimer",  1, 0, prio + 2, miutils::SchedulerType::Fifo)
			, _WriteTimer("WriteTimer", 1, 0, prio, miutils::SchedulerType::Fifo)
			, _Semaphore()
			, _DataReceivedEvent(nullptr)
			, _StringReceivedEvent(nullptr)
			, _Delemiter("")
			, _Threshold(0)
			, _CriticalSection()
			, _Stop(false)
		{

		}

		miDriver::DriverResults open();
		void close();

		miDriver::DriverResults  serialWrite(const std::list<unsigned char>& data);
		miDriver::DriverResults  serialWrite(const std::string& data);
		miDriver::DriverResults  serialRead(std::list<unsigned char>& data);
		miDriver::DriverResults  serialRead(std::string& data);
		miDriver::DriverResults  serialRegister(SerialDriverDataReceivedEvent* callback, int threshold);
		miDriver::DriverResults  serialRegister(SerialDriverStringReceivedEvent* callback, const std::string& delemiter);


		// Geerbt über EventListener
		virtual bool timerEventOccured(void* sender, const std::string& name) override;

	};
}


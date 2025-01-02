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
		fprintf(stderr,"miDriver::SerialDriver::open  error %s %s\n", strerror(errno), _SerialDevPath.c_str());
		return miDriver::DriverResults::ErrorOpen;
	}
	configure();
	_Stop = false;

	_ReadTimer.Start(1);
	_WriteTimer.Start(1);

	return miDriver::DriverResults::Ok;
}

void miDriver::SerialDriver::close()
{
	_Stop = true;
	_Semaphore.set();
	if (tcsetattr(_Handle, TCSANOW, &_oldTty) != 0) {
		perror("Fehler beim Anwenden der neuen Einstellungen");
		return;
	}
	::close(_Handle);
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
	int result = read(1, data);
	if(result > 0)
	{
		_InputBuffer.push_back(data[0]);
		
		if (_StringReceivedEvent != nullptr)
		{
			if ((char)data[0] == _Delemiter[0])
			{
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
	else  if(result < 0)
	{
		fprintf(stderr, "SerialDriver::readProc error %d \n", result);
		return;
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

void miDriver::SerialDriver::configure()
{
	struct termios tty;
	int baud = 0;
	memset(&tty, 0, sizeof(struct termios));
	if (tcgetattr(_Handle, &_oldTty) != 0) {
		perror("Fehler beim Abrufen der aktuellen Einstellungen");
		return;
	}

	switch (_Baudrate)
	{
		case 0:  baud = B0; break;
		case 50:  baud = B50; break;
		case 75:  baud = B75; break;
		case 110:  baud = B110; break;
		case 134:  baud = B134; break;
		case 150:  baud = B150; break;
		case 200:  baud = B200; break;
		case 300:  baud = B300; break;
		case 600:  baud = B600; break;
		case 1200:  baud = B1200; break;
		case 1800:  baud = B1800; break;
		case 2400:  baud = B2400; break;
		case 4800:  baud = B4800; break;
		case 9600:  baud = B9600; break;
		case 19200:  baud = B19200; break;
		case 38400:  baud = B38400; break;
		case 57600:  baud = B57600; break;
		case 115200:  baud = B115200; break;
		case 460800: baud = B460800; break;
		case 500000: baud = B500000; break;
		case 576000: baud = B576000; break;
		case 1000000:baud = B1000000; break;
		case 1152000:baud = B1152000; break;
		case 1500000:baud = B1500000; break;
		case 2000000:baud = B2000000; break;
		case 2500000:baud = B2500000; break;
		case 3000000:baud = B3000000; break;
		case 3500000:baud = B3500000; break;
		case 4000000:baud = B4000000; break;
		default:
		{
			std::cout << _Baudrate << "wrong set to 9600";
			baud = B9600;
		}

	}
	// Baudrate setzen
	cfsetispeed(&tty, baud);
	cfsetospeed(&tty, baud);

	// Einstellungen konfigurieren
	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8 Datenbits
	tty.c_cflag &= ~PARENB;                    // Kein Paritätsbit
	tty.c_cflag &= ~CSTOPB;                    // 1 Stoppbit
	tty.c_cflag &= ~CRTSCTS;                   // Kein Hardware-Flow-Control
	tty.c_cflag |= CLOCAL | CREAD;             // Lokale Verbindung, Lesen aktivieren

	tty.c_iflag &= ~(IXON | IXOFF | IXANY);    // Kein Software-Flow-Control
	tty.c_oflag = 0;                           // Rohdatenmodus
	tty.c_lflag = 0;                           // Nicht-kanonisch, kein Echo

	tty.c_cc[VMIN] = 1;                        // Minimum 1 Zeichen
	tty.c_cc[VTIME] = 0;                       // Keine Verzögerung
	
	// Neue Einstellungen anwenden
	if (tcsetattr(_Handle, TCSANOW, &tty) != 0) {
		perror("Fehler beim Anwenden der neuen Einstellungen");
		return;
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

miDriver::DriverResults  miDriver::SerialDriver::serialRegister(SerialDriverStringReceivedEvent* callback, const std::string& delemiter)
{
	if (callback == nullptr)
	{
		return miDriver::DriverResults::ErrorOpen;
	}
	_Delemiter = delemiter;
	_StringReceivedEvent = callback;

	return miDriver::DriverResults();
}

bool miDriver::SerialDriver::timerEventOccured(void* sender, const std::string& name)
{
	
	if (name == "WriteTimer")
	{
		writeProc();
		if (_Stop)
		{
			return true;
		}
	}
	if (name == "ReadTimer")
	{
		readProc();
		if (_Stop)
		{
			return true;
		}
	}
	return false;
}
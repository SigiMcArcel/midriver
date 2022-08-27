#include "SPIDriver.h"
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

int midriver::SPIDriver::open()
{
    if (_Device == SPIDevices::SPIDev0)
    {
        return ::open(SPI0Path.c_str(), O_RDWR);
    }
    else
    {
        return ::open(SPI1Path.c_str(), O_RDWR);
    }

    
}

void midriver::SPIDriver::close(int fd)
{
    ::close(fd);
}

int midriver::SPIDriver::configure(int fd)
{
    int mode = 0;
    int bits = (int)_Bits;
    int speed = _Speed;
    int result = 0;
    int lsb = 0;

    if (_Bits == SPI8Bits)
    {
        bits = 0;
    }
    switch (_Mode)
    {
    case SPIModes::SPIMode0:
        {
        mode = SPI_MODE_0;
        break;
        }
    case SPIModes::SPIMode1:
    {
        mode = SPI_MODE_1;
        break;
    }
    case SPIModes::SPIMode2:
    {
        mode = SPI_MODE_2;
        break;
    }
    case SPIModes::SPIMode3:
    {
        mode = SPI_MODE_3;
        break;
    }
    default:
    {
        return -1;
    }
    }

    result = ioctl(fd, SPI_IOC_WR_MODE32, &mode);
    if (result < 0)
    {
        perror("Could not set SPIMode (WR)...ioctl fail");
        return result;
    }

    result =  ioctl(fd, SPI_IOC_RD_MODE32, &mode);
    if (result  < 0)
    {
        perror("Could not set SPIMode (RD)...ioctl fail");
        return result;
    }

    result = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (result < 0)
    {
        perror("Could not set SPI bitsPerWord (WR)...ioctl fail");
        return result;
    }

    result = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (result < 0)
    {
        perror("Could not set SPI bitsPerWord(RD)...ioctl fail");
        return result;
    }

    result = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (result < 0)
    {
        perror("Could not set SPI speed (WR)...ioctl fail");
        return result;
    }

    result = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (result < 0)
    {
        perror("Could not set SPI speed (RD)...ioctl fail");
        return result;
    }

    result = ioctl(fd, SPI_IOC_WR_LSB_FIRST, &lsb);
    if (result < 0)
    {
        perror("Could not set SPI lsb...ioctl fail");
        return result;
    }
	return 0;
}

int midriver::SPIDriver::SPIRead(int address, int len, unsigned char* data)
{
	int result = 0;
	int fd = open();
	if (fd < 0)
	{
		return fd;
	}
    result = configure(fd);
	if (result < 0)
	{
		close(fd);
		return result;
	}
	result = (int)::read(fd, data, len);
	if (result != len)
	{
		close(fd);
		return 0;
	}
	close(fd);
	return len;
    return 0;
}

int midriver::SPIDriver::SPIWrite(int address, int len, unsigned char* data)
{
    return 0;
}

int midriver::SPIDriver::SPITransfer(int address, int len, unsigned char* txdata, unsigned char* rxdata)
{
    struct spi_ioc_transfer spi;
    int result = 0;
    int fd = open();
    if (fd < 0)
    {
        return fd;
    }

    configure(fd);
    memset(&spi, 0, sizeof(spi));
    spi.tx_buf = (unsigned long)txdata;		//transmit from "data"
    spi.rx_buf = (unsigned long)rxdata;		//receive into "data"
    spi.len = len;
    spi.delay_usecs = 0;
    spi.speed_hz = _Speed;
    spi.bits_per_word = (unsigned char)_Bits;
    spi.cs_change = 0;						//0=Set CS high after a transfer, 1=leave CS set low
    
    result = ioctl(fd, SPI_IOC_MESSAGE(1), &spi);

    if (result < 0)
    {
        perror(strerror(errno));
        return result;
    }
    close(fd);
    return result;
}

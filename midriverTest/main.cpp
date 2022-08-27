#include <cstdio>
#include <stdio.h>
#include "../midriver/SPIDriver.h"

int main()
{
    unsigned char data[] = { 0x11  , 0x22, 0x33 };

    midriver::SPIDriver driver(midriver::SPIModes::SPIMode3,500000, midriver::SPI8Bits, midriver::SPIDev0);
    int result = driver.SPITransfer(0, 3, data,data);
   
    printf("Hallo aus %s!\n", "midriverTest");
    while (1)
    {
        //usleep(1000000);
    }
    return 0;
}
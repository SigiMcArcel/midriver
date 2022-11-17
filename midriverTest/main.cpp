#include <cstdio>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include "../midriver/SPIDriver.h"

int main()
{
    unsigned char rdata[100] = { 0 };
    unsigned char tdata[] = { 0x55  , 0xaa, 0xee };

    midriver::SPIDriver driver(midriver::SPIModes::SPIMode0,500000, midriver::SPI8Bits, midriver::SPIDev0);
    
   
    printf("Hallo aus %s!\n", "midriverTest");
    while (1)
    {
        int n = 0;
        std::cin >> n;
        std::cout << "send";
        driver.SPITransfer(0, 3, tdata, rdata);
        printf("rx 0x%2x 0x%2x 0x%2x\n", rdata[0], rdata[1], rdata[2]);
        
    }
    return 0;
}
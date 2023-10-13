#include <cstdio>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <mi/mimidriver/I2CDriver.h>

typedef struct bits
{
    unsigned char b0 : 1;
    unsigned char b1 : 1;
    unsigned char b2 : 1;
    unsigned char b3 : 1;
    unsigned char b4 : 1;
    unsigned char b5 : 1;
    unsigned char b6 : 1;
    unsigned char b7 : 1;

}BITS;

typedef union test_t
{
    unsigned char b;
    BITS bits;
    
}TESTDATA;


int main()
{
  
    TESTDATA buff0 = { 0x00 };
    TESTDATA buff1 = { 0x00 };
    TESTDATA buff2 = { 0x00 };
    TESTDATA buff3 = { 0x00 };
    TESTDATA buff4 = { 0x00 };
    TESTDATA buff5 = { 0x00 };
    uint16_t an = 0;
   
    miDriver::I2CDriver _driver1(0x20);
    miDriver::I2CDriver _driver2(0x21);
    miDriver::I2CDriver _driver3(0x22);
    miDriver::I2CDriver _driver4(0x38);
    miDriver::I2CDriver _driver5(0x39);
    miDriver::I2CDriver _driver6(0x3a);
    miDriver::I2CDriver _driver7(0x42);

    _driver1.open();
    _driver2.open();
    _driver3.open();
    _driver4.open();
    _driver5.open();
    _driver6.open();
    _driver7.open();
   
    printf("Hallo aus %s!\n", "midriverTest");
    while (1)
    {
       
       
        _driver4.read(1, (unsigned char*)&buff0);
        _driver5.read(1, (unsigned char*)&buff1);
        _driver6.read(1, (unsigned char*)&buff2);
        _driver7.read(2, (unsigned char*)&an);

        //module1
        buff3.bits.b0 = buff0.bits.b0;
        buff3.bits.b1 = buff0.bits.b1;
        buff3.bits.b2 = buff0.bits.b2;
        buff3.bits.b3 = buff0.bits.b3;

       // buff3.bits.b4 = buff0.bits.b4;


        buff3.bits.b5 = buff0.bits.b5;
        buff3.bits.b6 = buff0.bits.b6;
        buff3.bits.b7 = buff0.bits.b7;


        //module 2
       // buff4.bits.b0 = buff1.bits.b0;
        buff4.bits.b1 = buff1.bits.b1;
        buff4.bits.b2 = buff1.bits.b2;
        buff4.bits.b3 = buff1.bits.b3;
        buff4.bits.b4 = buff1.bits.b4;
        buff4.bits.b5 = buff1.bits.b5;
        buff4.bits.b6 = buff1.bits.b6;
        buff4.bits.b7 = buff1.bits.b7;

        //module 3
        buff5.bits.b0 = buff2.bits.b0;
        buff5.bits.b1 = buff2.bits.b1;
        buff5.bits.b2 = buff2.bits.b2;
        buff5.bits.b3 = 1;

        buff5.bits.b4 = buff0.bits.b4;
        buff5.bits.b5 = buff1.bits.b0;
        buff5.bits.b6 = buff2.bits.b3;
        buff5.bits.b7 = 1;

        
        printf("data in 0x%x 0x%x 0x%x %d\n",buff0.b,buff1.b,buff2.b,an);
        printf("data out 0x%x 0x%x 0x%x\n", buff0.b, buff1.b, buff2.b);
        printf("\n");

       // buff5.bits.b4 = buff0.bits.b4;
       // buff5.bits.b5 = buff0.bits.b0;

       // buff5.bits.b6 = buff2.bits.b6;
       // buff5.bits.b7 = buff2.bits.b7;


        _driver1.write(1, (unsigned char*)&buff3);
        _driver2.write(1, (unsigned char*)&buff4);
        _driver3.write(1, (unsigned char*)&buff5);
        
        ::usleep(100000);

    }
    return 0;
}
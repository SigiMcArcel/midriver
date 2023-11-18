#include <cstdio>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <mi/midriver/MidiDriver.h>




int main()
{
    miDriver::MidiMessage msg;
    msg.Len = 3;
    msg.U.Message.Key = 0;
    msg.U.Message.StatusByte.Command = miDriver::MidiCommand_e::NoteOn;
    msg.U.Message.StatusByte.Channel = 0;
    miDriver::MidiDriver* _d = miDriver::MidiDriver::GetInstance("/dev/snd/midiC1D0");
    _d->open();

   


    while (1)
    {
        _d->write(msg);
        //_d->read(msg);
        usleep(50000);
    }
    
    return 0;
}
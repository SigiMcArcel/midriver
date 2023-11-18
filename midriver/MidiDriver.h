#pragma once

#include "DriverBase.h"
#include <mi/miutils/Timer.h>
#include <mi/miutils/Semaphore.h>
#include <vector>

namespace miDriver
{
	enum class MidiCommand_e : unsigned char
	{
		NoteOff = 0x08,
		NoteOn = 0x09,
		PolyPressure = 0x0A,
		Controller = 0x0B,
		ProgramChange = 0x0C,
		ChannelPressure = 0x0D,
		PitchWheel = 0x0E
	};

	struct  MidiMessage
	{
		union midiMessage_u
		{
			unsigned char MessageRaw[20];
			struct Message_t
			{
				struct StatusByte_t
				{
					unsigned char Channel : 4;
					MidiCommand_e Command : 4;
				}StatusByte;
				unsigned char Key;
				unsigned char Velocity;
			}Message;
		}U;
		size_t Len;
	};

	class MidiDriver : public miutils::EventListener
	{
	private:
		const std::size_t _MaxWriteMessage = 100;

		std::string _MidiDevPath;
		int _Handle;
		std::vector<miDriver::MidiMessage> _MidiInputBuffer;
		std::vector<miDriver::MidiMessage> _MidiOutputBuffer;
		miutils::Timer _ReadTimer;
		miutils::Timer _WriteTimer;
		miutils::Semaphore _Semaphore;

		miDriver::DriverResults read(int len, unsigned char* data);
		miDriver::DriverResults write(int len, unsigned char* data);
		void readProc();
		void writeProc();


	

	protected:
		MidiDriver()
			:_MidiDevPath("/dev/midi")
			, _Handle(-1)
			, _MidiInputBuffer()
			, _MidiOutputBuffer(std::vector<miDriver::MidiMessage>())
			, _ReadTimer("ReadTimer",this)
			, _WriteTimer("WriteTimer",this)
			, _Semaphore()
		{
		}
		MidiDriver(std::string dev)
			:_MidiDevPath(dev)
			, _Handle(-1)
			, _MidiInputBuffer()
			, _MidiOutputBuffer()
			, _ReadTimer("ReadTimer", this)
			, _WriteTimer("WriteTimer", this)	
			, _Semaphore()
		{

		}
		static MidiDriver* _MidiDriverInstance;

	public:
		/**
	* Singletons should not be cloneable.
	*/
		MidiDriver(MidiDriver& other) = delete;
		/**
		 * Singletons should not be assignable.
		 */
		void operator=(const MidiDriver&) = delete;
		/**
		 * This is the static method that controls the access to the singleton
		 * instance. On the first run, it creates a singleton object and places it
		 * into the static field. On subsequent runs, it returns the client existing
		 * object stored in the static field.
		 */

		static MidiDriver* GetInstance(std::string dev)
		{
			if (_MidiDriverInstance == nullptr)
			{
				_MidiDriverInstance = new MidiDriver(dev);
			}
			return _MidiDriverInstance;
		}
		


		miDriver::DriverResults open();
		void close();
		miDriver::DriverResults read(MidiMessage& data);
		miDriver::DriverResults write(const MidiMessage& data);
		


		// Geerbt über EventListener
		virtual void eventOccured(void* sender, const std::string& name) override;

	};
	
	
	
}




DESTDIR=
prefix=/usr
includedir=$(prefix)/include
libdir=$(prefix)/lib

INCLUDES = -I. -I/.
CXX_FLAGS = -fpic -Wall -Wl,--no-undefined
LD_FLAGS = -shared

SOURCES=GPIODriver.cpp I2CDriver.cpp SPIDriver.cpp MidiDriver.cpp SerialDriver.cpp
OBJECTS=$(SOURCES:.cpp=.o)

LIBRARYOUT=libmidriver.so


.cpp.o:
	$(CXX) $(CXX_FLAGS) $(INCLUDES) -c $<
$(LIBRARYOUT):$(OBJECTS)
	$(CXX) $(LD_FLAGS) $(OBJECTS) -o $(LIBRARYOUT)
clean:
	$(RM) $(LIBRARYOUT) *.o
	

install: $(LIBRARYOUT)
	install -d $(DESTDIR)/${libdir}
	install -D -m 0644 libmidriver.so $(DESTDIR)/${libdir}/$(LIBRARYOUT)
	install -d $(DESTDIR)/${includedir}
	install -d $(DESTDIR)/${includedir}/mi
	install -d $(DESTDIR)/${includedir}/mi/midriver
	install -D -m 0644 DriverBase.h $(DESTDIR)/${includedir}/mi/midriver/DriverBase.h
	install -D -m 0644 I2CDriver.h $(DESTDIR)/${includedir}/mi/midriver/I2CDriver.h
	install -D -m 0644 SPIDriver.h $(DESTDIR)/${includedir}/mi/midriver/SPIDriver.h
	install -D -m 0644 GPIODriver.h $(DESTDIR)/${includedir}/mi/midriver/GPIODriver.h
	install -D -m 0644 MidiDriver.h $(DESTDIR)/${includedir}/mi/midriver/MidiDriver.h
	install -D -m 0644 SerialDriver.h $(DESTDIR)/${includedir}/mi/midriver/SerialDriver.h


DESTDIR=
LIBDIR=/usr/lib
INCLUDEDIR=/usr/include

INCLUDES = -I. -I/.
CXX_FLAGS = -fpic -Wall
LD_FLAGS = -shared

SOURCES=GPIODriver.cpp I2CDriver.cpp SPIDriver.cpp
OBJECTS=$(SOURCES:.cpp=.o)

LIBRARYOUT=libmidriver.so


.cpp.o:
	$(CXX) $(CXX_FLAGS) $(INCLUDES) -c $<
$(LIBRARYOUT):$(OBJECTS)
	$(CXX) $(LD_FLAGS) $(OBJECTS) -o $(LIBRARYOUT)
clean:
	$(RM) $(LIBRARYOUT) *.o
	

install: $(LIBRARYOUT)
	mkdir -p $(DESTDIR)/$(LIBDIR)
	mkdir -p $(DESTDIR)/$(INCLUDEDIR)
	cp GPIODriver.h $(DESTDIR)/$(INCLUDEDIR)
	cp I2CDriver.h $(DESTDIR)/$(INCLUDEDIR)
	cp SPIDriver.h $(DESTDIR)/$(INCLUDEDIR)
	cp DriverBase.h $(DESTDIR)/$(INCLUDEDIR)
	cp $(LIBRARYOUT) $(DESTDIR)/$(LIBDIR)
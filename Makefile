STAGING=/home/alexey/router/openwrt/trunk/staging_dir
STAGING_DIR=$(STAGING)/toolchain-mips_r2_gcc-4.6-linaro_uClibc-0.9.33.2
PATH_TO_BIN=$(STAGING_DIR)/bin
PATH_TO_USR=$(STAGING)/target-mips_r2_uClibc-0.9.33.2/usr
CC=$(PATH_TO_BIN)/mips-openwrt-linux-uclibc-g++
LD=$(PATH_TO_BIN)/mips-openwrt-linux-uclibc-g++
#CFLAGS=-c -I$(PATH_TO_USR)/include
#LDFLAGS= -L$(PATH_TO_USR)/lib
CFLAGS=-c -I$(STAGING_DIR)/include -Os -ffunction-sections -Wl,--gc-sections -fno-asynchronous-unwind-tables -Wl,--strip-all
LDFLAGS= -L$(STAGING_DIR)/lib -Wl,--gc-sections -Wl,--strip-all
SOURCES=processing.cpp mac_description.cpp power_time_description.cpp misc.cpp dumpData.cpp cfg.cpp
#SOURCES=reader.cpp mac_description.cpp power_time_description.cpp misc.cpp dumpData.cpp cfg.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=processing.exe
#EXECUTABLE=reader_router.exe

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

reader:
	g++ -o reader.exe reader.cpp misc.cpp dumpData.cpp mac_description.cpp power_time_description.cpp

netcat:
	$(CC) -I$(STAGING_DIR)/include -L$(STAGING_DIR)/lib -o netcat netcat.cpp

install: $(EXECUTABLE)
	cp $(EXECUTABLE) ./firmware/files/$(EXECUTABLE)

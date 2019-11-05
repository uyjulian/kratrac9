
CC = i686-w64-mingw32-gcc
CXX = i686-w64-mingw32-g++
WINDRES := i686-w64-mingw32-windres
GIT_TAG := $(shell git describe --abbrev=0 --tags)
INCFLAGS += -I. -I.. -I../libatrac9/src -Iinclude
ALLSRCFLAGS += $(INCFLAGS) -DGIT_TAG=\"$(GIT_TAG)\"
CFLAGS += -O2 -flto
CFLAGS += $(ALLSRCFLAGS) -Wall -Wno-unused-value -Wno-format -DNDEBUG -DWIN32 -D_WIN32 -D_WINDOWS 
CFLAGS += -D_USRDLL -DMINGW_HAS_SECURE_API -DUNICODE -D_UNICODE -DNO_STRICT -D_GNU_SOURCE
CFLAGS += -DUSE_OPEN_SOURCE_LIBRARY
CXXFLAGS += $(CFLAGS) -fpermissive
WINDRESFLAGS += $(ALLSRCFLAGS) --codepage=65001
LDFLAGS += -static -static-libstdc++ -static-libgcc -shared -Wl,--kill-at
LDLIBS += -luuid

%.o: %.c
	@printf '\t%s %s\n' CC $<
	$(CC) -c $(CFLAGS) -o $@ $<

%.o: %.cpp
	@printf '\t%s %s\n' CXX $<
	$(CXX) -c $(CXXFLAGS) -o $@ $<

%.utf8.rc: %.rc
	@printf '\t%s %s\n' ICONV $<
	iconv -f UTF-16 -t UTF8 $< | sed 's/\.rc/.utf8.rc/g' > $@

%.o: %.utf8.rc
	@printf '\t%s %s\n' WINDRES $<
	$(WINDRES) $(WINDRESFLAGS) $< $@

DLL_SOURCES := tp_stub.cpp Atrac9MainUnit.cpp at9wave.cpp tvpsnd.c
# Resource.utf8.rc
LIBATRAC9_SOURCES := ../libatrac9/src/band_extension.c ../libatrac9/src/bit_allocation.c ../libatrac9/src/bit_reader.c ../libatrac9/src/decinit.c ../libatrac9/src/decoder.c ../libatrac9/src/huffCodes.c ../libatrac9/src/imdct.c ../libatrac9/src/libatrac9.c ../libatrac9/src/quantization.c ../libatrac9/src/scale_factors.c ../libatrac9/src/tables.c ../libatrac9/src/unpack.c ../libatrac9/src/utility.c

SOURCES := $(DLL_SOURCES) $(LIBATRAC9_SOURCES)
OBJECTS := $(SOURCES:.c=.o)
OBJECTS := $(OBJECTS:.cpp=.o)
OBJECTS := $(OBJECTS:.utf8.rc=.o)

BINARY ?= kratrac9.dll
ARCHIVE ?= kratrac9.$(GIT_TAG).7z

all: $(BINARY)

archive: $(ARCHIVE)

clean:
	rm -f $(OBJECTS) $(BINARY) $(ARCHIVE)

$(ARCHIVE): $(BINARY)
	rm -f $(ARCHIVE)
	7z a $@ $^

$(BINARY): $(OBJECTS) 
	@printf '\t%s %s\n' LNK $@
	$(CXX) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)


LIBATRAC9_SOURCES += libatrac9/src/band_extension.c libatrac9/src/bit_allocation.c libatrac9/src/bit_reader.c libatrac9/src/decinit.c libatrac9/src/decoder.c libatrac9/src/huffCodes.c libatrac9/src/imdct.c libatrac9/src/libatrac9.c libatrac9/src/quantization.c libatrac9/src/scale_factors.c libatrac9/src/tables.c libatrac9/src/unpack.c libatrac9/src/utility.c

SOURCES += kratrac9/Atrac9MainUnit.cpp kratrac9/at9wave.cpp
SOURCES += $(LIBATRAC9_SOURCES)

INCFLAGS += -Ilibatrac9/src -Ikratrac9/include
CFLAGS += -DUSE_OPEN_SOURCE_LIBRARY

PROJECT_BASENAME = kratrac9

USE_TVPSND = 1

include external/tp_stubz/Rules.lib.make

#This makefile should be used with a Gnu Make compatible
#make program.
#
#When the Gnu Make is not available,simply comment out
#the lines defining the test of OS:
#   ifeq ($(OS),Darwin)
#      LDFLAGS += -L/opt/local/lib -lusb-1.0
#   endif
#
#When running on Mac OS X system, please define the variable
#"OS" with the value "Darwin" by running the command
#"make OS=Darwin"
LIB_AND_APPS_PATH = ../../Libs-Apps/
QUANTIS_LIB_PATH = $(LIB_AND_APPS_PATH)build/Quantis
QUANTIS_EXT_LIB_PATH = $(LIB_AND_APPS_PATH)build/QuantisExtensions
QUANTIS_AIS_LIB_PATH = $(LIB_AND_APPS_PATH)build/QuantisAis31
QUANTIS_INC_PATH = $(LIB_AND_APPS_PATH)


CFLAGS += -I$(QUANTIS_INC_PATH)
CFLAGS += -O3
LDFLAGS = -L$(QUANTIS_LIB_PATH) -lQuantis -lusb-1.0 -pthread -lm
#LDFLAGS = -lusb-1.0 -lQuantis -pthread
#LDFLAGS += -L$(QUANTIS_EXT_LIB_PATH) -lQuantis_Extensions

ifeq ($(OS),Darwin)
  LDFLAGS += -L/opt/local/lib -lusb-1.0
else
  LDFLAGS += -lrt
endif

LINKER_FLAGS += -static -i-static

qrng-sources := \
	getopt.c \
	qrng.c \

qrng-objects := $(qrng-sources:.c=.c.o)



qrng-extensions-sources := \
	getopt.c \
	qrng_extensions.c \

qrng-extensions-objects := $(qrng-extensions-sources:.c=.c.o)



qrng-ais31-sources := \
	getopt.c \
	qrng_ais31.c \

qrng-ais31-objects := $(qrng-ais31-sources:.c=.c.o)



all: qrng qrng-extensions

qrng: $(qrng-objects)
	@echo "-->  Linking executable $@"
	$(CC) -o $@ $(qrng-objects) $(LDFLAGS)

qrng-extensions: $(qrng-extensions-objects)
	@echo "-->  Linking executable $@"
	$(CC) -o $@ $(qrng-extensions-objects) -L$(QUANTIS_EXT_LIB_PATH) -lQuantis_Extensions $(LDFLAGS)

qrng-ais31: $(qrng-ais31-objects)
	@echo "-->  Linking executable $@"
	$(CC) -o $@ $(qrng-ais31-objects) -L$(QUANTIS_AIS_LIB_PATH) -lQuantis_Ais31 -lssl -lcrypto $(LDFLAGS)



# qrng with hardware-less library
#qrng-nohw: $(qrng-objects)
#	@echo "Objects: $(qrng-objects)"
#	@echo "-->  Linking executable $@"
#	$(CXX) -o $@ $(qrng-objects) -L$(QUANTIS_LIB_PATH) -lQuantis-NoHw

clean:
	@rm -rf *.o *~ qrng qrng-extensions qrng-ais31

%.c.o: %.c
	@echo "->  Building C object $@"
	$(CC) -o $@ $< -c $(CFLAGS)


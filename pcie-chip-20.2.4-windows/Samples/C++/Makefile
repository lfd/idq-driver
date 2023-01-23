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
CXXFLAGS += -I$(QUANTIS_INC_PATH)

LDFLAGS = -L$(QUANTIS_LIB_PATH) -lQuantis -lusb-1.0

ifeq ($(OS),Darwin)
  LDFLAGS += -L/opt/local/lib -lusb-1.0
endif

qrng-sources := \
	getopt.c \
	qrng.cpp

qrng-objects := $(qrng-sources:.c=.c.o)
qrng-objects := $(qrng-objects:.cpp=.cpp.o)


qrng-extensions-sources := \
	getopt.c \
	qrng_extensions.cpp

qrng-extensions-objects := $(qrng-extensions-sources:.c=.c.o)
qrng-extensions-objects := $(qrng-extensions-objects:.cpp=.cpp.o)


qrng-ais31-sources := \
	getopt.c \
	qrng_ais31.cpp

qrng-ais31-objects := $(qrng-ais31-sources:.c=.c.o)
qrng-ais31-objects := $(qrng-ais31-objects:.cpp=.cpp.o)





all: qrng qrng-extensions


qrng: $(qrng-objects)
	@echo "-->  Linking executable $@"
	$(CXX) -o $@ $(qrng-objects) $(LDFLAGS)


qrng-extensions: $(qrng-extensions-objects)
	@echo "-->  Linking executable $@"
	$(CXX) -o $@ $(qrng-extensions-objects) $(LDFLAGS) -L$(QUANTIS_EXT_LIB_PATH) -lQuantis_Extensions


qrng-ais31: $(qrng-ais31-objects)
	@echo "-->  Linking executable $@"
	$(CXX) -o $@ $(qrng-ais31-objects) $(LDFLAGS) -L$(QUANTIS_AIS_LIB_PATH) -lQuantis_Ais31 -lssl -lcrypto


clean:
	rm -rf *.o *~ qrng qrng-extensions qrng-ais31


%.cpp.o: %.cpp
	@echo "->  Building CXX object $@"
	$(CXX) -c $(CXXFLAGS) -o $@ $<
 
%.c.o: %.c
	@echo "->  Building C object $@"
	$(CXX) -c $(CXXFLAGS) -o $@ $<


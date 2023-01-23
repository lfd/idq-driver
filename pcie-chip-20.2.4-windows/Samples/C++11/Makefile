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
QUANTIS_LIB_PATH = $(LIB_AND_APPS_PATH)build/Quantis/
QUANTIS_INC_PATH = $(LIB_AND_APPS_PATH)Quantis

CFLAGS += -I$(QUANTIS_INC_PATH)
CXXFLAGS += -I$(QUANTIS_INC_PATH)
LDFLAGS = -lusb-1.0 -L$(QUANTIS_LIB_PATH) -lQuantis 

ifeq ($(OS),Darwin)
  LDFLAGS += -L/opt/local/lib -lusb-1.0
endif

qrng-sources := \
	getopt.c \
	qrng.cpp

qrng-objects := $(qrng-sources:.c=.c.o)
qrng-objects := $(qrng-objects:.cpp=.cpp.o)

all: qrng

qrng: $(qrng-objects)
	@echo "-->  Linking executable $@"
	$(CXX) -o $@ $(qrng-objects) $(LDFLAGS)

# qrng with hardware-less library
#qrng-nohw: $(qrng-objects)
#	@echo "Objects: $(qrng-objects)"
#	@echo "-->  Linking executable $@"
#	$(CXX) -o $@ $(qrng-objects) -L$(QUANTIS_LIB_PATH) -lQuantis-NoHw

clean:
	rm -rf *.o *~ qrng

%.cpp.o: %.cpp
	@echo "->  Building CXX object $@"
	$(CXX) -c $(CXXFLAGS) -o $@ $<
 
%.c.o: %.c
	@echo "->  Building C object $@"
	$(CXX) -c $(CXXFLAGS) -o $@ $<


CC      = gcc
CXX     = g++
CFLAGS  = -g -O3 -Wall -std=c++0x
LIBS    =
LDFLAGS = -g

OBJECTS = Conf.o Log.o Network.o StopWatch.o Timer.o UDPSocket.o Utils.o YSFReflector.o

all:		YSFReflector

YSFReflector:	$(OBJECTS)
		$(CXX) $(OBJECTS) $(CFLAGS) $(LIBS) -o YSFReflector

%.o: %.cpp
		$(CXX) $(CFLAGS) -c -o $@ $<

clean:
		$(RM) YSFReflector *.o *.d *.bak *~
 
#Makefile
# Created on: 2017/01/19
#     Author: sin


CXX = g++ -std=c++11
CXXFLAGS = -Wall -O2 -g3
INCLUDES = -I./

%.o: %.c
	$(CC) $(CCFLAGS) $(INCLUDES) -c $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $<

#
LIBOBJS = actrie.o dawg.o

all: acdawg

acdawg:  main.o $(LIBOBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o acdawg.exe main.o $(LIBOBJS)
	
ac: aho.o $(LIBOBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o ac.exe aho.o $(LIBOBJS)


clean: 
	rm *.o *.exe #*# *~

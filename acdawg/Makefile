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

all: aho-dawg

aho-dawg:  main.o $(LIBOBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o aho-dawg.exe main.o $(LIBOBJS)
	
aho: aho.o $(LIBOBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o aho.exe aho.o $(LIBOBJS)


clean: 
	rm *.o *.exe #*# *~

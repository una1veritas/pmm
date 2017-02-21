#Makefile
# Created on: 2017/01/19
#     Author: sin


CXX = g++ -std=c++11
CXXFLAGS = -Wall -O2 -g3
INCDIRS = -I./

INCLUDES = dawg.h actrie.h

%.o: %.c
	$(CC) $(CCFLAGS) $(INCDIRS) -c $<

%.o: %.cpp $(INCLUDES)
	$(CXX) $(CXXFLAGS) $(INCDIRS) -c $<

#
LIBOBJS = actrie.o dawg.o

all: acdawg

acdawg:  main.o $(LIBOBJS) $(INCLUDES)
	$(CXX) $(CXXFLAGS) $(INCDIRS) -o acdawg.exe main.o $(LIBOBJS)

ac: aho.o $(LIBOBJS)
	$(CXX) $(CXXFLAGS) $(INCDIRS) -o ac.exe aho.o $(LIBOBJS)


clean: 
	rm -rf *.o *.exe *.dSYM #*# *~

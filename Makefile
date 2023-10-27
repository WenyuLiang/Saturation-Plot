CC=g++
GCC=gcc
INCLUDE=-I/share/home/liangwy/biotool/htslib 
CFLAGS=-g -O3 -Wall -Wextra -funroll-loops -march=native
CXXFLAGS=$(CFLAGS) -std=c++17
TARGETS=satu2  
OBJS=satu2.o  
BINDIR=/usr/local/bin
LDFLAGS = -L/share/home/liangwy/biotool/htslib -lhts -lz -lbz2 -lpthread -lrt 
all: $(TARGETS)

satu2: satu2.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

readmarker: readmarker.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

saturation: saturation.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.cpp  
	$(CC) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

%.o: %.c
	$(GCC) $(CFLAGS) $(INCLUDE) -c $< -o $@

.PHONY: clean

clean:
	rm -f $(OBJS) $(TARGETS)

format:
	clang-format -i --style=GNU *.cpp *.h

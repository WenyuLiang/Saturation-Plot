CC=g++
GCC=gcc
INCLUDE=-I/usr/local/include  
CFLAGS=-g -O3 -Wall -Wextra -funroll-loops -march=native
CXXFLAGS=$(CFLAGS) -std=c++17
TARGETS=satu2 readmarker saturation
OBJS=satu2.o readmarker.o saturation.o 
BINDIR=/usr/local/bin
LDFLAGS = -L/usr/lib64 -L/usr/local/lib -lhts -lz -lbz2 -lpthread -lrt 
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

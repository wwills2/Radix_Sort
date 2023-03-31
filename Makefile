CC=g++
CFLAGS_SERIAL= -Wall -g3 -pedantic -std=c++14

all: serial_radix

serial_radix: SerialRadix.h SerialRadix.cpp serial_driver.cpp
	$(CC) $(CFLAGS_SERIAL) SerialRadix.cpp serial_driver.cpp -o serial_radix.exe

clean:
	rm -rf serial_radix.exe

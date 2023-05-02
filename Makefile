CC=g++
CFLAGS_OPTIMIZED= -Wall -O3 -pedantic -std=c++14
CFLAGS_DEBUG= -Wall -g3 -pedantic -std=c++14

all: serial_radix

serial_radix: SerialRadix.h SerialRadix.cpp serial_driver.cpp
	$(CC) $(CFLAGS_OPTIMIZED) SerialRadix.cpp serial_driver.cpp -o serial_radix.exe

serial_radix_debug: SerialRadix.h SerialRadix.cpp serial_driver.cpp
	$(CC) $(CFLAGS_DEBUG) SerialRadix.cpp serial_driver.cpp -o serial_radix.exe

clean:
	rm -rf serial_radix.exe

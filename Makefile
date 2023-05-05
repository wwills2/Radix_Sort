CC=g++
CFLAGS_OPTIMIZED= -Wall -O3 -pedantic -std=c++14
CFLAGS_DEBUG= -Wall -g3 -pedantic -std=c++14

all: serial_radix

serial_radix: SerialRadix.h SerialRadix.cpp serial_driver.cpp timer.c
	$(CC) $(CFLAGS_OPTIMIZED) SerialRadix.cpp serial_driver.cpp timer.c -o serial_radix.exe

serial_radix_debug: SerialRadix.h SerialRadix.cpp serial_driver.cpp timer.c
	$(CC) $(CFLAGS_DEBUG) SerialRadix.cpp serial_driver.cpp timer.c -o serial_radix.exe

mpi_quick_sort: jw_parallel_quick_sort.c timer.c
	mpicc -Wall -O3 -pedantic jw_parallel_quick_sort.c timer.c -o jw_parallel_quick_sort.exe

clean:
	rm -rf serial_radix.exe
	rm -rf jw_parallel_quick_sort.exe

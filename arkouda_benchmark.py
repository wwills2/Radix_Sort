"""
Driver used to run radix sort benchmarks. MAX, MIN, TEST_SIZE, and num_threads were manually varied to generate the
test cases. NOTE: this script must be run on a system which has the chapel programming language and the arkouda
exploratory data analysis suite installed. The script should be run in the same directory as the arkouda Makefile.
"""

import time
import arkouda as ak
import os

MAX = 99999
MIN = 10000
TEST_SIZE = 500000000
num_threads = 4

os.environ["CHPL_RT_NUM_THREADS_PER_LOCALE"] = str(num_threads)
pid = os.fork()
if pid == 0:
    os.execl("./arkouda_server", "-nl 1")

# connect to arkouda server at local host
ak.connect()
print("\n****************************************************************\n")
print("building random list of size", TEST_SIZE)
print("\n****************************************************************\n")

# build random list to sort
to_sort = ak.randint(MIN, MAX, TEST_SIZE)
print("\n****************************************************************\n")
print("sorting the list")
print("\n****************************************************************\n")

start_time = time.time()
# get the sorted list
sorted_nums = ak.sort(to_sort)

print("\n****************************************************************\n")
print("sort complete")
print("RUNTIME:", time.time() - start_time, "seconds")
print("\n****************************************************************\n")

# shut server down
ak.shutdown()





"""
This file was used to collect the runtime metrics for the arkouda sort

Important: This file is in this repository for reference only. This file cannot be run in the absence of a arkouda
           installation
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





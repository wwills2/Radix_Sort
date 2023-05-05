/* !!! IMPORTANT !!!
    This file is the intellectual property of Bears-R-Us and is taken from the Arkouda Data Analytics Python API for
    super computing. None of the below code is my own work and I do not own any rights to the code. The original file is
    located within the src/ directory on the master branch of the Arkouda repository. The repository can be found at
    https://github.com/Bears-R-Us/arkouda and the original file can be opened directly at
    https://github.com/Bears-R-Us/arkouda/blob/master/src/RadixSortLSD.chpl

    All block comments prefaced with "to-do" are my analysis of the code.
 */



/* Radix Sort Least Significant Digit */
module RadixSortLSD
{
    config const RSLSD_vv = false;
    const vv = RSLSD_vv; // these need to be const for comms/performance reasons

    /* todo: config keyword: variables that can be over written on the command line. argument flags are added when the
            config keyword is used. <--RSLSD_numTasks 100> on the command line as an argument would set RSLSD_numTasks
            to 100
    */
    config const RSLSD_numTasks = here.maxTaskPar; // tasks per locale based on locale0
    const numTasks = RSLSD_numTasks; // tasks per locale
    const Tasks = {0..#numTasks}; // these need to be const for comms/performance reasons
    /* todo: locale refers to a compute node. in our case, this would map to a core i believe */

    /*todo: the param key word specifies value whose size must be known to the compiler */
    private param bitsPerDigit = RSLSD_bitsPerDigit; // these need to be const for comms/performance reasons
    private param numBuckets = 1 << bitsPerDigit; // these need to be const for comms/performance reasons

    use BlockDist; 
    use BitOps;
    use AryUtil;
    use CommAggregation;
    use IO;
    use CTypes;
    use Reflection;
    use RangeChunk;
    use Logging;
    use ServerConfig;

    use ArkoudaBlockCompat; /*todo: there are several variations of this, but most seem to be slight variations of the
                                BlockDist module. Seems to be returning a block dist of a certain size for the arkouda
                                application */

    private config const logLevel = ServerConfig.logLevel;
    private config const logChannel = ServerConfig.logChannel;
    const rsLogger = new Logger(logLevel, logChannel);




    /* todo: a record is the same thing as a class; methods and data fields encapsulated, there are differences though.
            records DO NOT support inheritance and virtualization ("virtual dispatch"). a record variable must refer
            to UNIQUE memory (multiple variables cannot map to the same record). records DO support copy initialization
            and assignment, but classes DO NOT */
    record KeysComparator {
      inline proc key(k) { return k; }
    }




    record KeysRanksComparator {
      inline proc key(kr) { const (k, _) = kr; return k; }
    }




    /* todo: proc keyword declares a function (proc == "procedure") */
    // calculate sub-domain for task
    inline proc calcBlock(task: int, low: int, high: int) {

        var totalsize = high - low + 1;
        var div = totalsize / numTasks;
        var rem = totalsize % numTasks;
        var rlow: int;
        var rhigh: int;

        if (task < rem) {

            /* todo: the lower bound of the subdomain will be the size of each subdomain (+ 1) multiplied by the task
                    number, incremented by the data set's smallest number */
            rlow = task * (div+1) + low;
            rhigh = rlow + div;
        }
        else {

            /* todo: if the task number is greater than the remainder of the data partitioning, then section off a
                     partition size within the remainder */
            rlow = task * div + rem + low;
            rhigh = rlow + div - 1;
        }

        return {rlow .. rhigh};
    }




    // calc global transposed index
    // (bucket,loc,task) = (bucket * numLocales * numTasks) + (loc * numTasks) + task;
    inline proc calcGlobalIndex(bucket: int, loc: int, task: int): int {
        return ((bucket * numLocales * numTasks) + (loc * numTasks) + task);
    }




    /* Radix Sort Least Significant Digit
       In-place radix sort a block distributed array
       comparator is used to extract the key from array elements
     */
    /*todo: the question mark operator queries the type of a variable. in the below line, it is getting the type of aD
            then setting then casting the parameter 'a' to an array consisting of elements with the type of aD */
    private proc radixSortLSDCore(a:[?aD] ?t, nBits, negs, comparator) {
        try! rsLogger.debug(getModuleName(),getRoutineName(),getLineNumber(),
                                       "type = %s nBits = %t".format(t:string,nBits));
        var temp = a;
        
        // create a global count array to scan
        /* todo: the # generates a counted range. so 0..#(n) represents the range from 0 to n. in this case n is
                the ~number of locales~ * ~number of tasks~ * ~number of buckets~. variable gD is a distributed domain
                 with n sub domains */
        var gD = Block.createDomain({0..#(numLocales * numTasks * numBuckets)});
        var globalCounts: [gD] int;


        // loop over digits todo: via binary rightshift
        for rshift in {0..#nBits by bitsPerDigit} {
            const last = (rshift + bitsPerDigit) >= nBits;
            try! rsLogger.debug(getModuleName(),getRoutineName(),getLineNumber(),
                                                        "rshift = %t".format(rshift));
            // count digits
            /* todo: Locales seems to be a built-in structure representing the UID's of the processing units.
                    the coforall here is semantically equivalent to dispatching a pthread to each core. */
            coforall loc in Locales {

                /* todo: <on loc> tells the compiler to dispatch the work within the block to parallel threads */
                on loc {

                    /*_____________________________PARALLEL BY LOCALE_____________________________*/

                    // allocate counts
                    /* todo: allocate an array with the length of the number of tasks, then for each index, set its
                            value to be a counting range of numbers from 0 to numBuckets */
                    var tasksBucketCounts: [Tasks] [0..#numBuckets] int;


                    /* todo: the nested coforall here maps to the same threads as the outer coforall. for each task
                            number */
                    coforall task in Tasks {

                        /*_____________________________PARALLEL BY TASK_____________________________*/

                        /* todo: this ref variable here creates a reference to the appropriate task's buckets, which
                                are contained in the 2D var tasksBucketCounts: [Tasks] [0..#numBuckets] int; above */
                        ref taskBucketCounts = tasksBucketCounts[task];

                        // get local domain's indices
                        var lD = aD.localSubdomain();

                        // calc task's indices from local domain's indices
                        var tD = calcBlock(task, lD.low, lD.high);

                        /* todo: loop over */
                        // count digits in this task's part of the array
                        for i in tD {

                            /* todo: temp here is a copy of input array 'a'. the key is the value of the copy
                                array at i*/
                            const key = comparator.key(temp.localAccess[i]);

                            /*todo getDigit() line 264 AryUtil.chpl, returns the current binary digit */
                            var bucket = getDigit(key, rshift, last, negs); // calc bucket from key

                            /*todo: increment bucket which matches with the key */
                            taskBucketCounts[bucket] += 1;
                        }

                        /*_____________________________END PARALLEL BY TASK_____________________________*/

                    }//coforall task

                    // write counts in to global counts in transposed order
                    coforall tid in Tasks {

                        /*_____________________________PARALLEL BY TASK_____________________________*/

                        /* todo: newDstAggregator from CommAggregation.chpl line 16. performs an out of order
                                aggregation of the inputs when the source is local and the destination is global.
                                In this case, used to sum all counts in a given local proc bucket, and then copy that
                                sum to the corresponding index in the global counts */
                        var aggregator = newDstAggregator(int);

                        for task in Tasks {

                            ref taskBucketCounts = tasksBucketCounts[task];

                            /*todo: aggregator.copy(destination, source), over all buckets: local -> global */
                            for bucket in chunk(0..#numBuckets, numTasks, tid) {
                                aggregator.copy(globalCounts[calcGlobalIndex(bucket, loc.id, task)],
                                                             taskBucketCounts[bucket]);
                            }
                        }

                        /* todo: makes the aggregation result visible in dest, completes the aggregation. */
                        aggregator.flush();

                        /*_____________________________END PARALLEL BY TASK_____________________________*/

                    }//coforall task

                    /*_____________________________END PARALLEL BY LOCALE_____________________________*/
                }//on loc
            }//coforall loc

            /* todo: at this point, the buckets have been counted up, the next step is placing them back into the list
                    in sorted order. */

            // scan globalCounts to get bucket ends on each locale/task
            var globalStarts = + scan globalCounts; //todo: not sure exactly how the open ended + op with scan works
            globalStarts -= globalCounts;
            
            if vv {printAry("globalCounts =",globalCounts);try! stdout.flush();}
            if vv {printAry("globalStarts =",globalStarts);try! stdout.flush();}

            // calc new positions and permute
            coforall loc in Locales {

                on loc {

                    /*_____________________________PARALLEL BY LOCALE_____________________________*/

                    // allocate counts
                    var tasksBucketPos: [Tasks] [0..#numBuckets] int;

                    // read start pos in to globalStarts back from transposed order
                    coforall tid in Tasks {

                        /*_____________________________PARALLEL BY TASK_____________________________*/

                        /* todo: source aggregator does the same thing as destination aggregator, but it copies from
                                global to local buffers */
                        var aggregator = newSrcAggregator(int);

                        for task in Tasks {

                            ref taskBucketPos = tasksBucketPos[task];

                            /*todo: aggregator.copy(destination, source), over all buckets: global -> local. used to
                                    copy data from the global buffer back to a buffer local to the thread.*/
                            for bucket in chunk(0..#numBuckets, numTasks, tid) {
                              aggregator.copy(taskBucketPos[bucket],
                                         globalStarts[calcGlobalIndex(bucket, loc.id, task)]);
                            }
                        }

                        /* todo: makes the aggregation result visible in dest, completes the aggregation. */
                        aggregator.flush();

                        /*_____________________________END PARALLEL BY TASK_____________________________*/

                    }//coforall task

                    /* todo: writes values back to the count */
                    coforall task in Tasks {

                        /*_____________________________PARALLEL BY TASK_____________________________*/

                        ref taskBucketPos = tasksBucketPos[task];

                        // get local domain's indices
                        var lD = aD.localSubdomain();

                        // calc task's indices from local domain's indices
                        var tD = calcBlock(task, lD.low, lD.high);

                        // calc new position and put data there in temp
                        {
                            var aggregator = newDstAggregator(t);
                            for i in tD {
                                const ref tempi = temp.localAccess[i];
                                const key = comparator.key(tempi);
                                var bucket = getDigit(key, rshift, last, negs); // calc bucket from key
                                var pos = taskBucketPos[bucket];
                                taskBucketPos[bucket] += 1;
                                aggregator.copy(a[pos], tempi);
                            }
                            aggregator.flush();
                        }

                        /*_____________________________END PARALLEL BY TASK_____________________________*/

                    }//coforall task

                    /*_____________________________END PARALLEL BY LOCALE_____________________________*/

                }//on loc
            }//coforall loc

            // copy back to temp for next iteration
            // Only do this if there are more digits left
            if !last {
              temp <=> a;
            }
        } // for rshift
    }//proc radixSortLSDCore


    // todo: all of these are different entry points into the sort. radixSortLSDCore is the main function


    proc radixSortLSD(a:[?aD] ?t, checkSorted: bool = true): [aD] (t, int) {
        var kr: [aD] (t,int) = [(key,rank) in zip(a,aD)] (key,rank);
        if (checkSorted && isSorted(a)) {
            return kr;
        }
        var (nBits, negs) = getBitWidth(a);
        radixSortLSDCore(kr, nBits, negs, new KeysRanksComparator());
        return kr;
    }




    /* Radix Sort Least Significant Digit
       radix sort a block distributed array
       returning a permutation vector as a block distributed array */
    proc radixSortLSD_ranks(a:[?aD] ?t, checkSorted: bool = true): [aD] int {
        if (checkSorted && isSorted(a)) {
            var ranks: [aD] int = [i in aD] i;
            return ranks;
        }

        var kr: [aD] (t,int) = [(key,rank) in zip(a,aD)] (key,rank);
        var (nBits, negs) = getBitWidth(a);
        radixSortLSDCore(kr, nBits, negs, new KeysRanksComparator());
        var ranks: [aD] int = [(_, rank) in kr] rank;
        return ranks;
    }




    /* Radix Sort Least Significant Digit
       radix sort a block distributed array
       returning sorted keys as a block distributed array */
    proc radixSortLSD_keys(a: [?aD] ?t, checkSorted: bool = true): [aD] t {
        var copy = a;
        if (checkSorted && isSorted(a)) {
            return copy;
        }
        var (nBits, negs) = getBitWidth(a);
        radixSortLSDCore(copy, nBits, negs, new KeysComparator());
        return copy;
    }




    proc radixSortLSD_memEst(size: int, itemsize: int) {
        // 2 temp key+ranks arrays + globalStarts/globalClounts
        return (2 * size * (itemsize + numBytes(int))) +
               (2 * numLocales * numTasks * numBuckets * numBytes(int));
    }




    proc radixSortLSD_keys_memEst(size: int, itemsize: int) {
        // 2 temp key arrays + globalStarts/globalClounts
        return (2 * size * itemsize) +
               (2 * numLocales * numTasks * numBuckets * numBytes(int));

    }
}


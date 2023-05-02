//
// Created by William Wills on 3/29/23.
//

#include "SerialRadix.h"

// prints the values of m_integers and their digits vector
void SerialRadix::printList(std::vector<int> &list) {

    for (int i = 0; i < (int) list.size(); i++){
        if (i % 15 == 0){
            std::cout << std::endl;
        }
        std::cout << list[i] << " ";
    }
    std::cout << std::endl;
}

// entry point into the serial radix sort
void SerialRadix::radixSort(std::vector<int> &list){

    //todo: add support for numbers of varying lengths

    std::vector<std::deque<int> > buckets;
    //populate m_buckets with queues
    for (int j = 0; j < 10; j++){

        std::deque<int> bucket;
        buckets.push_back(bucket);
    }

    const int digitCount = getDigitCount(list[0]);

    //used in loop
    int index;

    // look at all digits in each number. 'i' is the digit location. digits in ascending order, loop in reverse
    for (int i = 0; i < digitCount; i++){

        // loop over all numbers in list
        for (int &currNum: list){

            //place number in bucket based on digit value
            buckets[getDigit(currNum, i)].push_back(currNum);
        }

        index = 0;

        // empty buckets back into sorted list
        for (std::deque<int> &bucket : buckets){

            while (!bucket.empty()){
                list[index] = bucket.front();
                bucket.pop_front();
                index++;
            }
        }
    }
}

int SerialRadix::getDigit(const int &num, const int &index){

    int tempNum = num;

    // divide by 10^(index_value), then get remainder of an additional divide by 10
    tempNum /= (int) std::pow(10, index);
    return tempNum % 10;
}

int SerialRadix::getDigitCount(const int &num){

    int tempNum = num;
    int digitCount = 0;

    // loop over all digits and get count
    bool parseNum = true;
    while(parseNum){

        tempNum = tempNum / 10;
        digitCount++;

        if (tempNum <= 0){
            parseNum = false;
        }
    }

    return digitCount;
}

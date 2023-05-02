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
    int digitValue; //used in loop

    // look at all digits in each number. 'i' is the digit location. digits in ascending order, loop in reverse
    for (int i = 0; i < digitCount; i++){

        // loop over all numbers in list
        for (int &currNum: list){

            digitValue = getDigit(currNum, i);

            //place number in bucket based on digit value
            buckets[digitValue].push_back(currNum);
        }

        int index = 0;

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

    if (index < 0){
        throw std::invalid_argument( "received negative digit index value" );
    }

    int tempNum = num;
    int digit;
    tempNum /= (int) std::pow(10, index);
    digit = tempNum % 10;

    return digit;
}

int SerialRadix::getDigitCount(const int &num){

    int tempNum = num;
    int digitCount = 0;

    // push digits of number into stack in reverse order
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

/* todo: remove
//overloaded Integer Subclass constructor
SerialRadix::Integer::Integer(int &inputNum) {

    num = inputNum;
    int tempNum = num;
    int currDigit = 1;
    std::stack<int> tempStack;

    // push digits of number into stack in reverse order
    bool parseNum = true;
    while(parseNum){

        currDigit = tempNum % 10;
        tempStack.push(currDigit);
        tempNum = tempNum / 10;

        if (tempNum <= 0){
            parseNum = false;
        }
    }

    // unload stack into m_digits vector
    while(!tempStack.empty()){
        m_digits.push_back(tempStack.top());
        tempStack.pop();
    }

    m_digit_count = (int) m_digits.size();
}
 */


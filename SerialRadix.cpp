//
// Created by William Wills on 3/29/23.
//

#include "SerialRadix.h"

// SerialRadix Class functions -----------------------------------------------------------------------------------------

// overloaded SerialRadix Constructor, takes in a vector of integer values
SerialRadix::SerialRadix(std::vector<int> &inputData){

    // create vector of integer objects representing each integer in the input data
    for (int num : inputData){
        Integer currInt(num);
        m_integers.push_back(currInt);
    }
}

// prints the values of m_integers and their digits vector
void SerialRadix::printData() {

    for (Integer _int: m_integers){
        std::cout << _int.getNumber() << " :: digits = ";
        for (int digit : _int.m_digits){
            std::cout << digit << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

// entry point into the serial radix sort
void SerialRadix::radixSort(){

    //todo: add support for numbers of varying lengths

    const int digitCount = m_integers[0].getDigitCount();
    int digitValue; //used in loop

    // look at all digits in each number. 'i' is the digit location. digits in ascending order, loop in reverse
    for (int i = digitCount - 1; i >= 0; i--){

        std::vector<std::deque<Integer> > buckets;
        //populate m_buckets with queues
        for (int i = 0; i < 10; i++){

            std::deque<Integer> bucket;
            buckets.push_back(bucket);
        }

        // loop over all numbers in list
        for (Integer numToSort : m_integers){

            digitValue = numToSort.m_digits[i];

            //place number in bucket based on digit value
            buckets[digitValue].push_back(numToSort);
        }

        int index = 0;

        // empty buckets back into sorted list
        for (std::deque<Integer> bucket : buckets){

            while (!bucket.empty()){
                m_integers[index] = bucket.front();
                bucket.pop_front();
                index++;
            }
        }
    }
}

std::vector<int> SerialRadix::getSortedList() {
    std::vector<int> sortedList(m_integers.size());

    for (int & i : sortedList){
        i = m_integers[i].getNumber();
    }

    return sortedList;
}

// Integer Class functions ---------------------------------------------------------------------------------------------

//overloaded Integer Subclass constructor
SerialRadix::Integer::Integer(int &inputNum) {

    m_number = inputNum;
    int tempNum = m_number;
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

//getter for private m_number
int SerialRadix::Integer::getNumber() {
    return m_number;
}

int SerialRadix::Integer::getDigitCount() {
    return m_digit_count;
}

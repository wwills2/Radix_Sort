//
// Created by William Wills on 3/29/23.
//

#include "SerialRadix.h"

// SerialRadix Class functions -----------------------------------------------------------------------------------------

// overloaded SerialRadix Constructor, takes in a vector of integer values
SerialRadix::SerialRadix(std::vector<int> &inputData){

    m_sortedList = std::vector<int>(inputData.size());

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
}

// entry point into the serial radix sort
std::vector<int> &SerialRadix::radixSort() {

    return m_sortedList;
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
}

//getter for private m_number
int SerialRadix::Integer::getNumber() {
    return m_number;
}

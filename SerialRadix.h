//
// Created by William Wills on 3/29/23.
//

#ifndef INC_483_PARALLEL_SORT_SERIALRADIX_H
#define INC_483_PARALLEL_SORT_SERIALRADIX_H

#include <vector>
#include <iostream>
#include <stack>
#include <cmath>

class SerialRadix {
public:
    // prints the values of m_integers and their digits vector
    static void printList(std::vector<int> &list);

    // entry point into the serial radix sort
    static void radixSort(std::vector<int> &list);

private:

    static int getDigit(const int &num, const int &index);

    static int getDigitCount(const int &num);
};


#endif //INC_483_PARALLEL_SORT_SERIALRADIX_H

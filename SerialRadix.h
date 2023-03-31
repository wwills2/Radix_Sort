//
// Created by William Wills on 3/29/23.
//

#ifndef INC_483_PARALLEL_SORT_SERIALRADIX_H
#define INC_483_PARALLEL_SORT_SERIALRADIX_H

#include <vector>
#include <iostream>
#include <stack>

class SerialRadix {
public:

    // overloaded SerialRadix Constructor, takes in a vector of integer values
    explicit SerialRadix(std::vector<int> &);

    // prints the values of m_integers and their digits vector
    void printData();

    // entry point into the serial radix sort
    std::vector<int> &radixSort();

private:

    class Integer{
    public:

        //overloaded Integer Subclass constructor
        explicit Integer(int &number);

        //getter for private m_number
        int getNumber();

        std::vector<int> m_digits;

    private:
        int m_number;
    };

    std::vector<Integer> m_integers;
    std::vector<int> m_sortedList;
};


#endif //INC_483_PARALLEL_SORT_SERIALRADIX_H

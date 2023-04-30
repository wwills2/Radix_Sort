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
   void radixSort();

private:

    class Integer{
    public:

        //overloaded Integer Subclass constructor
        explicit Integer(int &number);

        //getter for private m_number
        int getNumber();

        //getter for m_digit_count
        int getDigitCount();

        std::vector<int> m_digits;

    private:

        int m_number;
        int m_digit_count;
    };

    std::vector<std::deque<Integer> > m_buckets;
    std::vector<Integer> m_integers;
};


#endif //INC_483_PARALLEL_SORT_SERIALRADIX_H

//
// Created by William Wills on 3/29/23.
//

#include <iostream>
#include <random>

#include "SerialRadix.h"

enum RANDOM {UNIFORM, NORMAL};
class Random {
public:
    Random(int min, int max, RANDOM type=UNIFORM) : m_min(min), m_max(max), m_type(type)
    {
        if (type == NORMAL){
            m_generator = std::mt19937(m_device());
            //the data set will have the mean of 50 and standard deviation of 20
            m_normdist = std::normal_distribution<>(50,20);
        }
        else{
            // Using a fixed seed value generates always the same sequence
            // of pseudorandom numbers, e.g. reproducing scientific experiments
            // here it helps us with testing since the same sequence repeats
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_unidist = std::uniform_int_distribution<>(min,max);
        }
    }

    int getRandNum(){
        int result = 0;
        if(m_type == NORMAL){
            //returns a random number in a set with normal distribution
            //we limit random numbers by the min and max values
            result = m_min - 1;
            while(result < m_min || result > m_max)
                result = m_normdist(m_generator);
        }
        else{
            //this will generate a random number between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

    void buildRandomVector(std::vector<int> &randomList){
        for (int & i : randomList){
            i = getRandNum();
        }
    }

private:
    int m_min;
    int m_max;
    RANDOM m_type;
    std::random_device m_device;
    std::mt19937 m_generator;
    std::normal_distribution<> m_normdist;//normal distribution
    std::uniform_int_distribution<> m_unidist;//uniform distribution

};


int main(){

    std::cout << std::endl << "+++ small list smoke test +++" << std::endl;
    std::vector<int> smallTestNums = {123, 456, 789, 234, 567, 890};
    for (int num : smallTestNums){
        std::cout << num << " ";
    }
    std::cout << std::endl;

    SerialRadix serialRadix(smallTestNums);
    serialRadix.printData();

    serialRadix.radixSort();
    serialRadix.printData();


    std::cout << std::endl << "+++ 50 size unit test +++" << std::endl;
    Random rand(10000, 99999);

    int testSize = 50;
    std::vector<int> list(testSize);
    rand.buildRandomVector(list);

    for (int num : list){
        std::cout << num << " ";
    }
    std::cout << std::endl;

    // construct a sort object - list remains unchanged
    serialRadix = SerialRadix(list);
    serialRadix.printData();

    // sort the data internally
    serialRadix.radixSort();
    serialRadix.printData();

    // retrieve sorted list
    list = serialRadix.getSortedList();

    //validate sort
    int prevVal = 0;
    for (int i : list){
        if (i < prevVal){
            std::cerr << "out of order" << std::endl;
        }
        prevVal = i;
    }


    testSize = 5000000;
    std::cout << std::endl << "+++ " << testSize << " size unit test +++" << std::endl;

    std::cout << std::endl << "building random vector ->" << std::endl;
    list = std::vector<int>(testSize);
    rand.buildRandomVector(list);

    std::cout << "processing list -> " << std::endl;
    serialRadix = SerialRadix(list);

    std::cout << "sorting list -> " << std::endl;
    serialRadix.radixSort();

    std::cout << "copying to output buffer -> " << std::endl;
    list = serialRadix.getSortedList();

    std::cout << "validating sort: ";

    //validate sort
    prevVal = 0;
    for (int i : list){
        if (i < prevVal){
            std::cerr << "out of order" << std::endl;
            return -1;
        }
        prevVal = i;
    }

    std::cout << "VALIDATED" << std::endl;
}

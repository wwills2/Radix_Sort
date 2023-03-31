//
// Created by William Wills on 3/29/23.
//

#include <iostream>

#include "SerialRadix.h"

int main(){

    std::vector<int> smallTestNums = {123, 456, 789, 234, 567, 890};
    for (int num : smallTestNums){
        std::cout << num << " ";
    }
    std::cout << std::endl;

    SerialRadix serialRadix(smallTestNums);
    serialRadix.printData();
}

//Autor: Krystyna Łosieczka
//Temat: Restauracja

#include "Glass.h"
#include "Barman.h"

#include <array>
#include <iostream>


#define NUMOFGLASSES 3



int main() {
    std::cout << "Hello, World!" << std::endl;

    std::array<Glass, NUMOFGLASSES> glasses;

    for (int i=0; i < NUMOFGLASSES; i++){
        glasses.at(i).glassId = i+1;
    }



    return 0;
}
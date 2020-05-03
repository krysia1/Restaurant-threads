//Autor: Krystyna Łosieczka
//Temat: Restauracja

#include "Glass.h"
#include "Barman.h"

#include <array>
#include <iostream>
#include <mutex>


#define NUMOFGLASSES 3

std::mutex knifeMutex;

int main() {

    std::array<Glass, NUMOFGLASSES> glasses;

    for (int i=0; i < NUMOFGLASSES; i++){
        glasses.at(i).glassId = i+1;
    }



    return 0;
}
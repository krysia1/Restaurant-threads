//Autor: Krystyna Łosieczka
//Opis: Klasa Waiter reprezentująca Kelnera

#include "Waiter.h"
#include <thread>




void Waiter::clean(){
    for(int i=0; i<NUMOFGLASSES; i++){
        if (allGlasses[i].dirty){
            allGlasses[i].glassMutex.lock();
            allGlasses[i].state = 3; //stan na czyszczony

            int part = std::uniform_int_distribution<int>(25, 35)(rng);
            for (auto i = 1; i <= part; i++) {
                double p = (double) i / (double) part;
                progress = (int) std::round(p * 100.0);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            allGlasses[i].state = 0; //stan na czysty
            allGlasses[i].glassMutex.unlock();
        }
    }
}

void Waiter::serve(){

}



void Waiter::work(){
    while(!exitWaiter){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if(barman.askToClean){
            clean();
        }
        serve();
    }
}



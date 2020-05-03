//Autor: Krystyna Łosieczka
//Opis: Klasa Barman reprezentująca Barmana

#include "Barman.h"
#include <mutex>

std::mutex counter[COUNTERCAPACITY]; //miejsca przy kontuarze jako mutexy
std::mutex servedDrunkard;      //aktualnie obsługiwany pijak

void Barman::checkCounter(bool counterIsEmpty) {
    for (int i=0; i < COUNTERCAPACITY; i++){
        if(counter[i].try_lock()){
            counter[i].unlock();
        }else{
            counterIsEmpty = false;
        }
    }

    if(counterIsEmpty){
        if(servedDrunkard.try_lock()){
            int part = std::uniform_int_distribution<int>(25,35)(rng);
            for(auto i=1; i<=part; i++){
                double p = (double)i / (double)part;
                progress = (int)std::round(p * 100.0);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }





}

void Barman::serveDrunkard() {
    if(servedDrunkard.try_lock()){
        servedDrunkard.unlock();
    }else{

    }

}

void Barman::live(){
    while(!exit) {
        checkCounter(counterIsEmpty);
        checkGlasses();
    }
}


Barman::Barman(Glass (&glassArray)[NUMOFGLASSES]) : allGlasses(glassArray), barmanThread(&Barman::live, this) {}

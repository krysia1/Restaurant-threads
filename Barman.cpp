//Autor: Krystyna Łosieczka
//Opis: Klasa Barman reprezentująca Barmana

#include "Barman.h"
#include <mutex>

std::mutex counter[COUNTERCAPACITY]; //miejsca przy kontuarze jako mutexy
std::mutex servedDrunkard;      //aktualnie obsługiwany pijak


//todo
//add status to show with ncurses

void Barman::checkGlasses() {
    for(int i=0; i<NUMOFGLASSES; i++){
        if (allGlasses[i].dirty){
            dirtyGlasses++;
        }
    }
    if(dirtyGlasses > 1){
        askToClean = true;
    }

}

void Barman::tryGlasses(){
    for(int i=0; i<NUMOFGLASSES; i++){      //próba wzięcia szklanek
        if(allGlasses[i].glassMutex.try_lock()){
            if(!allGlasses[i].dirty) {
                if (!allGlasses[i].taken) {
                    allGlasses[i].taken = true;
                    pickedGlasses++;
                    for (int j = 0; j < NUMOFGLASSES; j++) {
                        if (i == j) {
                            continue;
                        }

                        if (allGlasses[j].glassMutex.try_lock()) {
                            if (!allGlasses[j].dirty) {
                                if (!allGlasses[j].taken) {
                                    allGlasses[j].taken = true;
                                    pickedGlasses++;
                                } else {
                                    allGlasses[j].glassMutex.unlock();
                                }
                            }
                        }

                        if (pickedGlasses == 2) {
                            break;
                        }
                    }
                } else {
                    allGlasses[i].glassMutex.unlock();
                }
            }
        }
        if(pickedGlasses == 2){
            break;
        }
    }
}


void Barman::serveDrunkard() {

    tryGlasses();

    if(pickedGlasses < 2){      //jeśli udało się wziąć tylko jedną szklankę to ją odkłada
        for(int i=0; i<NUMOFGLASSES; i++) {
            if (allGlasses[i].taken){
                allGlasses[i].taken = false;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(30));

    }else{


        for(int i=0; i<NUMOFGLASSES; i++) {
            if (allGlasses[i].taken){
                allGlasses[i].dirty = true;
            }
        }


        int part = std::uniform_int_distribution<int>(25, 35)(rng);
        for (auto i = 1; i <= part; i++) {
            double p = (double) i / (double) part;
            progress = (int) std::round(p * 100.0);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }


        for(int i=0; i<NUMOFGLASSES; i++){
            if (allGlasses[i].dirty){
                allGlasses[i].glassMutex.unlock();
            }
        }
    }

    checkGlasses();

}


void Barman::rest(){
    int part = std::uniform_int_distribution<int>(25,35)(rng);
    for(auto i=1; i<=part; i++){
        double p = (double)i / (double)part;
        progress = (int)std::round(p * 100.0);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}


void Barman::checkCounter(bool counterIsEmpty) {
    while(!exit) {
        for (int i = 0; i < COUNTERCAPACITY; i++) {
            if (counter[i].try_lock()) {
                counter[i].unlock();
            } else {
                counterIsEmpty = false;
            }
        }

        if (counterIsEmpty) {
            if (servedDrunkard.try_lock()) {
                rest();
                servedDrunkard.unlock();
            } else {
                counterIsEmpty = false;
            }
        }


        if(servedDrunkard.try_lock()){
            servedDrunkard.unlock();
        }else{
            serveDrunkard();
        }

    }

}


Barman::Barman(Glass (&glassArray)[NUMOFGLASSES]) : allGlasses(glassArray), barmanThread(&Barman::checkCounter, this) {}

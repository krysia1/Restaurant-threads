//Autor: Krystyna Łosieczka
//Opis: Klasa Barman reprezentująca Barmana

#ifndef SO2P_BARMAN_H
#define SO2P_BARMAN_H

#include "Glass.h"
#include "Waiter.h"
#include "Client.h"
#include "Cook.h"

#include <array>
#include <thread>
#include <random>




class Barman{
public:

    Glass (&allGlasses)[NUMOFGLASSES];  //referencja na liste szklanek
    int pickedGlasses = 0;  //liczba trzymanych przez barmana szklanek
    int dirtyGlasses = 0; //liczba brudnych szklanek
    bool counterIsEmpty = true; //zmienna do sprawdzania, czy jest ktoś do obsłużenia przy kontuarze
    std::thread barmanThread;   //wątek barmana
    bool exitBarman = false; //czy kończymy program
    int progress;
    std::mt19937 rng{std::random_device{}()};
    bool askToClean = false;
    int state = 0; //na początku stan na czekanie


//    Barman(Glass (&glassArray)[NUMOFGLASSES])
//        : allGlasses(glassArray), barmanThread(&Barman::checkCounter, this) {}

    Barman(Glass (&allGlasses)[3]);

    void checkCounter();
    void serveDrunkard();
    void tryGlasses();
    void checkGlasses();
    void rest();
};

#endif //SO2P_BARMAN_H

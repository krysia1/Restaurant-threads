//Autor: Krystyna Łosieczka
//Opis: Klasa Barman reprezentująca Barmana

#ifndef SO2P_BARMAN_H
#define SO2P_BARMAN_H

#include <array>
#include "Glass.h"
#include <thread>
#include <random>

#define COUNTERCAPACITY 3
#define NUMOFGLASSES 3

class Barman{
public:

    Glass (&allGlasses)[NUMOFGLASSES];  //referencja na liste szklanek
    int pickedGlasses = 0;  //liczba trzymanych przez barmana szklanek
    int dirtyGlasses = 0; //liczba brudnych szklanek
    bool counterIsEmpty = true; //zmienna do sprawdzania, czy jest ktoś do obsłużenia przy kontuarze
    std::thread barmanThread;   //wątek barmana
    bool exit = false; //czy kończymy program
    int progress;
    std::mt19937 rng{std::random_device{}()};
    bool askToClean = false;


    Barman(Glass (&glassArray)[NUMOFGLASSES]);

    void checkCounter( bool counterIsEmpty);
    void serveDrunkard();
    void tryGlasses();
    void checkGlasses();
    void rest();
};

#endif //SO2P_BARMAN_H

//Autor: Krystyna Łosieczka
//Opis: Klasa Waiter reprezentująca Kelnera

#ifndef SO2P_WAITER_H
#define SO2P_WAITER_H

#include <mutex>
#include "Barman.h"


class Waiter {
public:
    int waiterId;
    bool inService = false;
    Barman &barman;
    bool exit = false;
    Glass (&allGlasses)[NUMOFGLASSES];  //referencja na liste szklanek
    int progress;
    std::mt19937 rng{std::random_device{}()};

    std::mutex waiterMutex;
    std::thread waiterThread;

    Waiter(Barman &barman, Glass (&glassArray)[NUMOFGLASSES])
        : barman(barman), allGlasses(glassArray), waiterThread(&Waiter::work,this) {}

    void work();
    void clean();
    void serve();
};


#endif //SO2P_WAITER_H

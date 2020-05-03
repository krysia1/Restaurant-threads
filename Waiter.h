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

    std::mutex waiterMutex;
    std::thread waiterThread;

    Waiter(Barman &barman)
        : barman(barman), waiterThread(&Waiter::work,this) {}

    void work();
    void clean();
};


#endif //SO2P_WAITER_H

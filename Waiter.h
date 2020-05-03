//Autor: Krystyna Łosieczka
//Opis: Klasa Waiter reprezentująca Kelnera

#ifndef SO2P_WAITER_H
#define SO2P_WAITER_H

#include <mutex>


class Waiter {
public:
    int waiterId;
    bool inService = false;

    std::mutex waiterMutex;

};


#endif //SO2P_WAITER_H

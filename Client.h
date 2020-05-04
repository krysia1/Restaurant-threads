//Autor: Krystyna Łosieczka
//Opis: Klasa Client reprezentująca Klienta (zwykłego lub Pijaka) - wątek

#ifndef SO2P_CLIENT_H
#define SO2P_CLIENT_H

#include <random>
#include <thread>

#include "Barman.h"

class Client{
public:
    int clientId;
    std::mt19937 rng{std::random_device{}()};   //generator liczb losowych - do losowania czy klient czy pijak
    std::thread clientThread;



    Client(int const clientId)
    : clientId(clientId), clientThread(&Client::visitRestaurant, this){}

    void takeASeat();
    void eat();
    void useToilet();

    void seatByTheCounter();
    void drink();

    void pay();
    void leave();

    void visitRestaurant();

};


#endif //SO2P_CLIENT_H

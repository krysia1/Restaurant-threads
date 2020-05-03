//Autor: Krystyna Łosieczka
//Opis: Klasa Client reprezentująca Klienta (zwykłego lub Pijaka) - wątek

#ifndef SO2P_CLIENT_H
#define SO2P_CLIENT_H

#include <random>

class Client{
public:
    int clientId;
    std::mt19937 rng{std::random_device{}()};   //generator liczb losowych - do losowania czy klient czy pijak

    void takeASeat();
    void eat();
    void pay();
    void useToilet();
    void leave();

    void seatByTheCounter();
    void drink();
};


#endif //SO2P_CLIENT_H

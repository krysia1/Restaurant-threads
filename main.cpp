//Autor: Krystyna Łosieczka
//Temat: Restauracja

#include "Glass.h"
#include "Barman.h"
#include "Waiter.h"
#include "Client.h"
#include "Cook.h"

#include <array>
#include <iostream>
#include <mutex>
#include <vector>
#include <iomanip>
#include <memory>


#define NUMOFGLASSES 3
#define NUMOFCLIENTS 5
#define NUMOFWAITERS 3

std::vector<Waiter *> waiters;
std::vector<Client *> clients;
std::vector<Cook *> cooks;

std::mutex knifeMutex;

bool stop = false;




int main() {

    std::array<Glass, NUMOFGLASSES> glasses;

    for (int i=0; i < NUMOFGLASSES; i++){
        glasses.at(i).glassId = i+1;
    }

    for (auto i=0; i<NUMOFCLIENTS; i++){
        Client *c = new Client (i);
        clients.push_back(c);
    }

//    for (auto i=0; i<NUMOFWAITERS; i++){
//        Waiter *w = new Waiter ();
//        waiters.push_back(w);
//    }

    std::this_thread::sleep_for(std::chrono::seconds(1));



    return 0;
}
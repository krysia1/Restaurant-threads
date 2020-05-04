//Autor: Krystyna Łosieczka
//Opis: Klasa Client reprezentująca Klienta (zwykłego lub Pijaka)

#include "Client.h"

void Client::seatByTheCounter(){};
void Client::takeASeat(){};
void Client::leave(){};

void Client::visitRestaurant() {

    int decision = (std::rand() % 99) + 1;

    if(decision > 70){
        for(int i=0; i<COUNTERCAPACITY; i++){
            if(counter[i].try_lock()){
                seatByTheCounter();
                counter[i].unlock();
                break;
            }
        }

    } else {
        takeASeat();
    }

    leave();


}
//Autor: Krystyna Łosieczka
//Opis: Klasa Client reprezentująca Klienta (zwykłego lub Pijaka)

#include "Client.h"
#include "Waiter.h"



void Client::seatByTheCounter(){

    if(servedDrunkard.try_lock()){

    }

};


void Client::takeASeat(){};


void Client::leave(){
    clientPurpose = 0;
};


void Client::visitRestaurant() {
    while(!exitClient) {

        int decision = (std::rand() % 99) + 1;

        if (decision > 70) {
            for (int i = 0; i < COUNTERCAPACITY; i++) {
                if (counter[i].try_lock()) {
                    clientPurpose = 1;
                    seatByTheCounter();
                    counter[i].unlock();
                    break;
                }
            }

        } else {
            for (int i = 0; i < RESTAURANTCAPACITY; i++) {
                if (restaurantTables[i].try_lock()) {
                    clientPurpose = 2;
                    takeASeat();
                    restaurantTables[i].unlock();
                    break;
                }
            }

        }

        leave();
    }

}
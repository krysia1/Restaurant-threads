//Autor: Krystyna Łosieczka
//Opis: Klasa Waiter reprezentująca Kelnera

#include "Waiter.h"
#include <thread>

void Waiter::work(){}


void Waiter::clean(){
    if(barman.askToClean){}

}


Waiter::Waiter(Barman &barman) : barman(barman), waiterThread(&Waiter::work,this) {}
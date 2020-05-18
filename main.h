

#ifndef SO2P_MAIN_H
#define SO2P_MAIN_H


#include <iostream>
#include <mutex>
#include <vector>

#define COUNTERCAPACITY 3
#define RESTAURANTCAPACITY 5

#define NUMOFGLASSES 3
#define NUMOFWAITERS 3
#define NUMOFCLIENTS 10

std::mutex changeStatusMutex; //mutex do zmiany statusu

std::mutex restaurant[RESTAURANTCAPACITY]; //miejsca przy stolikach jako mutexy
std::mutex restaurantQueue[RESTAURANTCAPACITY]; //kolejka do obsługi klientów

std::vector<int> restaurantStatus; //wektor do wyswietlania statusu klienta

std::mutex counter[COUNTERCAPACITY]; //miejsca przy kontuarze jako mutexy
std::mutex servedDrunkard; //aktualnie obsługiwany pijak
std::mutex counterQueue[COUNTERCAPACITY]; //kolejka do obługi pijakow

std::vector<int> counterStatus; //vector do wyświetlania statusu pijaka ([0]-aktualnie obsługiwany)

std::vector<bool> amIFull;

bool stop = false;
int drinkingDrunkard;
bool amIDrunk = false;

bool isCounterTaken = false;




#endif //SO2P_MAIN_H



#ifndef SO2P_MAIN_H
#define SO2P_MAIN_H


#include <iostream>
#include <mutex>
#include <vector>

#define COUNTERCAPACITY 3
#define RESTAURANTCAPACITY 5
#define QUEUECAPACITY 5
#define TOILETCAPACITY 2

#define NUMOFGLASSES 3
#define NUMOFWAITERS 3
#define NUMOFCLIENTS 10
#define NUMOFTOILETS 2
#define NUMOFKNIVES 1
#define NUMOFCOOKS 2

std::mutex changeStatusMutex; //mutex do zmiany statusu
std::mutex changeScreenMutex; //mutex do zmian na ekranie

std::mutex restaurant[RESTAURANTCAPACITY]; //miejsca przy stolikach jako mutexy
std::mutex restaurantQueue[RESTAURANTCAPACITY]; //kolejka do obsługi klientów

//std::vector<int> restaurantStatus; //wektor do wyswietlania statusu klienta

std::mutex counter[COUNTERCAPACITY]; //miejsca przy kontuarze jako mutexy
std::mutex servedDrunkard; //aktualnie obsługiwany pijak
std::mutex counterQueue[COUNTERCAPACITY]; //kolejka do obługi pijakow

std::mutex toiletOneQueue[QUEUECAPACITY];
std::mutex toiletTwoQueue[QUEUECAPACITY];

//std::vector<int> counterStatus; //vector do wyświetlania statusu pijaka ([0]-aktualnie obsługiwany)



std::vector<bool> amIFull;

bool suppliesAlert = false;
std::mutex suppliesAlertMutex;

bool stop = false;
int drinkingDrunkard;
bool amIDrunk = false;

bool isCounterTaken = false;

int h;
int w;
int x;
int y;
const int max_len = 50;




#endif //SO2P_MAIN_H

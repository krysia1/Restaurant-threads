//Autor: Krystyna Łosieczka
//Opis: Klasa Glass reprezentująca Szklanke - zasób

#ifndef SO2P_GLASS_H
#define SO2P_GLASS_H

#include "Barman.h"

#include <mutex>

class Glass{
public:
    int glassId;
    bool taken = false;
    bool dirty = false;
    int state = 0; //stan na początku na czysty
    std::mutex glassMutex;

};

#endif //SO2P_GLASS_H

//Autor: Krystyna Łosieczka
//Opis: Klasa Glass reprezentująca Szklanke - zasób

#ifndef SO2P_GLASS_H
#define SO2P_GLASS_H

#include <mutex>

class Glass{
public:
    int glassId;
    bool taken = false;
    bool dirty = false;
    std::mutex glassMutex;

};

#endif //SO2P_GLASS_H

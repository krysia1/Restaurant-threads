//Autor: Krystyna Łosieczka
//Opis: Klasa Knife reprezentująca Nóż

#ifndef SO2P_KNIFE_H
#define SO2P_KNIFE_H

#include <mutex>

class Knife{
public:
    int ownerId;
    bool used = false;

    std::mutex knifeMutex;

    void tryUse();
    void endUse();

};

#endif //SO2P_KNIFE_H
